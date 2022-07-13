#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

#define EVENT_KIND_NOOP 0
#define EVENT_KIND_UNDO 1
#define EVENT_KIND_REDO 2

#define EVENT_DESCRIPTION_SIZE 64
#define NAV_DESCRIPTION_SIZE   16

typedef struct event_contract {
	void (*create    )(void *instance, allocator_t *alc);
	void (*destroy   )(void *instance, allocator_t *alc);
	b32  (*load      )(void *instance, void *userp);
	void (*save      )(const void *instance, void *userp);
	b32  (*execute   )(void *instance);
	void (*undo      )(const void *instance);
	void (*update    )(void *dst, const void *src); // both dst and src are instances
	b32  (*update_pre)(void *new, const optional(void) old); // both new and old are instances
} event_contract_t;

typedef struct event_metadata {
	const event_contract_t *contract;
	/* Addresses the use case when some data mutation might be repeated with a
	   different payload. If the desired undo behavior is such that it reverts
	   to the state before any of the repeated actions happened, then instead
	   of creating a new undo point with every transaction, the previous
	   transaction's undo point is modified. */
	const b32 multi_frame;
	const u32 size;
	const char description[EVENT_DESCRIPTION_SIZE];
	const char *label;
	/* a secondary event is one that will not be directly associated with an undo point */
	const b32 secondary;
	/* use VERY sparingly, i.e. when all side effects from nested events will be manually reverted
	 * in the root event __undo handler */
	const b32 soft_merge;
	const u32 version;
} event_metadata_t;

typedef struct event {
	const event_metadata_t *meta;
	array(struct event *) children;
	char nav_description[NAV_DESCRIPTION_SIZE];
	s64 time_since_epoch_ms;
    /* expect event_kind_e */
	u32 kind;
	s32 status;
	char instance[];
} event_t;

typedef enum event_status {
	EVENT_STATUS_DONE,
	EVENT_STATUS_UNDONE,
	EVENT_STATUS_UNREACHABLE,
} event_status_e;

event_t *event_create(u32 kind, const event_metadata_t *meta,
                      const char *nav_description, allocator_t *alc);
event_t *event_create_empty(u32 kind, const event_metadata_t *meta,
                            const char *nav_description, allocator_t *alc);
void event_destroy(event_t *event, allocator_t *alc);
b32  event_execute(event_t *event);
void event_undo(event_t *event, allocator_t *alc);
void event_unwind_children(event_t *event, allocator_t *alc);
/* fast forward an event to another event - only used in multi-frame interactions */
void event_update(event_t *dst, const event_t *src);
/* returns true if the events are mergeable, based on the event-specific implementation */
b32 event_update_pre(event_t *dst, const event_t *src);

