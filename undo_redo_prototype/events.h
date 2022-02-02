typedef enum event_kind {
	// EVENT_KIND_NOOP,
	// EVENT_KIND_UNDO,
	// EVENT_KIND_REDO,
	EVENT_KIND_GUI_TOGGLE_CHK,
	EVENT_KIND_GUI_ARR,
	EVENT_KIND__COUNT,
} event_kind_e;

static const event_metadata_t g_event_registry[EVENT_KIND__COUNT];

void *event_spawn_from_kind(event_kind_e kind)
{
	allocator_t *alc = array__allocator(g_events);
	void *instance = g_event_registry[kind].spawner(alc);
	const event_contract_t *contract = &g_event_registry[kind].contract;
	event_t *event = event_create(kind, instance, contract, alc);
	array_append(g_events, event);
	return event->instance;
}

void events_flush()
{
	allocator_t *alc = array__allocator(g_events);
	array_foreach(g_events, event_t *, event_ptr) {
		event_execute(*event_ptr);
		event_destroy(*event_ptr, alc);
	}
	array_clear(g_events);
}

/* Concrete Event Implementations */
struct event_gui_toggle_chk
{
	b32 value;
	b32 value_before;
};

static
struct event_gui_toggle_chk *event_gui_toggle_chk__create(allocator_t *alc)
{
	struct event_gui_toggle_chk *event = acalloc(1, sizeof(struct event_gui_toggle_chk), alc);
	return event;
}

static
void event_gui_toggle_chk__execute(struct event_gui_toggle_chk *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
    event->value_before = data->chk;
	data->chk = event->value;
	log_debug("Toggled checkbox %s", event->value ? "on" : "off");
}

static
void event_gui_toggle_chk__undo(struct event_gui_toggle_chk *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
    data->chk = event->value_before;
}

struct event_gui_arr
{
	list_operation_e op;
	optional(u32) value;
	optional(u32) idx;
	optional(u32) value_before;
};

static
struct event_gui_arr *event_gui_arr__create(allocator_t *alc)
{
	struct event_gui_arr *event = acalloc(1, sizeof(struct event_gui_arr), alc);
	event->value = acalloc(1, sizeof(u32), alc);
	event->idx = acalloc(1, sizeof(u32), alc);
	event->value_before = acalloc(1, sizeof(u32), alc);
	return event;
}

static
void event_gui_arr__destroy(struct event_gui_arr *event, allocator_t *alc)
{
	afree(event->value, alc);
	afree(event->idx, alc);
    afree(event->value_before, alc);
	event__destroy_noop(event, alc);
}

static
void event_gui_arr__execute(struct event_gui_arr *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);

	switch ((list_operation_e)event->op) {
	case LIST_OP_UPDATE:
		*event->value_before = data->arr[*event->idx];
		data->arr[*event->idx] = *event->value;
		log_debug("Updated from %d to %d at idx %d",
		          *event->value_before, *event->value, *event->idx);
	break;
	case LIST_OP_APPEND:
		array_append(data->arr, *event->value);
		log_debug("Appended %d", *event->value);
	break;
	case LIST_OP_POP:
		*event->value_before = array_last(data->arr);
		array_pop(data->arr);
		log_debug("Popped   %d", *event->value_before);
	break;
	}
}

static
void event_gui_arr__undo(const struct event_gui_arr *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);

	switch ((list_operation_e)event->op) {
	case LIST_OP_UPDATE:
		data->arr[*event->idx] = *event->value_before;
	break;
	case LIST_OP_APPEND:
		array_pop(data->arr);
	break;
	case LIST_OP_POP:
		array_append(data->arr, *event->value_before);
	break;
	}
}

/* Event Registry */
static const event_metadata_t g_event_registry[EVENT_KIND__COUNT] = {
	// [EVENT_KIND_NOOP]              = { NULL, NULL, "no-op" },
	// [EVENT_KIND_UNDO]              = { NULL, NULL, "undo" },
	// [EVENT_KIND_REDO]              = { NULL, NULL, "redo" },
	[EVENT_KIND_GUI_TOGGLE_CHK] = {
		event_factory(gui_toggle_chk),
		"Toggle GUI Checkbox #1",
	},
	[EVENT_KIND_GUI_ARR] = {
		event_factory_explicit_destructor(gui_arr),
		"GUI Array Operation",
	},
};
