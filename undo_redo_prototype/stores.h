typedef enum store_kind {
	STORE_KIND_GUI,
	STORE_KIND_A,
	STORE_KIND_B,
	STORE_KIND__COUNT,
} store_kind_e;

static const store_metadata_t g_store_registry[STORE_KIND__COUNT];

static
const store_metadata_t *store_metadata__from_kind(store_kind_e kind)
{
	return &g_store_registry[kind];
}

static
void store_spawn_from_kind(store_kind_e kind)
{
	const store_metadata_t *meta = store_metadata__from_kind(kind);
	transaction_spawn_store(meta, kind);
}

/* Concrete Event Implementations */

typedef struct store_a {
	char msg[64];
} store_a_t;

static
store_a_t *store_a__create(allocator_t *alc)
{
	store_alloc(a, store, alc);
	memcpy(store->msg, B2PS("you triggered store_a"));
	return store;
}

static
void store_a__destroy(store_a_t *store)
{
	NOOP;
}

typedef struct store_b {
	char msg[64];
} store_b_t;

static
store_b_t *store_b__create(allocator_t *alc)
{
	store_alloc(b, store, alc);
	memcpy(store->msg, B2PS("you triggered store_b"));
	return store;
}

static
void store_b__destroy(store_b_t *store)
{
	NOOP;
}

typedef struct store_gui {
	b32 chk;
	b32 chk2;
	s32 increment;
	r32 slider;
	r32 slider2;
	char npt[64];
	array(u32) arr;
} store_gui_t;

static
store_gui_t *store_gui__create(allocator_t *alc)
{
	store_alloc(gui, store, alc);
	memcpy(store->npt, B2PS("Do you even tater, bro?"));
	store->arr = array_create_ex(alc);
	return store;
}

static
void store_gui__destroy(store_gui_t *store, allocator_t *alc)
{
	array_destroy(store->arr);
	afree(store, alc);
}

/* Store Registry */
static const store_metadata_t g_store_registry[STORE_KIND__COUNT] = {
	[STORE_KIND_GUI] = store_factory(gui),
	[STORE_KIND_A] = store_factory(a),
	[STORE_KIND_B] = store_factory(b),
};