#define event_factory(type) \
	.contract = &(event_contract_t) { \
		.execute = (b32  (*)(void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
	}, \
	.size = sizeof(event_##type##_t)

#define event_factory_multi_frame(type) \
	.contract = &(event_contract_t) { \
		.execute = (b32  (*)(void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
		.update  = (void (*)(void *, const void *))event_##type##__update, \
	}, \
	.multi_frame = true, \
	.size = sizeof(event_##type##_t)

#define event_factory_dynamic(type) \
	.contract = &(event_contract_t) { \
		.create  = (void (*)(void *, allocator_t *))event_##type##__create, \
		.destroy = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.load    = (b32  (*)(void *, void *))event_##type##__load, \
		.save    = (void (*)(const void *, void *))event_##type##__save, \
		.execute = (b32  (*)(void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
	}, \
	.size = sizeof(event_##type##_t)

#define event_factory_multi_frame_dynamic(type) \
	.contract = &(event_contract_t) { \
		.create  = (void (*)(void *, allocator_t *))event_##type##__create, \
		.destroy = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.load    = (b32  (*)(void *, void *))event_##type##__load, \
		.save    = (void (*)(const void *, void *))event_##type##__save, \
		.execute = (b32  (*)(void *))event_##type##__execute, \
		.undo    = (void (*)(const void *))event_##type##__undo, \
		.update  = (void (*)(void *, const void *))event_##type##__update, \
	}, \
	.multi_frame = true, \
	.size = sizeof(event_##type##_t)

#define event_factory_multi_frame_pre(type) \
	.contract = &(event_contract_t) { \
		.execute    = (b32  (*)(void *))event_##type##__execute, \
		.undo       = (void (*)(const void *))event_##type##__undo, \
		.update     = (void (*)(void *, const void *))event_##type##__update, \
		.update_pre = (b32  (*)(void *, const void *))event_##type##__update_pre, \
	}, \
	.multi_frame = true, \
	.size = sizeof(event_##type##_t)

#define event_factory_multi_frame_pre_dynamic(type) \
	.contract = &(event_contract_t) { \
		.create     = (void (*)(void *, allocator_t *))event_##type##__create, \
		.destroy    = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.load       = (b32  (*)(void *, void *))event_##type##__load, \
		.save       = (void (*)(const void *, void *))event_##type##__save, \
		.execute    = (b32  (*)(void *))event_##type##__execute, \
		.undo       = (void (*)(const void *))event_##type##__undo, \
		.update     = (void (*)(void *, const void *))event_##type##__update, \
		.update_pre = (b32  (*)(void *, const void *))event_##type##__update_pre, \
	}, \
	.multi_frame = true, \
	.size = sizeof(event_##type##_t)

#endif // VIOLET_EVENT_H

/* Implementation */

#ifdef EVENT_IMPLEMENTATION

event_t *event_create(u32 kind, const event_metadata_t *meta,
                      const char *nav_description, allocator_t *alc)
{
	event_t *event = event_create_empty(kind, meta, nav_description, alc);
	if (meta->contract->create)
		meta->contract->create(event->instance, alc);
	return event;
}

event_t *event_create_empty(u32 kind, const event_metadata_t *meta,
                            const char *nav_description, allocator_t *alc)
{
	event_t *event = acalloc(1, sizeof(event_t) + meta->size, alc);
	event->kind = kind;
	event->meta = meta;
	if (nav_description)
		strbcpy(event->nav_description, nav_description);
	else
		event->nav_description[0] = 0;
	event->time_since_epoch_ms = time_milliseconds_since_epoch();
	event->children = array_create_ex(alc);
	return event;
}

void event_destroy(event_t *event, allocator_t *alc)
{
	/* recursively destroy children */
	array_foreach(event->children, event_t *, child_ptr)
		event_destroy(*child_ptr, alc);
	array_destroy(event->children);

	if (event->meta->contract->destroy)
		(event->meta->contract->destroy)(event->instance, alc);
	afree(event, alc);
}

b32 event_execute(event_t *event)
{
	/* expect children to be created as nested events from this entry point forward */
	assert(array_empty(event->children));

	return (event->meta->contract->execute)(event->instance);
}

void event__unwind_children(event_t *event, allocator_t *alc)
{
	/* since the side-effects from child events proceed after execution of their parent,
	 * reverting the side-effects requires that the children be undone first, in reverse
	 * insertion order */
	while (!array_empty(event->children)) {
		event_t *child = array_last(event->children);
		event_undo(child, alc);
		event_destroy(child, alc);
		array_pop(event->children);
	}
}

void event_undo(event_t *event, allocator_t *alc)
{
	event__unwind_children(event, alc);
	(event->meta->contract->undo)(event->instance);
}


void event_unwind_children(event_t *event, allocator_t *alc)
{
	event__unwind_children(event, alc);
}


void event_update(event_t *dst, const event_t *src)
{
	dst->time_since_epoch_ms = src->time_since_epoch_ms;
	(dst->meta->contract->update)(dst->instance, src->instance);
	/* NOTE(luke): no need to update children, since they will always be pruned
	 * upon undoing and re-created upon redoing */
}

b32 event_update_pre(event_t *dst, const event_t *src)
{
	b32 result = src != NULL;
	result &= dst->meta->contract->update_pre(dst->instance, src ? src->instance : NULL);
	return result;
}

#undef EVENT_IMPLEMENTATION
#endif // EVENT_IMPLEMENTATION
