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
	/* used whenever a primary event was the entry point, thus encapsulating a single undo point */
	event_bundle_t temp_primary_events;
	// TODO(undo): once old system is kaput, we will be able to store the undo point
	//             description in the event_bundle
	str_t last_event_desc;
	b32 undoing;
	event_bundle_t *redoing_bundle;
	u8 current_nest_level;
} transaction_system_t;

transaction_system_t transaction_system_create(allocator_t *alc);
void transaction_system_destroy(transaction_system_t *sys);
void transaction_system_set_active(transaction_system_t *sys);
void transaction_spawn_store(const store_metadata_t *meta, u32 kind);
void *transaction_spawn_event(const event_metadata_t *meta, const char *nav_description, u32 kind);
// TODO(undo): make static once old system is kaput; can also change return to void
/* Returns a nonzero event kind for executed, non-secondary events */
u32 transaction__flush(event_t *event);
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
		.temp_primary_events = (event_bundle_t) {
			.d = array_create_ex(alc),
			.secondary = false,
		},
		.undoing = false,
		.redoing_bundle = NULL,
		.current_nest_level = EVENT_NEST_LEVEL_INVALID,
	};
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
	event_bundle_destroy(&sys->temp_primary_events,   sys->alc);

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
	for (u32 i = array_sz(bundle->d); i-- > 0; ) {
		event_t *event_i = bundle->d[i];
		event_undo(event_i);
		/* Discriminately remove nested events */
		if (event_i->nest_level != EVENT_NEST_LEVEL_NONE) {
			array_remove(bundle->d, i);
			event_destroy(event_i, sys->alc);
		}
	}
	sys->undoing = false;
}

static
void transaction__redo_bundle(transaction_system_t *sys, event_bundle_t *bundle)
{
	sys->redoing_bundle = bundle;
	/* Allowing a bundle with multiple non-nested events to redo in-place
	 * would screw up the event ordering. Thus, we make a copy of the non-nested
	 * events and process them one at a time, in the original sequence. */
	event_bundle_t temp_bundle = {0};
	event_bundle_copy(&temp_bundle, bundle);
	array_clear(bundle->d);

	for (u32 i = 0; i < array_sz(temp_bundle.d); ++i) {
		assert(temp_bundle.d[i]->nest_level == EVENT_NEST_LEVEL_NONE);
		transaction__handle_ordinary_event(sys, temp_bundle.d[i]);
	}

	/* As tempting as it is, we dare not destroy the temp_bundle itself,
	 * because it never owns the events stored in it. */
	array_destroy(temp_bundle.d);
	sys->redoing_bundle = NULL;
}

static
b32 event_undo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, true);

	if (!bundle)
		return false;

	event_bundle_unwind(&sys->temp_secondary_events, true, sys->alc);

	if (bundle != &array_last(sys->event_history)) {
		event_bundle_t *bundle_after = bundle+1;
		if (bundle_after->secondary)
			transaction__undo_bundle(sys, bundle_after);
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

	if (!bundle)
		return false;

	event_bundle_unwind(&sys->temp_secondary_events, true, sys->alc);

	if (bundle != &array_first(sys->event_history)) {
		event_bundle_t *bundle_before = bundle-1;
		if (   bundle_before->secondary
		    && bundle_before != &array_first(sys->event_history))
			transaction__redo_bundle(sys, bundle_before);
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
	assert(array_empty(sys->temp_primary_events.d));

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
void transaction__push_event_bundle(transaction_system_t *sys, event_bundle_t *temp_bundle)
{
	if (array_empty(temp_bundle->d))
		return;

	event_bundle_t bundle = {0};
	event_bundle_copy(&bundle, temp_bundle);
	array_append(sys->event_history, bundle);
	array_clear(temp_bundle->d);
}

static
u32 transaction__handle_ordinary_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	if (event->nest_level != EVENT_NEST_LEVEL_NONE) {
		/* Use the appropriate in-use bundle */
		event_bundle_t *bundle = NULL;
		if (sys->redoing_bundle)
			bundle = sys->redoing_bundle;
		else if (array_empty(sys->temp_primary_events.d))
			bundle = &sys->temp_secondary_events;
		else
			bundle = &sys->temp_primary_events;

		array_append(bundle->d, event);

		if (event_execute(event))
			result = event->kind;
		else
			event_bundle_unwind_to(bundle, event, true, sys->alc);

	} else if (event->meta->secondary) {
		event_bundle_t *bundle = sys->redoing_bundle
		                       ? sys->redoing_bundle
		                       : &sys->temp_secondary_events;
		event_t *last_event = event_bundle_last_non_nested_event(bundle);
		array_append(bundle->d, event);

		if (event_execute(event)) {
			if (   event->meta->multi_frame
			    && last_event
			    && last_event->kind == event->kind) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_bundle_unwind_to(bundle, event, false, sys->alc);
			} else {
				/* Handle fresh secondary event */
				result = event->kind;
			}
		} else {
			event_bundle_unwind_to(bundle, event, true, sys->alc);
		}
	} else {
		event_bundle_t *bundle = sys->redoing_bundle
		                       ? sys->redoing_bundle
		                       : &sys->temp_primary_events;
		event_t *last_event = array_empty(sys->event_history)
		                    ? NULL
		                    : event_bundle_last_non_nested_event(&array_last(sys->event_history));
		array_append(bundle->d, event);

		if (event_execute(event)) {
			if (   event->meta->multi_frame
			    && array_empty(sys->temp_secondary_events.d)
			    && last_event
			    && last_event->kind == event->kind
			    && (  event->time_since_epoch_ms - last_event->time_since_epoch_ms
			        < TRANSACTION_MULTIFRAME_TIMEOUT)) {
				/* Handle continued multi-frame event */
				// VERIFY(undo): is it sufficient to only update the "head" of the last bundle
				//               and none of its nested events??
				event_update(last_event, event);
				event_bundle_unwind_to(bundle, event, false, sys->alc);
			} else {
				/* Handle fresh primary event; corresponds to an undo point */
				transaction__push_event_bundle(sys, &sys->temp_secondary_events);
				transaction__push_event_bundle(sys, bundle);
				result = event->kind;

				str_clear(&sys->last_event_desc);
				transaction__set_event_description(&sys->last_event_desc,
				                                   array_last(array_last(sys->event_history).d));
			}
		} else {
			event_bundle_unwind_to(bundle, event, true, sys->alc);
		}
		assert(array_empty(bundle->d));
	}

	return result;
}

u32 transaction__flush(event_t *event)
{
	u32 result = EVENT_KIND_NOOP;
	transaction_system_t *sys = g_active_transaction_system;

	if (sys->undoing)
		/* While undoing, prevent nested events from executing, since they will be handled
		 * by explicit calls to their undo handlers. */
		goto out;

	sys->current_nest_level += 1;
	assert(sys->current_nest_level != EVENT_NEST_LEVEL_INVALID);

	event->nest_level = sys->current_nest_level;

	if (event->kind < 3) {
		result = transaction__handle_priority_event(sys, event);
	} else {
		const u32 tmp = transaction__handle_ordinary_event(sys, event);
		if (tmp != EVENT_KIND_NOOP && !event->meta->secondary)
			result = tmp;
	}

	sys->current_nest_level -= 1;

out:
	return result;
}

transaction_system_t *get_active_transaction_system()
{
	return g_active_transaction_system;
}

#undef TRANSACTION_IMPLEMENTATION
#endif // TRANSACTION_IMPLEMENTATION
