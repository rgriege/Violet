#ifndef VIOLET_TRANSACTION_H
#define VIOLET_TRANSACTION_H

#define TRANSACTION_MULTIFRAME_TIMEOUT 500

typedef struct transaction_logger {
	void (*log)(void *userp, u32 event_index);
	void *userp;
} transaction_logger_t;

typedef struct transaction_system {
	allocator_t *alc;
	array(store_t *) stores;
	/* append-only stack of event history */
	array(event_t *) event_history;
	/* secondary events awaiting confirmation by a primary event */
	array(event_t *) temp_secondary_events;
	str_t last_event_desc;
	b32 undoing;
	event_t *active_parent;
	transaction_logger_t *logger; /* NULLable, unowned */
} transaction_system_t;

transaction_system_t transaction_system_create(transaction_logger_t *logger, allocator_t *alc);
void transaction_system_reset(transaction_system_t *sys);
void transaction_system_destroy(transaction_system_t *sys);
void transaction_system_set_active(transaction_system_t *sys);
void transaction_spawn_store(const store_metadata_t *meta, u32 kind);
void *transaction_spawn_event(const event_metadata_t *meta, const char *nav_description, u32 kind);
void *transaction_spawn_empty_event(const event_metadata_t *meta, const char *nav_description, u32 kind);
void transaction_get_undoables(array(event_t *) *undoables);
void transaction_get_redoables(array(event_t *) *redoables);
b32  transaction_system_can_undo(void);
b32  transaction_system_can_redo(void);
b32  transaction_system_restore(array(event_t *) history);
/* Returns a nonzero event kind for executed, non-secondary events */
u32 transaction__flush(event_t *event);
transaction_system_t *get_active_transaction_system(void);

void *store_data(u32 kind);

typedef struct event_noop
{
	u8 dummy;
} event_noop_t;

typedef struct event_undo_redo
{
	str_t label;
} event_undo_redo_t;

extern const event_contract_t event_noop__contract;
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

transaction_system_t transaction_system_create(transaction_logger_t *logger, allocator_t *alc)
{
	return (transaction_system_t) {
		.alc = alc,
		.stores          = array_create_ex(alc),
		.event_history   = array_create_ex(alc),
		.last_event_desc = str_create(alc),
		.temp_secondary_events = array_create_ex(alc),
		.undoing = false,
		.active_parent = NULL,
		.logger = logger,
	};
}

/* store data is NOT reset */
void transaction_system_reset(transaction_system_t *sys)
{
	array_foreach(sys->event_history, event_t *, event_recorded)
		event_destroy(*event_recorded, sys->alc);
	array_clear(sys->event_history);

	array_foreach(sys->temp_secondary_events, event_t *, event_temp)
		event_destroy(*event_temp, sys->alc);
	array_clear(sys->temp_secondary_events);

	str_clear(&sys->last_event_desc);
}

void transaction_system_destroy(transaction_system_t *sys)
{
	array_foreach(sys->stores, store_t *, store_ptr)
		store_destroy(*store_ptr, sys->alc);
	array_destroy(sys->stores);

	array_foreach(sys->event_history, event_t *, event_recorded)
		event_destroy(*event_recorded, sys->alc);
	array_destroy(sys->event_history);

	array_foreach(sys->temp_secondary_events, event_t *, event_temp)
		event_destroy(*event_temp, sys->alc);
	array_destroy(sys->temp_secondary_events);

	str_destroy(&sys->last_event_desc);
}

void transaction_system_set_active(transaction_system_t *sys)
{
	g_active_transaction_system = sys;
}

static
event_t *transaction__last_doable_event(transaction_system_t *sys, b32 undoing)
{
	event_t *result = NULL;
	array(event_t *) doables;
	array_init_ex(doables, 4, g_temp_allocator);

	if (undoing)
		transaction_get_undoables(&doables);
	else
		transaction_get_redoables(&doables);

	if (!array_empty(doables))
		result = doables[0];

	array_destroy(doables);
	return result;
}

/* Returns undoable root events, whose head corresponds to the most recently `done` event. */
void transaction_get_undoables(array(event_t *) *undoables)
{
	transaction_system_t *sys = g_active_transaction_system;

	for (u32 i = 0, n = array_sz(sys->event_history); i < n; ++i) {
		event_t *event = sys->event_history[i];

		if (   event->meta->secondary
		    || event->status == EVENT_STATUS_UNREACHABLE
		    || event->kind <= EVENT_KIND_REDO)
			continue;

		if (event->status == EVENT_STATUS_UNDONE)
			/* nothing else to undo */
			break;

		array_append(*undoables, event);
	}
	array_reverse(*undoables);
}

