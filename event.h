#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

#define EVENT_KIND_NOOP 0
#define EVENT_KIND_UNDO 1
#define EVENT_KIND_REDO 2

#define EVENT_DESCRIPTION_SIZE 64
#define NAV_DESCRIPTION_SIZE   16

#define EVENT_NEST_LEVEL_INVALID UINT8_MAX
#define EVENT_NEST_LEVEL_NONE    0

typedef struct event_contract {
	void (*destroy)(void *instance, allocator_t *alc);
	b32  (*execute)(const void *instance);
	void (*undo   )(const void *instance);
	void (*update )(void *dst, const void* src);    // both dst and src are instances
	// TODO(undo): if writing event stream to file, each event needs to be able
	//             to serialize & deserialize itself
} event_contract_t;

typedef struct event_metadata {
	void (*spawner)(void *instance, allocator_t *alc);
	const event_contract_t *contract;
	/* Addresses the use case when some data mutation might be repeated with a
	   different payload. If the desired undo behavior is such that it reverts
	   to the state before any of the repeated actions happened, then instead
	   of creating a new undo point with every transaction, the previous
	   transaction's undo point is modified. */
	const b32 multi_frame;
	const u32 size;
	const char description[EVENT_DESCRIPTION_SIZE];
	/* a secondary event is one that will not be directly associated with an undo point */
	const b32 secondary;
} event_metadata_t;

typedef struct event {
	const event_metadata_t *meta;
	char nav_description[NAV_DESCRIPTION_SIZE];
	s64 time_since_epoch_ms;
    /* expect event_kind_e */
	u32 kind;
	u8 nest_level;
	char _padding[3];
	char instance[];
} event_t;

typedef struct event_bundle {
	array(event_t *) d;
	/* all events in a bundle are either primary or secondary */
	b32 secondary;
} event_bundle_t;

event_t *event_create(u32 kind, u32 instance_size, const event_metadata_t *meta,
                      const char *nav_description, allocator_t *alc);
void event_destroy(event_t *event, allocator_t *alc);
b32  event_execute(const event_t *event);
void event_undo(const event_t *event);
/* fast forward an event to another event - only used in multi-frame interactions */
void event_update(event_t *dst, const event_t *src);

event_bundle_t event_bundle_create(event_t *event, allocator_t *alc);
void event_bundle_destroy(event_bundle_t *bundle, allocator_t *alc);
void event_bundle_clear(event_bundle_t *bundle, allocator_t *alc);
void event_bundle_copy(event_bundle_t *dst, const event_bundle_t *src);
void event_bundle_unwind(event_bundle_t *bundle, b32 undo, allocator_t *alc);
void event_bundle_unwind_to(event_bundle_t *bundle, event_t *event, b32 undo, allocator_t *alc);
event_t *event_bundle_last_non_nested_event(const event_bundle_t *bundle);

#define event_factory(type) \
	.contract = &(event_contract_t) { \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
	}, \
	.size = sizeof(event_##type##_t)

#define event_factory_multi_frame(type) \
	.contract = &(event_contract_t) { \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
		.update  = (void (*)(void *, const void *))event_##type##__update, \
	}, \
	.multi_frame = true, \
	.size = sizeof(event_##type##_t)

#define event_factory_dynamic(type) \
	.spawner = (void (*)(void *, allocator_t *))event_##type##__create, \
	.contract = &(event_contract_t) { \
		.destroy = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
	}, \
	.size = sizeof(event_##type##_t)

#define event_factory_multi_frame_dynamic(type) \
	.spawner = (void (*)(void *, allocator_t *))event_##type##__create, \
	.contract = &(event_contract_t) { \
		.destroy = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.execute = (b32  (*)(const void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
		.update  = (void (*)(void *, const void *))event_##type##__update, \
	}, \
	.multi_frame = true, \
	.size = sizeof(event_##type##_t)

#endif // VIOLET_EVENT_H

/* Implementation */

#ifdef EVENT_IMPLEMENTATION

event_t *event_create(u32 kind, u32 instance_size, const event_metadata_t *meta,
                      const char *nav_description, allocator_t *alc)
{
	event_t *event = acalloc(1, sizeof(event_t) + instance_size, alc);
	event->kind = kind;
	event->meta = meta;
	if (nav_description)
		strbcpy(event->nav_description, nav_description);
	else
		event->nav_description[0] = 0;
	event->time_since_epoch_ms = time_milliseconds_since_epoch();
	if (meta->spawner)
		meta->spawner(event->instance, alc);
	return event;
}

void event_destroy(event_t *event, allocator_t *alc)
{
	if (event->meta->contract->destroy)
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
	dst->time_since_epoch_ms = src->time_since_epoch_ms;
	(dst->meta->contract->update)(dst->instance, src->instance);
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

void event_bundle_clear(event_bundle_t *bundle, allocator_t *alc)
{
	array_foreach(bundle->d, event_t *, event_ptr)
		event_destroy(*event_ptr, alc);
	array_clear(bundle->d);
}

void event_bundle_copy(event_bundle_t *dst, const event_bundle_t *src)
{
	dst->secondary = src->secondary;
	dst->d = array_create_ex(array__allocator(src->d));
	array_copy(dst->d, src->d);
}

void event_bundle_unwind(event_bundle_t *bundle, b32 undo, allocator_t *alc)
{
	for (u32 i = array_sz(bundle->d); i-- > 0; ) {
		event_undo(bundle->d[i]);
		event_destroy(bundle->d[i], alc);
	}
	array_clear(bundle->d);
}

void event_bundle_unwind_to(event_bundle_t *bundle, event_t *event, b32 undo, allocator_t *alc)
{
	for (u32 i = array_sz(bundle->d); i-- > 0; ) {
		const event_t *event_i = bundle->d[i];
		if (event_i == event) {
			array_pop(bundle->d);
			event_destroy(event, alc);
			return;
		}
		if (undo)
			event_undo(bundle->d[i]);
		event_destroy(bundle->d[i], alc);
	}
	assert(false);
}

event_t *event_bundle_last_non_nested_event(const event_bundle_t *bundle)
{
	for (u32 i = array_sz(bundle->d); i-- > 0; ) {
		event_t *event = bundle->d[i];
		if (event->nest_level == EVENT_NEST_LEVEL_NONE)
			return event;
	}
	return NULL;
}

#undef EVENT_IMPLEMENTATION
#endif // EVENT_IMPLEMENTATION
