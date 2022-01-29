#ifndef VIOLET_STORE_H
#define VIOLET_STORE_H

typedef enum store_kind {
	STORE_KIND_GUI,
	STORE_KIND_A,
	STORE_KIND_B,
} store_kind_e;

/* achieve polymorphism via C++ style inheritance */
typedef struct store_contract {
	store_kind_e (*get_kind)(void *instance);
	void *       (*get_data)(void *instance);
	void         (*destroy )(void *instance, allocator_t *alc);
} store_contract_t;

typedef struct store {
	void *instance;
	const store_contract_t *contract;
} store_t;

store_t *    store_create(void *instance, store_contract_t *contract, allocator_t *alc);
void         store_destroy(store_t *store, allocator_t *alc);
store_kind_e store_get_kind(const store_t *store);
void *       store_get_data(const store_t *store);

#define store_offset_bytes(store, cmd, dst) \
	unsigned char *(dst) = (unsigned char *)store_get_data(store) + (cmd)->offset;

// TODO(luke): should we replace this with something that calls into
//             store_instance_from_kind and/or store_get_data()?
#define store_offsetof(store_type, member) \
	offsetof(store_type, data.member) - offsetof(store_type, data)

#endif // VIOLET_STORE_H

/* Implementation */

#ifdef STORE_IMPLEMENTATION

store_t *store_create(void *instance, store_contract_t *contract, allocator_t *alc)
{
	store_t *store   = amalloc(sizeof(store_t), alc);
	store->instance  = instance;
	store->contract = contract;
	return store;
}

void store_destroy(store_t *store, allocator_t *alc)
{
	(store->contract->destroy)(store->instance, alc);
	afree(store, alc);
}

store_kind_e store_get_kind(const store_t *store)
{
	return (store->contract->get_kind)(store->instance);
}

void *store_get_data(const store_t *store)
{
	return (store->contract->get_data)(store->instance);
}

#undef STORE_IMPLEMENTATION
#endif // STORE_IMPLEMENTATION