/* Returns redoable root events, whose head corresponds to the most recently `undone` event. */
void transaction_get_redoables(array(event_t *) *redoables)
{
	transaction_system_t *sys = g_active_transaction_system;

	for (u32 i = array_sz(sys->event_history); i-- > 0; ) {
		event_t *event = sys->event_history[i];

		if (   event->meta->secondary
		    || event->status == EVENT_STATUS_UNREACHABLE
		    || event->kind <= EVENT_KIND_REDO)
			continue;

		if (event->status == EVENT_STATUS_DONE)
			/* nothing else to redo */
			break;

		array_append(*redoables, event);
	}
	array_reverse(*redoables);
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

/* If you actually want to serialize events, then the application must provide
 * real definitions of these functions.  The stubs are provided to avoid
 * the application needing to provide them if serialization isn't used. */
#ifdef VIOLET_EVENT_SERIALIZATION
b32 event_undo_redo__load(event_undo_redo_t *event, void *userp);
void event_undo_redo__save(const event_undo_redo_t *event, void *userp);
#else
b32 event_undo_redo__load(event_undo_redo_t *event, void *userp)
{
	assert(false);
	return false;
}

void event_undo_redo__save(const event_undo_redo_t *event, void *userp)
{
	assert(false);
}
#endif // VIOLET_EVENT_SERIALIZATION

static
void transaction__undo_event(transaction_system_t *sys, event_t *event)
{
	assert(event->status != EVENT_STATUS_UNREACHABLE);
	sys->undoing = true;
	event_undo(event, sys->alc);
	sys->undoing = false;
	event->status = EVENT_STATUS_UNDONE;
}

static
b32 transaction__redo_event(transaction_system_t *sys, event_t *event)
{
	assert(event->status != EVENT_STATUS_UNREACHABLE);
	sys->active_parent = event;
	if (event->meta->contract->update_pre)
		event->meta->contract->update_pre(event->instance, NULL);
	if (!event_execute(event))
		return false;
	sys->active_parent = NULL;
	event->status = EVENT_STATUS_DONE;
	return true;
}

b32 transaction_system_restore(array(event_t *) history)
{
	transaction_system_t *sys = g_active_transaction_system;
	assert(array_empty(sys->event_history));
	array_foreach(history, event_t *, event_ptr) {
		if (!transaction__redo_event(sys, *event_ptr)) {
			array_clear(sys->event_history);
			return false;
		}
		array_append(sys->event_history, *event_ptr);
	}
	array_clear(history);
	return true;
}

b32 transaction_system_can_undo(void)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_t *event = transaction__last_doable_event(sys, true);
	return event != NULL;
}

b32 transaction_system_can_redo(void)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_t *event = transaction__last_doable_event(sys, false);
	return event != NULL;
}

static
b32 event_noop__execute(event_noop_t *event)
{
	return true;
}

static
void transaction__unwind_temp_events(transaction_system_t *sys)
{
	array(event_t *) events = sys->temp_secondary_events;
	for (u32 i = array_sz(events); i-- > 0; ) {
		event_undo(events[i], sys->alc);
		event_destroy(events[i], sys->alc);
	}
	array_clear(events);
}

static
u32 transaction__event_idx(transaction_system_t *sys, const event_t *event)
{
	for (u32 i = 0, n = array_sz(sys->event_history); i < n; ++i)
		if (event == sys->event_history[i])
			return i;
	return ~0;
}

static
b32 event_undo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_t *doable = transaction__last_doable_event(sys, true);

	if (!doable)
		return false;

	const u32 doable_idx = transaction__event_idx(sys, doable);
	if (doable_idx >= array_sz(sys->event_history)) {
		assert(false);
		return false;
	}

	transaction__unwind_temp_events(sys);

	event_t **last = &array_last(sys->event_history);
	/* There can potentially be several secondary events in sequence after the undo point. */
	event_t **tail = &sys->event_history[doable_idx];
	while (tail != last) {
		const event_t *next = *(tail+1);
		if (   (   next->meta->secondary
		        && next->status == EVENT_STATUS_DONE)
			|| next->status == EVENT_STATUS_UNREACHABLE)
			tail++;
		else
			break;
	}
	while (tail != &sys->event_history[doable_idx]) {
		if (    (*tail)->meta->secondary
		     && (*tail)->status == EVENT_STATUS_DONE)
			transaction__undo_event(sys, *tail);
		tail--;
	}

	/* Handle the undo point */
	transaction__undo_event(sys, doable);
	transaction__set_event_description(&event->label, doable);
	return true;
}

static
b32 event_redo__execute(event_undo_redo_t *event)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_t *doable = transaction__last_doable_event(sys, false);

	if (!doable)
		return false;

	const u32 doable_idx = transaction__event_idx(sys, doable);
	if (doable_idx >= array_sz(sys->event_history)) {
		assert(false);
		return false;
	}

	transaction__unwind_temp_events(sys);

	event_t **first = &array_first(sys->event_history);
	/* There can potentially be several secondary events in sequence before the undo point. */
	event_t **head = &sys->event_history[doable_idx];
	while (head != first) {
		const event_t *prev = *(head-1);
		if (   (   prev->meta->secondary
		        && prev->status == EVENT_STATUS_UNDONE)
		    || prev->status == EVENT_STATUS_UNREACHABLE)
			head--;
		else
			break;
	}
	while (head != &sys->event_history[doable_idx]) {
		if (    (*head)->meta->secondary
		     && (*head)->status == EVENT_STATUS_UNDONE)
			transaction__redo_event(sys, *head);
		head++;
	}

	/* Handle the undo point */
	transaction__redo_event(sys, doable);
	transaction__set_event_description(&event->label, doable);
	return true;
}

