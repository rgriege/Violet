#ifndef VIOLET_TRANSACTION_H
#define VIOLET_TRANSACTION_H

typedef struct transaction_system {
	allocator_t *alc;
	array(store_t *) stores;
	array(event_t *) event_queue;
	/* append-only stack of event history */
	array(event_bundle_t) event_history;
} transaction_system_t;

transaction_system_t transaction_system_create(allocator_t *alc);
void transaction_system_destroy(transaction_system_t *sys);
void transaction_system_set_active(transaction_system_t *sys);
void transaction_system_on_update();
void transaction_spawn_store(const store_metadata_t *meta, u32 kind);
void *transaction_spawn_event(const event_metadata_t *meta, u32 kind);
// TODO(undo): make static once old system is kaput; can also change return to void
/* returns nonzero value corresponding to an event_kind_e that was accomplished */
u32 transaction__flush();
// TODO(undo): should not be required once old system is kaput
transaction_system_t *get_active_transaction_system();

void *store_data(u32 kind);

// TODO(undo): would be nice if the undo/redo implementation specifics could be hidden
//             from the global namespace
extern const event_contract_t event_undo__contract;
extern const event_contract_t event_redo__contract;

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
		.stores        = array_create_ex(alc),
		.event_queue   = array_create_ex(alc),
		.event_history = array_create_ex(alc),
	};
}

void transaction_system_destroy(transaction_system_t *sys)
{
	array_foreach(sys->stores, store_t *, store_ptr)
		store_destroy(*store_ptr, sys->alc);
	array_destroy(sys->stores);

	array_foreach(sys->event_queue, event_t *, event_ptr)
		event_destroy(*event_ptr, sys->alc);
	array_destroy(sys->event_queue);

	array_foreach(sys->event_history, event_bundle_t, bundle_ptr)
		event_bundle_destroy(bundle_ptr, sys->alc);
	array_destroy(sys->event_history);
}

void transaction_system_set_active(transaction_system_t *sys)
{
	g_active_transaction_system = sys;
}

static
const event_bundle_t *transaction__last_valid_event_bundle(transaction_system_t *sys, b32 undoing)
{
	u32 undo_count = 0, redo_count = 0, real_event_count = 0;

	for (s32 i = array_sz(sys->event_history)-1; i >= 0; --i) {
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

typedef struct event_undo
{
	b8 _;    // coerce into a struct with nonzero size
} event_undo_t;

typedef struct event_redo
{
	b8 _;    // coerce into a struct with nonzero size
} event_redo_t;

static
b32 event_undo__execute(const event_undo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	const event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, true);

	if (!bundle)
		return false;

	if (bundle != &array_last(sys->event_history)) {
		/* there are secondary events to undo, in reverse insertion order */
		const event_bundle_t *bundle_after = bundle+1;
		if (bundle_after->secondary)
			for (s32 i = array_sz(bundle_after->d)-1; i >= 0; --i)
				event_undo(bundle_after->d[i]);
	}

	event_undo(bundle->d[0]);
	return true;
}

static
b32 event_redo__execute(const event_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	const event_bundle_t *bundle = transaction__last_valid_event_bundle(sys, false);

	if (!bundle)
		return false;

	if (bundle != &array_first(sys->event_history)) {
		const event_bundle_t *bundle_before = bundle-1;
		if (bundle_before->secondary)
			for (u32 i = 0; i < array_sz(bundle_before->d); ++i)
				event_execute(bundle_before->d[i]);
	}

	event_execute(bundle->d[0]);
	return true;
}

const event_contract_t event_undo__contract = {
	.destroy = event__destroy_noop,
	.execute = (b32 (*)(const void *))event_undo__execute,
};

const event_contract_t event_redo__contract = {
	.destroy = event__destroy_noop,
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

void *transaction_spawn_event(const event_metadata_t *meta, u32 kind)
{
	transaction_system_t *sys = g_active_transaction_system;

	void *instance = NULL;

	switch (kind) {
	case EVENT_KIND_NOOP:
		assert(false);
	break;
	case EVENT_KIND_UNDO:
	{
		event_alloc(undo, undo_instance, sys->alc);
		instance = undo_instance;
	}
	break;
	case EVENT_KIND_REDO:
	{
		event_alloc(redo, redo_instance, sys->alc);
		instance = redo_instance;
	}
	break;
	default:
		instance = meta->spawner(sys->alc);
	}

	event_t *event = event_create(kind, instance, meta, sys->alc);
	array_append(sys->event_queue, event);
	return event->instance;
}

static
u32 transaction__handle_priority_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	if (event_execute(event)) {
		// NOTE(luke): priority events are never secondary
		array_append(sys->event_history, event_bundle_create(event, sys->alc));
		result = event->kind;
		// CLEANUP(undo): remove
		log_debug("        %s", event->meta->description);
	}
	else {
		event_destroy(event, sys->alc);
	}

	/* need to destroy the others, since they will never be added to history */
	array_foreach(sys->event_queue, event_t *, event_ptr)
		if (*event_ptr != event)
			event_destroy(*event_ptr, sys->alc);

	return result;
}

static
u32 transaction__handle_ordinary_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	/* ensure that only one event fires at a time */
	if (array_sz(sys->event_queue) == 1) {
		if (event_execute(event)) {
			event_bundle_t *last_bundle = array_empty(sys->event_history)
			                            ? NULL
			                            : &array_last(sys->event_history);

			event_t *last_event = last_bundle == NULL || array_empty(last_bundle->d)
			                    ? NULL
			                    : array_last(last_bundle->d);

			if (   event->meta->multi_frame
			    && last_event
			    && last_event->kind == event->kind) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh event */;
				if (   last_bundle
				    && last_bundle->secondary
				    && event->meta->secondary)
						array_append(last_bundle->d, event);
				else
					array_append(sys->event_history, event_bundle_create(event, sys->alc));

				result = event->kind;
			}
			// CLEANUP(undo): remove
			log_debug("        %s", event->meta->description);
		} else {
			event_destroy(event, sys->alc);
		}
	} else {
		assert(false);
		array_foreach(sys->event_queue, event_t *, event_ptr)
			event_destroy(*event_ptr, sys->alc);
	}

	return result;
}

u32 transaction__flush()
{
	u32 result = EVENT_KIND_NOOP;
	transaction_system_t *sys = g_active_transaction_system;

	if (array_empty(sys->event_queue))
		return result;

	event_t *priority_event = NULL;

	/* check that an undo or redo event did not get queued */
	array_foreach(sys->event_queue, event_t *, event_ptr)
		if ((*event_ptr)->kind < 3) {
			priority_event = *event_ptr;
			break;
		}

	if (priority_event) {
		result = transaction__handle_priority_event(sys, priority_event);
	} else {
		event_t *ordinary_event = array_first(sys->event_queue);
		const u32 tmp = transaction__handle_ordinary_event(sys, ordinary_event);
		if (!ordinary_event->meta->secondary)
			result = tmp;
	}

	array_clear(sys->event_queue);
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
