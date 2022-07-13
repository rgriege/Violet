typedef enum event_kind
{
	EVENT_KIND_GUI_TOGGLE_CHK = 3, // begins after EVENT_KIND_REDO
	EVENT_KIND_GUI_ARR,
	EVENT_KIND_GUI_NPT_CHANGE,
	EVENT_KIND_GUI_SLIDER_CHANGE,
	EVENT_KIND_GUI_PLUS_MINUS,
	EVENT_KIND__COUNT,
} event_kind_e;

static const event_metadata_t g_event_registry[EVENT_KIND__COUNT];

static const event_metadata_t *event_metadata__from_kind(event_kind_e kind)
{
	return &g_event_registry[kind];
}

/* returns the concrete instance of a new event, according to the specified kind */
static void *event_spawn(event_kind_e kind)
{
	const event_metadata_t *meta = event_metadata__from_kind(kind);
	return transaction_spawn_event(meta, "Prototype", kind);
}

static event_t *event_from_instance(void *instance)
{
	return ((event_t*)instance) - 1;
}

/* Concrete Event Implementations */

#pragma region gui_toggle_chk
typedef struct event_gui_toggle_chk
{
	b32 value;
	b32 value_before;
} event_gui_toggle_chk_t;

static b32 event_gui_toggle_chk__execute(event_gui_toggle_chk_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	event->value_before = data->chk;
	data->chk = event->value;
	log_debug("Toggled checkbox %s", event->value ? "on" : "off");
	return true;
}

static void event_gui_toggle_chk__undo(event_gui_toggle_chk_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	data->chk = event->value_before;
	log_debug("Reverted checkbox toggle");
}
#pragma endregion gui_toggle_chk

#pragma region gui_arr
typedef enum list_operation
{
	LIST_OP_UPDATE,
	LIST_OP_APPEND,
	LIST_OP_POP,
} list_operation_e;

/* this event could be split into 3 separate events, or simply re-used, since the events
   are closely associated with one another */
typedef struct event_gui_arr
{
	list_operation_e op;
	optional(u32) value;
	optional(u32) idx;
	optional(u32) value_before;
} event_gui_arr_t;

static void event_gui_arr__create(event_gui_arr_t *event, allocator_t *alc)
{
	event->value = acalloc(1, sizeof(u32), alc);
	event->idx = acalloc(1, sizeof(u32), alc);
	event->value_before = acalloc(1, sizeof(u32), alc);
}

static void event_gui_arr__destroy(event_gui_arr_t *event, allocator_t *alc)
{
	afree(event->value, alc);
	afree(event->idx, alc);
	afree(event->value_before, alc);
}

/* notice that 1. there may be conditions where the execution fails, so */
static b32 event_gui_arr__execute(event_gui_arr_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);

	switch ((list_operation_e)event->op)
	{
	case LIST_OP_UPDATE:
		if (array_sz(data->arr) <= *event->idx)
			return false;

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
		if (array_empty(data->arr))
			return false;

		*event->value_before = array_last(data->arr);
		array_pop(data->arr);
		log_debug("Popped   %d", *event->value_before);
		break;
	}

	return true;
}

static void event_gui_arr__undo(const event_gui_arr_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);

	switch ((list_operation_e)event->op)
	{
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
#pragma endregion gui_arr

#pragma region gui_npt_change
typedef struct event_gui_npt_change
{
	char value[64];
	char value_before[64];
} event_gui_npt_change_t;

static b32 event_gui_npt_change__execute(event_gui_npt_change_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	memcpy(event->value_before, B2PC(data->npt));
	memcpy(data->npt, B2PC(event->value));
	log_debug("Text input change");
	return true;
}

static void event_gui_npt_change__undo(event_gui_npt_change_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	memcpy(data->npt, B2PC(event->value_before));
	log_debug("Reverted text input change");
}

static void event_gui_npt_change__update(event_gui_npt_change_t *dst,
										 const event_gui_npt_change_t *src)
{
	memcpy(dst->value, B2PC(src->value));
}
#pragma endregion gui_npt_change

#pragma region gui_slider
typedef struct event_gui_slider_change
{
	r32 value;
	r32 value_before;
} event_gui_slider_change_t;

static b32 event_gui_slider_change__execute(event_gui_slider_change_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	event->value_before = data->slider;
	data->slider = event->value;
	log_debug("Slider change");
	return true;
}

static void event_gui_slider_change__undo(event_gui_slider_change_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	data->slider = event->value_before;
	log_debug("Reverted slider change");
}

static void event_gui_slider_change__update(event_gui_slider_change_t *dst,
											const event_gui_slider_change_t *src)
{
	dst->value = src->value;
}
#pragma endregion gui_slider

#pragma region gui_plus_minus
typedef struct event_gui_plus_minus_t
{
	s32 value;
	s32 value_before;
} event_gui_plus_minus_t;

static b32 event_gui_plus_minus__execute(event_gui_plus_minus_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	event->value_before = data->increment;
	data->increment = event->value;
	log_debug("Plus / Minus change");
	return true;
}

static void event_gui_plus_minus__undo(event_gui_plus_minus_t *event)
{
	store_gui_t *data = store_data(STORE_KIND_GUI);
	data->increment = event->value_before;
	log_debug("Reverted plus / minus change");
}

static void event_gui_plus_minus__update(event_gui_plus_minus_t *dst, const event_gui_plus_minus_t *src)
{
	dst->value = src->value;
}
#pragma endregion gui_plus_minus

/* Event Registry */
static const event_metadata_t g_event_registry[EVENT_KIND__COUNT] = {
	[EVENT_KIND_NOOP] = {0},
	[EVENT_KIND_UNDO] = {
		.contract = &event_undo__contract,
		.description = "Undo",
	},
	[EVENT_KIND_REDO] = {
		.contract = &event_redo__contract,
		.description = "Redo",
	},
	[EVENT_KIND_GUI_TOGGLE_CHK] = {
		event_factory(gui_toggle_chk),
		"Toggle GUI Checkbox #1",
	},
	[EVENT_KIND_GUI_ARR] = {
		event_factory_dynamic(gui_arr),
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