const event_contract_t event_noop__contract = {
	.execute = (b32 (*)(void *))event_noop__execute,
};

const event_contract_t event_undo__contract = {
	.create  = &event_undo_redo__create,
	.destroy = (void (*)(void *, allocator_t *))event_undo_redo__destroy,
	.execute = (b32 (*)(void *))event_undo__execute,
	.load    = (b32  (*)(void *, void *))event_undo_redo__load,
	.save    = (void (*)(const void *, void *))event_undo_redo__save,
};

const event_contract_t event_redo__contract = {
	.create  = &event_undo_redo__create,
	.destroy = (void (*)(void *, allocator_t *))event_undo_redo__destroy,
	.execute = (b32 (*)(void *))event_redo__execute,
	.load    = (b32  (*)(void *, void *))event_undo_redo__load,
	.save    = (void (*)(const void *, void *))event_undo_redo__save,
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
	event_t *event = event_create(kind, meta, nav_description, sys->alc);
	return event->instance;
}

void *transaction_spawn_empty_event(const event_metadata_t *meta, const char *nav_description, u32 kind)
{
	transaction_system_t *sys = g_active_transaction_system;
	event_t *event = event_create_empty(kind, meta, nav_description, sys->alc);
	return event->instance;
}

static
u32 transaction__handle_priority_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;

	assert(!event->meta->contract->update_pre);

	if (event_execute(event)) {
		/* Priority events should be never accessible to other undos/redos */
		event->status = EVENT_STATUS_UNREACHABLE;
		/* Priority events are never secondary, nor are they ever nested */
		array_append(sys->event_history, event);
		if (sys->logger)
			sys->logger->log(sys->logger->userp, array_sz(sys->event_history) - 1);
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
void transaction__push_temp_events(transaction_system_t *sys)
{
	array(event_t *) events = sys->temp_secondary_events;
	if (array_empty(events))
		return;

	array_appendn(sys->event_history, events, array_sz(events));
	array_clear(events);
}

static
event_t *transaction__last_event(transaction_system_t *sys)
{
	if (!array_empty(sys->temp_secondary_events))
		return array_last(sys->temp_secondary_events);
	else if (!array_empty(sys->event_history))
		return array_last(sys->event_history);
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
void transaction__mark_unreachables(transaction_system_t *sys)
{
	s32 deficit = 0;
	for (u32 i = array_sz(sys->event_history); i-- > 0; ) {
		event_t *event = sys->event_history[i];

		switch (event->kind) {
		case EVENT_KIND_UNDO:
			deficit++;
		break;
		case EVENT_KIND_REDO:
			deficit--;
		break;
		default:
			assert(deficit >= 0);
			if (deficit == 0)
				break;
			/* NOTE: this marks both `secondary` and `non-secondary` events,
			 *       and events already marked as EVENT_STATUS_UNREACHABLE may
			 *       be re-marked as such. */
			event->status = EVENT_STATUS_UNREACHABLE;
			if (!event->meta->secondary && --deficit == 0)
				break;
		}
	}
}

static
u32 transaction__handle_ordinary_event(transaction_system_t *sys, event_t *event)
{
	u32 result = EVENT_KIND_NOOP;
	sys->active_parent = event;

	event_t *last_event = transaction__last_event(sys);
	b32 mergeable = transaction__events_mergeable(event, last_event);

	if (event->meta->contract->update_pre)
		mergeable &= event_update_pre(event, mergeable ? last_event : NULL);

	if (event_execute(event)) {
		if (event->meta->secondary) {
			if (mergeable) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh secondary event */
				array_append(sys->temp_secondary_events, event);
				result = event->kind;
			}
		} else {
			if (mergeable) {
				/* Handle continued multi-frame event */
				event_update(last_event, event);
				if (sys->logger)
					sys->logger->log(sys->logger->userp, array_sz(sys->event_history) - 1);
				if (!event->meta->soft_merge)
					event_unwind_children(event, sys->alc);
				event_destroy(event, sys->alc);
			} else {
				/* Handle fresh primary event; corresponds to an undo point */
				const u32 start = array_sz(sys->event_history);
				transaction__mark_unreachables(sys);
				transaction__push_temp_events(sys);
				array_append(sys->event_history, event);
				if (sys->logger)
					for (u32 i = start; i < array_sz(sys->event_history); ++i)
						sys->logger->log(sys->logger->userp, i);
				result = event->kind;
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
