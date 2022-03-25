#ifndef VIOLET_TRANSACTION_H
#define VIOLET_TRANSACTION_H

#define TRANSACTION_MULTIFRAME_TIMEOUT 500

typedef struct transaction_system {
	allocator_t *alc;
	array(store_t *) stores;
	event_t *queued_event;
	/* append-only stack of event history */
	array(event_bundle_t) event_history;
	/* secondary events awaiting confirmation by a primary event */
	event_bundle_t temp_secondary_events;
	// TODO(undo): once old system is kaput, we will be able to store the undo point
	//             description in the event_bundle
	str_t last_event_desc;
} transaction_system_t;

transaction_system_t transaction_system_create(allocator_t *alc);
void transaction_system_destroy(transaction_system_t *sys);
void transaction_system_set_active(transaction_system_t *sys);
void transaction_system_on_update();
void transaction_spawn_store(const store_metadata_t *meta, u32 kind);
void *transaction_spawn_event(const event_metadata_t *meta, const char *nav_description, u32 kind);
// TODO(undo): make static once old system is kaput; can also change return to void
/* Returns a nonzero event kind for executed, non-secondary events */
u32 transaction__flush();
// TODO(undo): should not be required once old system is kaput
transaction_system_t *get_active_transaction_system();

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
	};
}

void transaction_system_destroy(transaction_system_t *sys)
{
	array_foreach(sys->stores, store_t *, store_ptr)
		store_destroy(*store_ptr, sys->alc);
	array_destroy(sys->stores);

	if (sys->queued_event)
		event_destroy(sys->queued_event, sys->alc);

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
const event_bundle_t *transaction__last_valid_event_bundle(transaction_system_t *sys, b32 undoing)
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
void transaction__unwind_event_bundle(event_bundle_t *bundle, allocator_t *alc)
{
	for (u32 i = array_sz(bundle->d); i-- > 0; ) {
		event_undo(bundle->d[i]);
		event_destroy(bundle->d[i], alc);
	}
	array_clear(bundle->d);
}

static
b32 event_undo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	const event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, true);

	if (!bundle)
		return false;

	transaction__unwind_event_bundle(&sys->temp_secondary_events, sys->alc);

	if (bundle != &array_last(sys->event_history)) {
		/* there are secondary events to undo, in reverse insertion order */
		const event_bundle_t *bundle_after = bundle+1;
		if (bundle_after->secondary)
			for (u32 i = array_sz(bundle_after->d); i-- > 0; )
				event_undo(bundle_after->d[i]);
	}

	event_undo(bundle->d[0]);
	transaction__set_event_description(&event->label, bundle->d[0]);
	return true;
}

static
b32 event_redo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	const event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, false);

	if (!bundle)
		return false;

	transaction__unwind_event_bundle(&sys->temp_secondary_events, sys->alc);

	if (bundle != &array_first(sys->event_history)) {
		const event_bundle_t *bundle_before = bundle-1;
		if (   bundle_before->secondary
		    && bundle_before != &array_first(sys->event_history))
			for (u32 i = 0; i < array_sz(bundle_before->d); ++i)
				event_execute(bundle_before->d[i]);
	}

	event_execute(bundle->d[0]);
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
	assert(!sys->queued_event);
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

	sys->queued_event = event_create(kind, instance_size, meta, nav_description, sys->alc);
	return sys->queued_event->instance;
}

static
u32 transaction__handle_priority_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	if (event_execute(event)) {
		// NOTE(luke): priority events are never secondary
		array_append(sys->event_history, event_bundle_create(event, sys->alc));
		result = event->kind;
	}
	else {
		event_destroy(event, sys->alc);
	}

	return result;
}

static
b32 transaction__multi_frame_events_mergeable(const event_t *event,
                                              const event_t *last_event)
{
	return last_event->kind == event->kind
	    && (  event->time_since_epoch_ms - last_event->time_since_epoch_ms
	        < TRANSACTION_MULTIFRAME_TIMEOUT);
}

static
u32 transaction__handle_ordinary_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	if (event_execute(event)) {
		if (event->meta->secondary) {
			event_bundle_t *bundle = &sys->temp_secondary_events;
			event_t *last_event = array_empty(bundle->d)
		                        ? NULL
		                        : array_last(bundle->d);

			if (   event->meta->multi_frame
			    && last_event
			    && transaction__multi_frame_events_mergeable(event, last_event)) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh secondary event */
				array_append(bundle->d, event);
				result = event->kind;
			}
		} else {
			event_t *last_event = array_empty(sys->event_history)
			                    ? NULL
			                    : array_last(array_last(sys->event_history).d);

			if (   event->meta->multi_frame
			    && array_empty(sys->temp_secondary_events.d)
			    && last_event
			    && transaction__multi_frame_events_mergeable(event, last_event)) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh primary event */
				if (!array_empty(sys->temp_secondary_events.d)) {
					event_bundle_t bundle_secondary;
					event_bundle_copy(&bundle_secondary, &sys->temp_secondary_events);
					array_append(sys->event_history, bundle_secondary);
					array_clear(sys->temp_secondary_events.d);
				}
				const event_bundle_t bundle_primary = event_bundle_create(event, sys->alc);
				array_append(sys->event_history, bundle_primary);
				str_clear(&sys->last_event_desc);
				transaction__set_event_description(&sys->last_event_desc, bundle_primary.d[0]);
				result = event->kind;
			}
		}
	} else {
		event_destroy(event, sys->alc);
	}

	return result;
}

u32 transaction__flush()
{
	u32 result = EVENT_KIND_NOOP;
	transaction_system_t *sys = g_active_transaction_system;
	event_t *event = sys->queued_event;

	if (!event)
		return result;

	if (event->kind < 3) {
		result = transaction__handle_priority_event(sys, event);
	} else {
		const u32 tmp = transaction__handle_ordinary_event(sys, event);
		if (tmp != EVENT_KIND_NOOP && !event->meta->secondary)
			result = tmp;
	}

	sys->queued_event = NULL;
	return result;
}

transaction_system_t *get_active_transaction_system()
{
	return g_active_transaction_system;
}

void transaction_system_on_update()
{
	transaction__flush();
}

#undef TRANSACTION_IMPLEMENTATION
#endif // TRANSACTION_IMPLEMENTATION
