#ifndef VIOLET_STORE_H
#define VIOLET_STORE_H

typedef struct store_contract {
	void  (*destroy )(void *instance, allocator_t *alc);
} store_contract_t;

typedef struct store_metadata {
	void *(*spawner)(allocator_t *alc);
	const store_contract_t *contract;
} store_metadata_t;

typedef struct store {
	/* expect store_kind_e */
	u32 kind;
	void *instance;
	const store_metadata_t *meta;
} store_t;

store_t *store_create(u32 kind, void *instance, const store_metadata_t *meta, allocator_t *alc);
void store_destroy(store_t *store, allocator_t *alc);

#define store_factory(type) \
	(void *(*)(allocator_t *))store_##type##__create, \
	&(store_contract_t) { \
		.destroy  = (void  (*)(void *, allocator_t *))store_##type##__destroy, \
	}

#define store_alloc(type, store, alc) \
	struct store_##type *store = acalloc(1, sizeof(struct store_##type), alc);

#endif // VIOLET_STORE_H

/* Implementation */

#ifdef STORE_IMPLEMENTATION

store_t *store_create(u32 kind, void *instance, const store_metadata_t *meta, allocator_t *alc)
{
	store_t *store  = amalloc(sizeof(store_t), alc);
	store->kind = kind;
	store->instance = instance;
	store->meta = meta;
	return store;
}

void store_destroy(store_t *store, allocator_t *alc)
{
	(store->meta->contract->destroy)(store->instance, alc);
	afree(store, alc);
}

#undef STORE_IMPLEMENTATION
#endif // STORE_IMPLEMENTATION
