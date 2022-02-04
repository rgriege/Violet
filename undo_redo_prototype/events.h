typedef enum event_kind {
	EVENT_KIND_GUI_TOGGLE_CHK = 3, // begins after EVENT_KIND_REDO
	EVENT_KIND_GUI_ARR,
	EVENT_KIND_GUI_NPT_CHANGE,
	EVENT_KIND_GUI_SLIDER_CHANGE,
	EVENT_KIND_GUI_PLUS_MINUS,
	EVENT_KIND__COUNT,
} event_kind_e;

static const event_metadata_t g_event_registry[EVENT_KIND__COUNT];

static
const event_metadata_t *event_metadata__from_kind(event_kind_e kind)
{
	return &g_event_registry[kind];
}

/* returns the concrete instance of a new event, according to the specified kind */
static
void *event_spawn_from_kind(event_kind_e kind)
{
	const event_metadata_t *meta = event_metadata__from_kind(kind);
	return transaction_spawn_event(meta, kind);
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
	event_alloc(gui_toggle_chk, event, alc);
	return event;
}

static
b32 event_gui_toggle_chk__execute(struct event_gui_toggle_chk *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
    event->value_before = data->chk;
	data->chk = event->value;
	log_debug("Toggled checkbox %s", event->value ? "on" : "off");
	return true;
}

static
void event_gui_toggle_chk__undo(struct event_gui_toggle_chk *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
    data->chk = event->value_before;
	log_debug("Reverted checkbox toggle");
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
	event_alloc(gui_arr, event, alc);
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
b32 event_gui_arr__execute(struct event_gui_arr *event)
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

	return true;
}

static
void event_gui_arr__undo(const struct event_gui_arr *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);

	switch ((list_operation_e)event->op) {
	case LIST_OP_UPDATE:
		data->arr[*event->idx] = *event->value_before;
		log_debug("Reverted update at idx %d", *event->idx);
	break;
	case LIST_OP_APPEND:
		array_pop(data->arr);
		log_debug("Reverted append of value %d", *event->value);
	break;
	case LIST_OP_POP:
		array_append(data->arr, *event->value_before);
		log_debug("Reverted pop of value %d", *event->value_before);
	break;
	}
}

struct event_gui_npt_change
{
	char value[64];
	char value_before[64];
};

static
struct event_gui_npt_change *event_gui_npt_change__create(allocator_t *alc)
{
	event_alloc(gui_npt_change, event, alc);
	return event;
}

static
b32 event_gui_npt_change__execute(struct event_gui_npt_change *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
	memcpy(event->value_before, B2PC(data->npt));
	memcpy(data->npt, B2PC(event->value));
	log_debug("Text input change");
	return true;
}

static
void event_gui_npt_change__undo(struct event_gui_npt_change *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
	memcpy(data->npt, B2PC(event->value_before));
	log_debug("Reverted text input change");
}

static
void event_gui_npt_change__update(struct event_gui_npt_change *dst,
                                  const struct event_gui_npt_change *src)
{
	memcpy(dst->value, B2PC(src->value));
}

struct event_gui_slider_change
{
	r32 value;
	r32 value_before;
};

static
struct event_gui_slider_change *event_gui_slider_change__create(allocator_t *alc)
{
	event_alloc(gui_slider_change, event, alc);
	return event;
}

static
b32 event_gui_slider_change__execute(struct event_gui_slider_change *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
	event->value_before = data->slider;
	data->slider = event->value;
	log_debug("Slider change");
	return true;
}

static
void event_gui_slider_change__undo(struct event_gui_slider_change *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
	data->slider = event->value_before;
	log_debug("Reverted slider change");
}

static
void event_gui_slider_change__update(struct event_gui_slider_change *dst, const struct event_gui_slider_change *src)
{
	dst->value = src->value;
}

struct event_gui_plus_minus
{
	s32 value;
	s32 value_before;
};

static
struct event_gui_plus_minus *event_gui_plus_minus__create(allocator_t *alc)
{
	event_alloc(gui_plus_minus, event, alc);
	return event;
}

static
b32 event_gui_plus_minus__execute(struct event_gui_plus_minus *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
	event->value_before = data->increment;
	data->increment = event->value;
	log_debug("Plus / Minus change");
	return true;
}

static
void event_gui_plus_minus__undo(struct event_gui_plus_minus *event)
{
	store_gui_data_t *data = store_data_from_kind(STORE_KIND_GUI);
	data->increment = event->value_before;
	log_debug("Reverted plus / minus change");
}

static
void event_gui_plus_minus__update(struct event_gui_plus_minus *dst, const struct event_gui_plus_minus *src)
{
	dst->value = src->value;
}

/* Event Registry */
static const event_metadata_t g_event_registry[EVENT_KIND__COUNT] = {
	[EVENT_KIND_NOOP] = {},
	[EVENT_KIND_UNDO] = {
		.contract = &event_undo__contract,
		.description = "Undo"
	},
	[EVENT_KIND_REDO] = {
		.contract = &event_redo__contract,
		.description = "Redo"
	},
	[EVENT_KIND_GUI_TOGGLE_CHK] = {
		event_factory(gui_toggle_chk),
		"Toggle GUI Checkbox #1",
	},
	[EVENT_KIND_GUI_ARR] = {
		event_factory_explicit_destructor(gui_arr),
		"GUI Array Operation",
	},
	[EVENT_KIND_GUI_NPT_CHANGE] = {
		event_factory_multi_frame(gui_npt_change),
		"GUI Text Input Change",
	},
	[EVENT_KIND_GUI_SLIDER_CHANGE] = {
		event_factory_multi_frame(gui_slider_change),
		"GUI Slider Change",
	},
	[EVENT_KIND_GUI_PLUS_MINUS] = {
		event_factory_multi_frame(gui_plus_minus),
		"GUI Increment Change",
	},
};
