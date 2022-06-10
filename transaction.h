#ifndef VIOLET_TRANSACTION_H
#define VIOLET_TRANSACTION_H

#define TRANSACTION_MULTIFRAME_TIMEOUT 500

typedef struct transaction_system {
	allocator_t *alc;
	array(store_t *) stores;
	/* append-only stack of event history */
	array(event_bundle_t) event_history;
	/* secondary events awaiting confirmation by a primary event */
	event_bundle_t temp_secondary_events;
	// TODO(undo): once old system is kaput, we will be able to store the undo point
	//             description in the event_bundle
	str_t last_event_desc;
	b32 undoing;
	event_t *active_parent;
} transaction_system_t;

transaction_system_t transaction_system_create(allocator_t *alc);
void transaction_system_reset(transaction_system_t *sys);
void transaction_system_destroy(transaction_system_t *sys);
void transaction_system_set_active(transaction_system_t *sys);
void transaction_spawn_store(const store_metadata_t *meta, u32 kind);
void *transaction_spawn_event(const event_metadata_t *meta, const char *nav_description, u32 kind);
// TODO(undo): make static once old system is kaput; can also change return to void
/* Returns a nonzero event kind for executed, non-secondary events */
u32 transaction__flush(event_t *event);
// TODO(undo): should not be required once old system is kaput
transaction_system_t *get_active_transaction_system(void);

void *store_data(u32 kind);

typedef struct event_undo_redo
{
	str_t label;
} event_undo_redo_t;

// TODO(undo): would be nice if the undo/redo implementation specifics could be hidden
//             from the global namespace
extern const event_contract_t event_undo__contract;
extern const event_contract_t event_redo__contract;
extern void event_undo_redo__create(void *instance, allocator_t *alc);

#endif // VIOLET_TRANSACTION_H

/* Implementation */

#ifdef TRANSACTION_IMPLEMENTATION

transaction_system_t *g_active_transaction_system = NULL;

static
u32 transaction__handle_ordinary_event(transaction_system_t *sys, event_t *event);

static
store_t *store__from_kind(u32 kind)
{
	array_foreach(g_active_transaction_system->stores, store_t *, store_ptr) {
		if ((*store_ptr)->kind == kind)
			return *store_ptr;
	}
	assert(false);
	return NULL;
}

void *store_data(u32 kind)
{
	const store_t *store = store__from_kind(kind);
	return store->instance;
}

transaction_system_t transaction_system_create(allocator_t *alc)
{
	return (transaction_system_t) {
		.alc = alc,
		.stores          = array_create_ex(alc),
		.event_history   = array_create_ex(alc),
		.last_event_desc = str_create(alc),
		.temp_secondary_events = (event_bundle_t) {
			.d = array_create_ex(alc),
			.secondary = true,
		},
		.undoing = false,
		.active_parent = NULL,
	};
}

/* store data is NOT reset */
void transaction_system_reset(transaction_system_t *sys)
{
	array_foreach(sys->event_history, event_bundle_t, bundle)
		event_bundle_destroy(bundle, sys->alc);
	array_clear(sys->event_history);

	event_bundle_clear(&sys->temp_secondary_events, sys->alc);

	str_clear(&sys->last_event_desc);
}

void transaction_system_destroy(transaction_system_t *sys)
{
	array_foreach(sys->stores, store_t *, store_ptr)
		store_destroy(*store_ptr, sys->alc);
	array_destroy(sys->stores);

	array_foreach(sys->event_history, event_bundle_t, bundle)
		event_bundle_destroy(bundle, sys->alc);
	array_destroy(sys->event_history);

	event_bundle_destroy(&sys->temp_secondary_events, sys->alc);

	str_destroy(&sys->last_event_desc);
}

void transaction_system_set_active(transaction_system_t *sys)
{
	g_active_transaction_system = sys;
}

static
event_bundle_t *transaction__last_valid_event_bundle(transaction_system_t *sys, b32 undoing)
{
	u32 undo_count = 0, redo_count = 0, real_event_count = 0;

	for (u32 i = array_sz(sys->event_history); i-- > 0; ) {
		event_bundle_t *bundle = &sys->event_history[i];

		if (bundle->secondary)
			continue;

		switch (bundle->d[0]->kind) {
		case EVENT_KIND_UNDO:
			undo_count++;
		break;
		case EVENT_KIND_REDO:
			redo_count++;
		break;
		default:
			real_event_count++;

			if (   ( undoing && real_event_count >  undo_count - redo_count)
			    || (!undoing && real_event_count == undo_count - redo_count))
				return bundle;

			if (redo_count >= undo_count)
				/* nothing else to redo */
				return NULL;
		}
	}

	return NULL;
}

