#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

#define EVENT_KIND_NOOP 0
#define EVENT_KIND_UNDO 1
#define EVENT_KIND_REDO 2

typedef struct event_contract {
	void (*destroy)(void *instance, allocator_t *alc);
	b32  (*execute)(const void *instance);
	void (*undo   )(const void *instance);
	// TODO(undo): if writing event stream to file, each event needs to be able
	//             to serialize & deserialize itself
} event_contract_t;

typedef struct event_metadata {
	void *(*spawner)(allocator_t *alc);
	const event_contract_t *contract;
	const char *description;
} event_metadata_t;

typedef struct event {
    /* expect event_kind_e */
	u32 kind;
	void *instance;
	const event_metadata_t *meta;
} event_t;

event_t *event_create(u32 kind, void *instance, const event_metadata_t *meta,
                      allocator_t *alc);
void event_destroy(event_t *event, allocator_t *alc);
b32  event_execute(const event_t *event);
void event_undo(const event_t *event);

void event__destroy_noop(void *instance, allocator_t *alc);

// CLEANUP(undo): find a better spot for this type
/* for dynamic arrays */
typedef enum list_operation {
	LIST_OP_UPDATE,
	LIST_OP_APPEND,
	LIST_OP_POP,
	// TODO(undo): what about insert and remove at index?
} list_operation_e;

#define event_factory(type) \
	(void *(*)(allocator_t *))event_##type##__create, \
	&(event_contract_t) { \
		.destroy  = event__destroy_noop, \
		.execute  = (b32  (*)(const void *))event_##type##__execute, \
		.undo     = (void (*)(const void *))event_##type##__undo, \
	}

#define event_factory_explicit_destructor(type) \
	(void *(*)(allocator_t *))event_##type##__create, \
	&(event_contract_t) { \
		.destroy     = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.execute     = (b32  (*)(const void *))event_##type##__execute, \
		.undo        = (void (*)(const void *))event_##type##__undo, \
	}

#define event_alloc(type, event, alc) \
	struct event_##type *event = acalloc(1, sizeof(struct event_##type), alc);

#endif // VIOLET_EVENT_H

/* Implementation */

#ifdef EVENT_IMPLEMENTATION

event_t *event_create(u32 kind, void *instance, const event_metadata_t *meta,
                      allocator_t *alc)
{
	event_t *event = amalloc(sizeof(event_t), alc);
	event->kind = kind;
	event->instance = instance;
	event->meta = meta;
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
	return (event->meta->contract->undo)(event->instance);
}

void event__destroy_noop(void *instance, allocator_t *alc)
{
	afree(instance, alc);
}

#undef EVENT_IMPLEMENTATION
#endif // EVENT_IMPLEMENTATION