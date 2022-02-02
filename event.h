#ifndef VIOLET_EVENT_H
#define VIOLET_EVENT_H

typedef struct event_contract {
	void (*destroy)(void *instance, allocator_t *alc);
	void (*execute)(const void *instance);
	void (*undo   )(const void *instance);
	// TODO(undo): if writing event stream to file, each event needs to be able
	//             to serialize & deserialize itself
} event_contract_t;

typedef struct event {
    /* expect event_kind_e */
	u32 kind;
	void *instance;
	const event_contract_t *contract;
} event_t;

event_t *event_create(u32 kind, void *instance, const event_contract_t *contract,
                      allocator_t *alc);
void event_destroy(event_t *event, allocator_t *alc);
void event_execute(const event_t *event);
void event_undo(const event_t *event);

void event__destroy_noop(void *instance, allocator_t *alc);

typedef struct event_metadata {
	void *(*spawner)(allocator_t *alc);
	event_contract_t contract;
	const char *description;
	// b32 mark_undo_point_start;  // TODO(undo); evaluate if this is a good approach
} event_metadata_t;

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
	(event_contract_t) { \
		.destroy  = (void (*)(void *, allocator_t *))event__destroy_noop, \
		.execute  = (void (*)(const void *))event_##type##__execute, \
		.undo     = (void (*)(const void *))event_##type##__undo, \
	}

#define event_factory_explicit_destructor(type) \
	(void *(*)(allocator_t *))event_##type##__create, \
	(event_contract_t) { \
		.destroy     = (void (*)(void *, allocator_t *))event_##type##__destroy, \
		.execute     = (void (*)(const void *))event_##type##__execute, \
		.undo        = (void (*)(const void *))event_##type##__undo, \
	}

#endif // VIOLET_EVENT_H

/* Implementation */

#ifdef EVENT_IMPLEMENTATION

array(event_t *) g_events = NULL;

event_t *event_create(u32 kind, void *instance, const event_contract_t *contract,
                      allocator_t *alc)
{
	event_t *event = amalloc(sizeof(event_t), alc);
	event->kind = kind;
	event->instance = instance;
	event->contract = contract;
	return event;
}

void event_destroy(event_t *event, allocator_t *alc)
{
	(event->contract->destroy)(event->instance, alc);
	afree(event, alc);
}

void event_execute(const event_t *event)
{
	return (event->contract->execute)(event->instance);
}

void event_undo(const event_t *event)
{
	return (event->contract->undo)(event->instance);
}

void event__destroy_noop(void *instance, allocator_t *alc)
{
	afree(instance, alc);
}

#undef EVENT_IMPLEMENTATION
#endif // EVENT_IMPLEMENTATION