static
void transaction__set_event_description(str_t *description, const event_t *event)
{
	if (event->nav_description[0]) {
		str_cat(description, event->nav_description);
		str_cat(description, " - ");
	}
	str_cat(description, event->meta->description);
}

void event_undo_redo__create(void *instance, allocator_t *alc)
{
	event_undo_redo_t *event = (event_undo_redo_t *)instance;
	event->label = str_create(alc);
}

static
void event_undo_redo__destroy(event_undo_redo_t *event, allocator_t *alc)
{
	str_destroy(&event->label);
}

static
void transaction__undo_bundle(transaction_system_t *sys, event_bundle_t *bundle)
{
	sys->undoing = true;
	for (u32 i = array_sz(bundle->d); i-- > 0; )
		event_undo(bundle->d[i], sys->alc);
	sys->undoing = false;
	bundle->undone = true;
}

static
void transaction__redo_bundle(transaction_system_t *sys, event_bundle_t *bundle)
{
	array_foreach(bundle->d, event_t *, event) {
		sys->active_parent = *event;
		if ((*event)->meta->contract->update_pre)
			(*event)->meta->contract->update_pre((*event)->instance, NULL);
		event_execute(*event);
		sys->active_parent = NULL;
	}
	bundle->undone = false;
}

static
b32 event_undo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, true);
	event_bundle_t *last   = &array_last(sys->event_history);

	if (!bundle)
		return false;

	event_bundle_unwind(&sys->temp_secondary_events, sys->alc);

	/* There can potentially be several secondary bundles in sequence after the undo point */
	event_bundle_t *tail = bundle;
	while (tail != last) {
		if ((tail+1)->secondary && !(tail+1)->undone)
			tail++;
		else
			break;
	}
	while (tail != bundle) {
		transaction__undo_bundle(sys, tail);
		tail--;
	}

	/* Handle the undo point */
	transaction__undo_bundle(sys, bundle);
	transaction__set_event_description(&event->label, bundle->d[0]);
	return true;
}

static
b32 event_redo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, false);
	event_bundle_t *first  = &array_first(sys->event_history);

	if (!bundle)
		return false;

	event_bundle_unwind(&sys->temp_secondary_events, sys->alc);

	/* There can potentially be several secondary bundles in sequence before the undo point */
	event_bundle_t *head = bundle;
	while (head != first) {
		if ((head-1)->secondary && (head-1)->undone)
			head--;
		else
			break;
	}
	while (head != bundle) {
		transaction__redo_bundle(sys, head);
		head++;
	}

	/* Handle the undo point */
	transaction__redo_bundle(sys, bundle);
	transaction__set_event_description(&event->label, bundle->d[0]);
	return true;
}

const event_contract_t event_undo__contract = {
	.destroy = (void (*)(void *, allocator_t *))event_undo_redo__destroy,
	.execute = (b32 (*)(const void *))event_undo__execute,
};

const event_contract_t event_redo__contract = {
	.destroy = (void (*)(void *, allocator_t *))event_undo_redo__destroy,
	.execute = (b32 (*)(const void *))event_redo__execute,
};

void transaction_spawn_store(const store_metadata_t *meta, u32 kind)
{
	transaction_system_t *sys = g_active_transaction_system;

	/* having two or more stores with the same kind could wreak havoc */
	for (u32 i = 0, n = array_sz(sys->stores); i < n; ++i)
		assert(sys->stores[i]->kind != kind);

	void *instance = meta->spawner(sys->alc);
	store_t * store = store_create(kind, instance, meta, sys->alc);
	array_append(sys->stores, store);
}

