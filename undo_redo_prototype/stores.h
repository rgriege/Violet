typedef struct store_a_data {
	char msg[64];
} store_a_data_t;

typedef struct store_a {
	store_kind_e kind;
	store_a_data_t data;
} store_a_t;

typedef struct store_b_data {
	char msg[64];
} store_b_data_t;

typedef struct store_b {
	store_kind_e kind;
	store_b_data_t data;
} store_b_t;

typedef struct store_gui_data {
	b32 chk;
	b32 chk2;
	s32 increment;
	r32 slider;
	r32 slider2;
	char npt[64];
	array(u32) arr;
} store_gui_data_t;

typedef struct store_gui {
	store_kind_e kind;
	store_gui_data_t data;
} store_gui_t;

/* use for concrete store implementation(s), when there is no memory to clean up */
static
void store__destroy_noop(store_t *store, allocator_t *alc)
{
	afree(store, alc);
}

static
store_a_t *store_a_create(allocator_t *alc)
{
	store_a_t *store_a = amalloc(sizeof(store_a_t), alc);
	store_a->kind = STORE_KIND_A;
	store_a->data = (store_a_data_t){
		.msg = "you triggered store_a",
	};
	return store_a;
}

static
store_kind_e store_a__get_kind(store_a_t *store)
{
	return store->kind;
}

static
void *store_a__get_data(store_a_t *store)
{
	return &store->data;
}

static
store_contract_t *polymorph_store_a = &(store_contract_t) {
	.get_kind = (store_kind_e (*)(void *))store_a__get_kind,
	.get_data = (void *(*)(void *))store_a__get_data,
	.destroy = (void (*)(void *, allocator_t *))store__destroy_noop,
};

static
store_b_t *store_b_create(allocator_t *alc)
{
	store_b_t *store_b = amalloc(sizeof(store_b_t), alc);
	store_b->kind = STORE_KIND_B;
	store_b->data = (store_b_data_t){
		.msg = "you triggered store_b",
	};
	return store_b;
}

static
store_kind_e store_b__get_kind(store_b_t *store)
{
	return store->kind;
}

static
void *store_b__get_data(store_b_t *store)
{
	return &store->data;
}

static
store_contract_t *polymorph_store_b = &(store_contract_t) {
	.get_kind = (store_kind_e (*)(void *))store_b__get_kind,
	.get_data = (void *(*)(void *))store_b__get_data,
	.destroy = (void (*)(void *, allocator_t *))store__destroy_noop,
};


static
store_gui_t *store_gui_create(allocator_t *alc)
{
	store_gui_t *store_gui = amalloc(sizeof(store_gui_t), alc);
	store_gui->kind = STORE_KIND_GUI;
	store_gui->data = (store_gui_data_t){
		.npt = "Do you even tater, bro?",
		.arr = array_create_ex(alc),
	};
	return store_gui;
}

static
void store_gui__destroy(store_gui_t *store, allocator_t *alc)
{
	array_destroy(store->data.arr);
	afree(store, alc);
}

static
store_kind_e store_gui__get_kind(store_gui_t *store)
{
	return store->kind;
}

static
void *store_gui__get_data(store_gui_t *store)
{
	return &store->data;
}

static
store_contract_t *polymorph_store_gui = &(store_contract_t) {
	.get_kind = (store_kind_e (*)(void *))store_gui__get_kind,
	.get_data = (void *(*)(void *))store_gui__get_data,
	.destroy = (void (*)(void *, allocator_t *))store_gui__destroy,
};

array(store_t *) stores__init(allocator_t *alc)
{
	store_gui_t *store_gui = store_gui_create(alc);
	store_a_t *store_a = store_a_create(alc);
	store_b_t *store_b = store_b_create(alc);

	store_t *gui = store_create(store_gui, polymorph_store_gui, alc);
	store_t *a   = store_create(store_a, polymorph_store_a, alc);
	store_t *b   = store_create(store_b, polymorph_store_b, alc);

	array(store_t *) stores = array_create_ex(g_allocator);
	array_append(stores, gui);
	array_append(stores, a);
	array_append(stores, b);
	return stores;
}
