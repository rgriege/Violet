#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

#define EVENT_KIND_NOOP 0
#define EVENT_KIND_UNDO 1
#define EVENT_KIND_REDO 2

#define EVENT_DESCRIPTION_SIZE 24
#define NAV_DESCRIPTION_SIZE   16

typedef struct event_contract {
	void (*destroy)(void *instance, allocator_t *alc);
	b32  (*execute)(const void *instance);
	void (*undo   )(const void *instance);
	void (*update )(void *dst, const void* src);    // both dst and src are instances
	// TODO(undo): if writing event stream to file, each event needs to be able
	//             to serialize & deserialize itself
} event_contract_t;

typedef struct event_metadata {
	void *(*spawner)(allocator_t *alc);
	const event_contract_t *contract;
	/* Addresses the use case when some data mutation might be repeated with a
	   different payload. If the desired undo behavior is such that it reverts
	   to the state before any of the repeated actions happened, then instead
	   of creating a new undo point with every transaction, the previous
	   transaction's undo point is modified. */
	const b32 multi_frame;
	const char description[EVENT_DESCRIPTION_SIZE];
	/* a secondary event is one that will not be directly associated with an undo point */
	const b32 secondary;
} event_metadata_t;

typedef struct event {
    /* expect event_kind_e */
	u32 kind;
	void *instance;
	const event_metadata_t *meta;
	char nav_description[NAV_DESCRIPTION_SIZE];
	s64 time_since_epoch_ms;
} event_t;

typedef struct event_bundle {
	array(event_t *) d;
	/* all events in a bundle are either primary or secondary */
	b32 secondary;
} event_bundle_t;

event_t *event_create(u32 kind, void *instance, const event_metadata_t *meta,
                      char *nav_description, allocator_t *alc);
void event_destroy(event_t *event, allocator_t *alc);
b32  event_execute(const event_t *event);
void event_undo(const event_t *event);
	/* fast forward an event to another event - only used in multi-frame interactions */
void event_update(event_t *dst, const event_t *src);

void event__destroy_noop(void *instance, allocator_t *alc);

event_bundle_t event_bundle_create(event_t *event, allocator_t *alc);
void event_bundle_destroy(event_bundle_t *bundle, allocator_t *alc);

#define event_factory(type) \
	.spawner = (void *(*)(allocator_t *))event_##type##__create, \
	.contract = &(event_contract_t) { \
		.destroy = event__destroy_noop, \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
	}, \
	.multi_frame = false

#define event_factory_multi_frame(type) \
	.spawner = (void *(*)(allocator_t *))event_##type##__create, \
	.contract = &(event_contract_t) { \
		.destroy = event__destroy_noop, \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
		.update  = (void (*)(void *, const void *))event_##type##__update, \
	}, \
	.multi_frame = true

#define event_factory_explicit_destructor(type) \
	.spawner = (void *(*)(allocator_t *))event_##type##__create, \
	.contract = &(event_contract_t) { \
		.destroy = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
	}, \
	.multi_frame = false

#define event_factory_multi_frame_explicit_destructor(type) \
	.spawner = (void *(*)(allocator_t *))event_##type##__create, \
	.contract = &(event_contract_t) { \
		.destroy = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
		.update  = (void (*)(void *, const void *))event_##type##__update, \
	}, \
	.multi_frame = true

#define event_alloc(type, event, alc) \
	CONCAT(event_, CONCAT(type, _t)) *event = acalloc(1, sizeof(CONCAT(event_, \
	CONCAT(type, _t))), alc)

#endif // VIOLET_EVENT_H

/* Implementation */

#ifdef EVENT_IMPLEMENTATION

event_t *event_create(u32 kind, void *instance, const event_metadata_t *meta,
                      char *nav_description, allocator_t *alc)
{
	event_t *event = amalloc(sizeof(event_t), alc);
	event->kind = kind;
	event->instance = instance;
	event->meta = meta;
	if (nav_description)
		strbcpy(event->nav_description, nav_description);
	else
		event->nav_description[0] = 0;
	event->time_since_epoch_ms = time_milliseconds_since_epoch();
	return event;
}

void event_destroy(event_t *event, allocator_t *alc)
{
	(event->meta->contract->destroy)(event->instance, alc);
	afree(event, alc);
}

b32 event_execute(const event_t *event)
{
	return (event->meta->contract->execute)(event->instance);
}

void event_undo(const event_t *event)
{
	(event->meta->contract->undo)(event->instance);
}

void event_update(event_t *dst, const event_t *src)
{
	(dst->meta->contract->update)(dst->instance, src->instance);
}

void event__destroy_noop(void *instance, allocator_t *alc)
{
	afree(instance, alc);
}

event_bundle_t event_bundle_create(event_t *event, allocator_t *alc)
{
	event_bundle_t events = (event_bundle_t) {
		.secondary = event->meta->secondary,
		.d = array_create_ex(alc),
	};
	array_append(events.d, event);
	return events;
}

void event_bundle_destroy(event_bundle_t *bundle, allocator_t *alc)
{
	array_foreach(bundle->d, event_t *, event_ptr)
		event_destroy(*event_ptr, alc);
	array_destroy(bundle->d);
}

#undef EVENT_IMPLEMENTATION
#endif // EVENT_IMPLEMENTATION