void *transaction_spawn_event(const event_metadata_t *meta, const char *nav_description, u32 kind)
{
	transaction_system_t *sys = g_active_transaction_system;
	u32 instance_size = 0;

	switch (kind) {
	case EVENT_KIND_NOOP:
		assert(false);
	break;
	case EVENT_KIND_UNDO:
	case EVENT_KIND_REDO:
		instance_size = sizeof(event_undo_redo_t);
	break;
	default:
		instance_size = meta->size;
	}

	event_t *event = event_create(kind, instance_size, meta, nav_description, sys->alc);
	return event->instance;
}

static
u32 transaction__handle_priority_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	assert(!event->meta->contract->update_pre);

	if (event_execute(event)) {
		/* Priority events are never secondary, nor are they ever nested */
		array_append(sys->event_history, event_bundle_create(event, sys->alc));
		result = event->kind;
	}
	else {
		event_destroy(event, sys->alc);
	}
	return result;
}

static
u32 transaction__handle_child_event(transaction_system_t *sys, event_t *event, event_t *parent)
{
	u32 result = EVENT_KIND_NOOP;
	sys->active_parent = event;

	assert(!event->meta->contract->update_pre);

	if (event_execute(event)) {
		array_append(parent->children, event);
		result = event->kind;
	} else {
		event_unwind_children(event, sys->alc);
		event_destroy(event, sys->alc);
	}

	sys->active_parent = parent;
	return result;
}

static
void transaction__push_temp_bundle(transaction_system_t *sys, event_bundle_t *temp_bundle)
{
	if (array_empty(temp_bundle->d))
		return;

	event_bundle_t bundle = {0};
	event_bundle_copy(&bundle, temp_bundle);
	array_append(sys->event_history, bundle);
	array_clear(temp_bundle->d);
}

static
event_t *transaction__last_event(transaction_system_t *sys)
{
	if (!array_empty(sys->temp_secondary_events.d))
		return array_last(sys->temp_secondary_events.d);
	else if (!array_empty(sys->event_history))
		return array_last(array_last(sys->event_history).d);
	return NULL;
}

static
b32 transaction__events_mergeable(const event_t *lhs, const optional(event_t) rhs)
{
	return rhs
	    && lhs->meta->multi_frame
	    && lhs->kind == rhs->kind
	    && (  lhs->meta->secondary
	        ? array_empty(lhs->children)
	        :   lhs->time_since_epoch_ms - rhs->time_since_epoch_ms
	          < TRANSACTION_MULTIFRAME_TIMEOUT);
}

static
u32 transaction__handle_ordinary_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;
	sys->active_parent = event;

	event_t *last_event = transaction__last_event(sys);
	const b32 mergeable = transaction__events_mergeable(event, last_event);

	if (event->meta->contract->update_pre)
		event->meta->contract->update_pre(event->instance,
		                                  mergeable ? last_event->instance : NULL);

	if (event_execute(event)) {
		if (event->meta->secondary) {
			if (mergeable) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh secondary event */
				array_append(sys->temp_secondary_events.d, event);
				result = event->kind;
			}
		} else {
			if (mergeable) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_unwind_children(event, sys->alc);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh primary event; corresponds to an undo point */
				transaction__push_temp_bundle(sys, &sys->temp_secondary_events);
				array_append(sys->event_history, event_bundle_create(event, sys->alc));
				result = event->kind;

				str_clear(&sys->last_event_desc);
				transaction__set_event_description(&sys->last_event_desc,
				                                   array_last(array_last(sys->event_history).d));
			}
		}
	} else {
		if (mergeable)
			last_event->time_since_epoch_ms = event->time_since_epoch_ms;
		event_unwind_children(event, sys->alc);
		event_destroy(event, sys->alc);
	}

	sys->active_parent = NULL;
	return result;
}

u32 transaction__flush(event_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;

	if (sys->undoing) {
		/* While undoing, prevent nested events from executing, since they should have already
		 * been handled by explicit calls to their undo handlers. */
		event_destroy(event, sys->alc);
		return EVENT_KIND_NOOP;
	}

	if (event->kind < 3)
		return transaction__handle_priority_event(sys, event);
	else if (sys->active_parent)
		return transaction__handle_child_event(sys, event, sys->active_parent);
	else
		return transaction__handle_ordinary_event(sys, event);
}

transaction_system_t *get_active_transaction_system(void)
{
	return g_active_transaction_system;
}

#undef TRANSACTION_IMPLEMENTATION
#endif // TRANSACTION_IMPLEMENTATION
