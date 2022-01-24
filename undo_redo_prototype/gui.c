#include <string.h>

#include "GL/glew.h"

#define GUI_MAX_VERTS 16384
#define GUI_MAX_DRAW_CALLS 4096
#define GUI_MAX_SCISSORS 32
#define GUI_USE_CURSOR_BUTTON

#define ARRAY_IMPLEMENTATION
#define COLOR_IMPLEMENTATION
#define CORE_IMPLEMENTATION
#define FMATH_IMPLEMENTATION
#define GEOM_IMPLEMENTATION
#define GUI_IMPLEMENTATION
#define IMATH_IMPLEMENTATION
#define LIST_IMPLEMENTATION
#define OS_IMPLEMENTATION
#define PROFILER_IMPLEMENTATION
#define SDL_GL_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#define UTF8_IMPLEMENTATION

#include "violet/core.h"
#include "violet/array.h"
#include "violet/fmath.h"
#include "violet/geom.h"
#include "violet/imath.h"
#include "violet/color.h"
#include "violet/gui.h"
#include "violet/os.h"
#include "violet/string.h"
#include "violet/utf8.h"
#include "violet/profiler.h"
#include "violet/sdl_gl.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define MENU_HEIGHT 20
#define APP_NAME "Undo / Redo UI Prototype"

#define FPS_CAP 30
#define LOG_LINE_COUNT 100
#define LOG_LINE_SZ 128

enum panel
{
	PANEL_WIDGETS = 1,
	PANEL_LOG,
	PANEL_ABOUT,
};
#define PANEL_COUNT 3

struct log_data
{
	char buf[LOG_LINE_COUNT][LOG_LINE_SZ];
	s32 level[LOG_LINE_COUNT];
	u32 start, cnt;
};

#define PAYLOAD_LIMIT_PRIMITIVE 8  // bytes

typedef struct payload_primitive {
	union {
		b32 b32;
		u32 u32;
		s32 s32;
		r32 r32;
		unsigned char bytes[PAYLOAD_LIMIT_PRIMITIVE];
	} bytes;
	u32 size;
} payload_primitive_t;

typedef struct payload_dynamic {
	array(unsigned char) bytes;
} payload_dynamic_t;

static
payload_dynamic_t payload_dynamic_create(allocator_t *alc)
{
	return (payload_dynamic_t) {
		.bytes = array_create_ex(alc),
	};
}

static
void payload_dynamic_destroy(payload_dynamic_t *payload)
{
	array_destroy(payload->bytes);
}

#define payload_primitive(value, type, payload) \
	assert(sizeof(type) <= PAYLOAD_LIMIT_PRIMITIVE); \
	payload_primitive_t (payload) = { \
		.bytes.type = value, \
		.size = sizeof(type), \
	};

#define payload_dynamic_from_buffer(buf, alc, payload) \
	payload_dynamic_t (payload) = payload_dynamic_create(alc); \
	array_appendn(payload.bytes, (unsigned char *)buf, sizeof(buf));

typedef enum command_kind {
	COMMAND_KIND_MODIFY_PRIMITIVE,  // a single field
	COMMAND_KIND_MODIFY_BUFFER,     // a single statically allocated buffer
	COMMAND_KIND_REPLACE,           // an entire struct, whose size is compile time constant
} command_kind_e;

typedef enum store_kind {
	STORE_KIND_GUI,
	STORE_KIND_A,
	STORE_KIND_B,
} store_kind_e;

typedef union payload {
	payload_primitive_t primitive;
	payload_dynamic_t   dynamic;
} payload_t;

typedef struct command {
	command_kind_e kind;
	store_kind_e store_kind;
	u32 offset;
	u32 transaction_id;
	payload_t payload;
} command_t;

typedef struct revertable_command {
	command_kind_e kind;
	store_kind_e store_kind;
	u32 offset;
	u32 transaction_id;
	/* should only be used for comparing multi-frame transactions,
	   to know if back-to-back interactions target the same mutation */
	void *dst;
	array(unsigned char) bytes_before;
	array(unsigned char) bytes_after;
} revertable_command_t;

/* achieve polymorphism via C++ style inheritance, manually, of course */
typedef struct store_interface {
	store_kind_e (*get_kind)(void *instance);
	void *       (*get_data)(void *instance);
	void         (*destroy )(void *instance);
} store_interface_t;

typedef struct store {
	void *instance;
	const store_interface_t *interface;
} store_t;

store_t *store_create(void *instance, store_interface_t *interface, allocator_t *alc)
{
	store_t *store   = amalloc(sizeof(store_t), alc);
	store->instance  = instance;
	store->interface = interface;
	return store;
}

void store_destroy(store_t *store, allocator_t *alc)
{
	(store->interface->destroy)(store->instance);
	afree(store, alc);
}

store_kind_e store_get_kind(const store_t *store)
{
	return (store->interface->get_kind)(store->instance);
}

void *store_get_data(const store_t *store)
{
	return (store->interface->get_data)(store->instance);
}

#define store_offset_bytes(store, cmd, dst) \
	unsigned char *(dst) = (unsigned char *)store_get_data(store) + (cmd)->offset;

// TODO(luke): should we replace this with something that calls into
//             store_instance_from_kind and/or store_get_data()?
#define store_offsetof(store_type, member) \
	offsetof(store_type, data.member) - offsetof(store_type, data)

typedef struct megastore {
	array(store_t *) d;
	allocator_t *alc;
} megastore_t;


   typedef struct transaction_system {
	u32 current_id;
	store_kind_e current_store_kind;  // TODO(luke): make this into a stack, so we can push & pop to it,
	                                  //             similar to the idiom fof gui styles.
									  //             That should enable us to encapsulate which kind
									  //             should be applied to mutations as they are enqueued.
	b32 multi_frame;
	b32 pending;
	megastore_t mega;
	array(command_t) command_queue;
	array(revertable_command_t) undo_stack;
	array(revertable_command_t) redo_stack;
} transaction_system_t;

transaction_system_t g_transaction_system;

static
transaction_system_t transaction_system_create(megastore_t mega, allocator_t *alc)
{
	return (transaction_system_t) {
		.mega          = mega,
		.command_queue = array_create_ex(alc),
		.undo_stack    = array_create_ex(alc),
		.redo_stack    = array_create_ex(alc),
	};
}

void megastore_destroy(megastore_t mega)
{
	array_foreach(mega.d, store_t *, store_ptr)
		store_destroy(*store_ptr, mega.alc);
}

/* having two or more store_t with the same store_kind causes undefined behavior */
void megastore_append(megastore_t *mega, store_t *store)
{
	b32 store_kind_already_exists = false;
	store_kind_e kind = store_get_kind(store);
	array_foreach(mega->d, store_t *, store_ptr)
		if (store_get_kind(*store_ptr) == kind)
			store_kind_already_exists |= true;
	assert(!store_kind_already_exists);

	array_append(mega->d, store);
}

static
void transaction_system_destroy(transaction_system_t *sys)
{
	megastore_destroy(sys->mega);
	array_destroy(sys->command_queue);
	array_destroy(sys->undo_stack);
	array_destroy(sys->redo_stack);
}

typedef struct store_a_data {
	b32 chk;
	r32 slider;
	char npt[64];
} store_a_data_t;

typedef struct store_a {
	store_kind_e kind;
	store_a_data_t data;
} store_a_t;

store_a_t *store_a_create(allocator_t *alc)
{
	store_a_t *store_a = amalloc(sizeof(store_a_t), alc);
	store_a->kind = STORE_KIND_A;
	store_a->data = (store_a_data_t){};
	return store_a;
}

store_kind_e store_a__get_kind(store_a_t *store)
{
	return store->kind;
}

void *store_a__get_data(store_a_t *store)
{
	return &store->data;
}

/* use for concrete store implementation(s), when there is no memory to clean up */
void store__noop(store_t *store)
{
	NOOP;
}

store_interface_t *polymorph_store_a = &(store_interface_t) {
	.get_kind        = (store_kind_e (*)(void *))store_a__get_kind,
	.get_data        = (void *       (*)(void *))store_a__get_data,
	.destroy         = (void         (*)(void *))store__noop,
};

typedef struct store_b_data {
	u32 value;
} store_b_data_t;

typedef struct store_b {
	store_kind_e kind;
	store_b_data_t data;
} store_b_t;

store_b_t *store_b_create(allocator_t *alc)
{
	store_b_t *store_b = amalloc(sizeof(store_b_t), alc);
	store_b->kind = STORE_KIND_B;
	store_b->data = (store_b_data_t){};
	return store_b;
}

store_kind_e store_b__get_kind(store_b_t *store)
{
	return store->kind;
}

void *store_b__get_data(store_b_t *store)
{
	return &store->data;
}

store_interface_t *polymorph_store_b = &(store_interface_t) {
	.get_kind        = (store_kind_e (*)(void *))store_b__get_kind,
	.get_data        = (void *       (*)(void *))store_b__get_data,
	.destroy         = (void         (*)(void *))store__noop,
};

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

store_gui_t *store_gui_create(allocator_t *alc)
{
	store_gui_t *store_gui = amalloc(sizeof(store_gui_t), alc);
	store_gui->kind = STORE_KIND_GUI;
	store_gui->data = (store_gui_data_t){
		.npt = "Why are you the way that you are?",
		.arr = array_create_ex(alc),
	};
	return store_gui;
}

void store_gui__destroy(store_gui_t *store)
{
	array_destroy(store->data.arr);
}

store_kind_e store_gui__get_kind(store_gui_t *store)
{
	return store->kind;
}

void *store_gui__get_data(store_gui_t *store)
{
	return &store->data;
}

store_interface_t *polymorph_store_gui = &(store_interface_t) {
	.get_kind        = (store_kind_e (*)(void *))store_gui__get_kind,
	.get_data        = (void *       (*)(void *))store_gui__get_data,
	.destroy         = (void         (*)(void *))store_gui__destroy,
};

static
megastore_t megastore__create(allocator_t *alc)
{
	store_gui_t *store_gui = store_gui_create(alc);
	store_a_t *store_a = store_a_create(alc);
	store_b_t *store_b = store_b_create(alc);

	store_t *gui = store_create(store_gui, polymorph_store_gui, alc);
	store_t *a   = store_create(store_a, polymorph_store_a, alc);
	store_t *b   = store_create(store_b, polymorph_store_b, alc);

	megastore_t mega = { .d = array_create_ex(alc), .alc = alc };
	megastore_append(&mega, gui);
	megastore_append(&mega, a);
	megastore_append(&mega, b);
	return mega;
}

static
void revertable_command_init(revertable_command_t *cmd, const command_t *command, allocator_t *alc)
{
	*cmd = (revertable_command_t){
		.kind = command->kind,
		.offset = command->offset,
		.transaction_id = command->transaction_id,
	};
	cmd->bytes_before = array_create_ex(alc);
	cmd->bytes_after = array_create_ex(alc);
}

static
void revertable_command_destroy(revertable_command_t *cmd)
{
	array_destroy(cmd->bytes_before);
	array_destroy(cmd->bytes_after);
}

static
void command__copy_bytes(revertable_command_t *cmd, void *dst, const void *src, u32 n)
{
	cmd->dst = dst;
	array_appendn(cmd->bytes_before, dst, n);
	memcpy(dst, src, n);
	array_appendn(cmd->bytes_after, dst, n);
}

static
store_t *store_from_kind(store_kind_e kind)
{
	array_foreach(g_transaction_system.mega.d, store_t *, store_ptr) {
		if (store_get_kind(*store_ptr) == kind)
			return *store_ptr;
	}
	assert(false);
	return NULL;
}

static
void *store_instance_from_kind(store_kind_e kind)
{
	store_t *store = store_from_kind(kind);
	return store->instance;
}

// TODO(luke): this function needs to be responsible for undoing any changes if they prove invalid
//             as well as cleaning up any dynamic memory allocacted with the payload generation
b32 store_execute_command(const store_t *store, u32 command_idx, b32 replace_prev)
{
	revertable_command_t cmd;
	transaction_system_t *t = &g_transaction_system;
	command_t *command = &t->command_queue[command_idx];

	if (!t->multi_frame)
		assert(!replace_prev);

	switch (command->kind) {
	case COMMAND_KIND_MODIFY_PRIMITIVE:
	{
		const payload_primitive_t *payload = &command->payload.primitive;
		store_offset_bytes(store, command, dst);

		if (replace_prev) {
			// CLEANUP: de-dup with other COMMAND_KINDs
			memcpy(dst, &payload->bytes, payload->size);
			/* get the previously issued command that aligns to the current idx
			   in the current queue */
			revertable_command_t *prev_cmd = &t->undo_stack[  array_sz(t->undo_stack)
			                                                - array_sz(t->command_queue)
			                                                + command_idx];
			array_clear(prev_cmd->bytes_after);
			array_appendn(prev_cmd->bytes_after, dst, payload->size);
		} else {
			allocator_t *alc = array__allocator(t->undo_stack);
			revertable_command_init(&cmd, command, alc);
			command__copy_bytes(&cmd, dst, &payload->bytes, payload->size);
		}
	}
	break;
	case COMMAND_KIND_MODIFY_BUFFER:
	{
		payload_dynamic_t *payload = &command->payload.dynamic;
		store_offset_bytes(store, command, dst);
		const u32 size = array_sz(payload->bytes);

		if (replace_prev) {
			// CLEANUP: de-dup with other COMMAND_KINDs
			memcpy(dst, payload->bytes, size);
			/* get the previously issued command that aligns to the current idx
			   in the current queue */
			revertable_command_t *prev_cmd = &t->undo_stack[  array_sz(t->undo_stack)
			                                                - array_sz(t->command_queue)
			                                                + command_idx];
			array_clear(prev_cmd->bytes_after);
			array_appendn(prev_cmd->bytes_after, dst, size);
		} else {
			allocator_t *alc = array__allocator(t->undo_stack);
			revertable_command_init(&cmd, command, alc);
			command__copy_bytes(&cmd, dst, payload->bytes, size);
		}

		payload_dynamic_destroy(payload);
	}
	break;
	case COMMAND_KIND_REPLACE:
		NOOP;
	break;
	default:
		NOOP;
	break;
	}

	if (!replace_prev)
		array_append(t->undo_stack, cmd);

	return true;

// err:
// 	return false;

}

static
void transaction_begin()
{
	g_transaction_system.multi_frame = false;
	g_transaction_system.pending = true;
	array_clear(g_transaction_system.command_queue);
}

/* Addresses the use case when some data mutation might be repeated with a
   different payload. If the desired undo behavior is such that it reverts
   to the state before any of the repeated actions happened, then instead
   of creating a new undo point with every transaction, the previous
   transaction's undo point is modified. */
static
void transaction_begin_multi_frame()
{
	g_transaction_system.multi_frame = true;
	g_transaction_system.pending = true;
	array_clear(g_transaction_system.command_queue);
}

static
u32 transaction_prev_n_commands()
{
	transaction_system_t *t = &g_transaction_system;

	if (array_sz(t->undo_stack) == 0)
		return 0;

	u32 result = 1;
	u32 prev_frame_trans_id = array_last(t->undo_stack).transaction_id;
	for (s32 i = array_sz(t->undo_stack)-2; i >= 0 ;--i) {
		if (prev_frame_trans_id != t->undo_stack[i].transaction_id)
			break;
		++result;
	}
	return result;
}

static
b32 command_revert(megastore_t *mega, revertable_command_t *cmd)
{
	store_t *store = store_from_kind(cmd->store_kind);

	switch (cmd->kind) {
	case COMMAND_KIND_MODIFY_PRIMITIVE:
	case COMMAND_KIND_MODIFY_BUFFER:
		;
		store_offset_bytes(store, cmd, dst);
		assert(cmd->dst == dst);
		memcpy(dst, cmd->bytes_before, array_sz(cmd->bytes_before));
	break;
	case COMMAND_KIND_REPLACE:
		NOOP;
	break;
	default:
		NOOP;
	break;
	}
	return true;
}

static
b32 command_unwind_history(u32 unwind_count)
{
	transaction_system_t *t = &g_transaction_system;

	for (u32 i = 0, start_idx = array_sz(t->undo_stack)-1; i < unwind_count; ++i)
		if (command_revert(&t->mega, &t->undo_stack[start_idx-i]))
			array_pop(t->undo_stack);
		else
			return false;
	return true;
}

// CLEANUP: de-dup with store_redo
static
b32 transaction_undo()
{
	transaction_system_t *t = &g_transaction_system;

	if (array_sz(t->undo_stack) == 0)
		return false;

	do {
		revertable_command_t *cmd = &array_last(t->undo_stack);
		if (!command_revert(&t->mega, cmd))
			/* something went very haywire - expect undefined behavior */
			assert(false);

		array(unsigned char) swap = cmd->bytes_before;
		cmd->bytes_before = cmd->bytes_after;
		cmd->bytes_after = swap;

		array_append(t->redo_stack, array_last(t->undo_stack));
		array_pop(t->undo_stack);
	} while (   array_sz(t->undo_stack) > 0
	         && array_last(t->undo_stack).transaction_id
	         == array_last(t->redo_stack).transaction_id);

	--g_transaction_system.current_id;

	return true;
}

// CLEANUP: de-dup with store_undo
static
b32 transaction_redo()
{
	transaction_system_t *t = &g_transaction_system;

	if (array_sz(t->redo_stack) == 0)
		return false;

	do {
		revertable_command_t *cmd = &array_last(t->redo_stack);
		if (!command_revert(&t->mega, cmd))
			/* something went very haywire - expect undefined behavior */
			assert(false);

		array(unsigned char) swap = cmd->bytes_before;
		cmd->bytes_before = cmd->bytes_after;
		cmd->bytes_after = swap;

		array_append(t->undo_stack, array_last(t->redo_stack));
		array_pop(t->redo_stack);
	} while (   array_sz(t->redo_stack) > 0
	         && array_last(t->undo_stack).transaction_id
	         == array_last(t->redo_stack).transaction_id);

	++g_transaction_system.current_id;

	return true;
}

static
b32 transaction_should_replace_prev(megastore_t *mega)
{
	transaction_system_t *t = &g_transaction_system;

	if (!g_transaction_system.multi_frame || array_sz(t->undo_stack) == 0)
		return false;

	u32 prev_trans_n_cmds = transaction_prev_n_commands();
	u32 curr_trans_n_cmds = array_sz(g_transaction_system.command_queue);

	if (prev_trans_n_cmds != curr_trans_n_cmds)
		return false;

	for (u32 i = 0; i < curr_trans_n_cmds; ++i) {
		const command_t *command = &t->command_queue[i];
		store_t *store = store_from_kind(command->kind);
		store_offset_bytes(store, command, dst);
		if (t->undo_stack[array_sz(t->undo_stack) - prev_trans_n_cmds + i].dst != dst)
			return false;
	}

	return true;
}

static
void transaction__clear_redo_history()
{
	transaction_system_t *t = &g_transaction_system;

	array_foreach(t->redo_stack, revertable_command_t, cmd)
		revertable_command_destroy(cmd);
	array_clear(t->redo_stack);

}

static
b32 transaction_commit()
{
	u32 success_count;
	transaction_system_t *t = &g_transaction_system;
	const b32 replace_prev = transaction_should_replace_prev(&t->mega);

	for (u32 i = 0, n = array_sz(t->command_queue); i < n; ++i) {
		command_t *command = &t->command_queue[i];
		const store_t *store = store_from_kind(command->store_kind);
		if (store_execute_command(store, i, replace_prev)) {
			success_count++;
		} else {
			if (!command_unwind_history(success_count))
				/* something went very haywire - expect undefined behavior */
				assert(false);
			return false;
		}
	}

	array_clear(t->command_queue);
	transaction__clear_redo_history();

	if (!replace_prev)
		g_transaction_system.current_id++;

	g_transaction_system.pending = false;
	return true;
}

void transaction_set_store_kind(store_kind_e kind)
{
	g_transaction_system.current_store_kind = kind;
}

static
void transaction__enqueue_validate(transaction_system_t *t)
{
	assert(t->pending);
	// TODO(luke) assert that the transaction system's current kind is the same as the command
	//            that's about to be enqueued. this is to prevent API user footguns
}

void transaction_enqueue_mutation_primitive(payload_primitive_t *payload, u32 offset)
{
	transaction_system_t *t = &g_transaction_system;
	transaction__enqueue_validate(t);

	const command_t command = {
		.kind = COMMAND_KIND_MODIFY_PRIMITIVE,
		.store_kind = t->current_store_kind,
		.payload.primitive = *payload,
		.offset = offset,
		.transaction_id = t->current_id,
	};
	array_append(t->command_queue, command);
}

void transaction_enqueue_mutation_buffer(payload_dynamic_t *payload, u32 offset)
{
	transaction_system_t *t = &g_transaction_system;
	transaction__enqueue_validate(t);

	const command_t command = {
		.kind = COMMAND_KIND_MODIFY_BUFFER,
		.store_kind = t->current_store_kind,
		.payload.dynamic = *payload,    // NOTE(luke): copies ptr from dynamic allocation
		.offset = offset,
		.transaction_id = t->current_id,
	};
	array_append(t->command_queue, command);
}

static
void buffer_logger(void *udata, log_level_e level, const char *format, va_list ap)
{
	struct log_data *data = udata;
	char *p = data->buf[(data->start + data->cnt) % LOG_LINE_COUNT];
	data->level[(data->start + data->cnt) % LOG_LINE_COUNT] = level;
	vsnprintf(p, LOG_LINE_SZ, format, ap);
	if (data->cnt < LOG_LINE_COUNT)
		++data->cnt;
	else
		data->start = (data->start + 1) % LOG_LINE_COUNT;
}

static
void set_element_font_size(gui_element_style_t *style, s32 size)
{
	style->text.size = size;
}

static
void set_widget_font_size(gui_widget_style_t *style, s32 size)
{
	set_element_font_size(&style->inactive, size);
	set_element_font_size(&style->hot, size);
	set_element_font_size(&style->active, size);
	set_element_font_size(&style->disabled, size);
}

static
void set_font_size(gui_t *gui, s32 size)
{
	gui_style_t *style = gui_style(gui);
	style->txt.size = size;
	/* lazy implementation - doesn't set all widgets */
	set_widget_font_size(&style->npt, size);
	set_widget_font_size(&style->btn, size);
	set_widget_font_size(&style->chk, size);
	set_widget_font_size(&style->select, size);
	set_widget_font_size(&style->dropdown.btn, size);
	set_widget_font_size(&style->color_picker.btn, size);
	set_element_font_size(&style->hint, size);
	set_widget_font_size(&style->drag, size);
	set_widget_font_size(&style->tree, size);
}

static
void draw_menu_bar(gui_t *gui, box2i bbox, b32 *quit, gui_panel_t *panels[])
{
	const s32 cols[] = { 40, 0, 20, 20, 20 };

	s32 x, y, w, h;
	gui_grid_t grid;

	gui_style_push(gui, dropdown.btn.inactive.bg_color, gui_style(gui)->panel.bg_color);

	box2i_to_xywh(bbox, &x, &y, &w, &h);
	pgui_grid_begin(gui, &grid, x, y, w, h);
	pgui_row_cellsv(gui, 0, cols);

	if (pgui_menu_begin(gui, "View", 80, PANEL_COUNT + 2)) {
		for (u32 i = 0; i < PANEL_COUNT; ++i) {
			if (pgui_btn_txt(gui, panels[i]->title)) {
				if (panels[i]->closed)
					pgui_panel_open(gui, panels[i]);
				else
					pgui_panel_close(gui, panels[i]);
			}
		}
		if (pgui_btn_txt(gui, "Font +"))
			set_font_size(gui, gui_style(gui)->txt.size + 2);
		if (pgui_btn_txt(gui, "Font -"))
			set_font_size(gui, gui_style(gui)->txt.size - 2);
		pgui_menu_end(gui);
	}

	if (key_pressed(gui, KB_EQUALS) && key_mod(gui, KBM_SHIFT))
		set_font_size(gui, gui_style(gui)->txt.size + 2);
	if (key_pressed(gui, KB_MINUS))
		set_font_size(gui, gui_style(gui)->txt.size - 2);

	gui_style_push_s32(gui, txt.align, GUI_ALIGN_MIDCENTER);
	pgui_cell(gui, &x, &y, &w, &h);
	pgui_txt(gui, APP_NAME);
	gui_style_pop(gui);
	window_drag(gui_window(gui), x, y, w, h);

	if (pgui_btn_pen(gui, gui_pen_window_minimize))
		window_minimize(gui_window(gui));
	if (pgui_btn_pen(gui, gui_pen_window_maximize)) {
		if (window_is_maximized(gui_window(gui)))
			window_restore(gui_window(gui));
		else
			window_maximize(gui_window(gui));
	}
	if (pgui_btn_pen(gui, gui_pen_window_close))
		*quit = true;

	gui_style_pop(gui);
	pgui_grid_end(gui, &grid);
}

static
void draw_split_node_box(gui_t *gui, const gui_split_t *split)
{
	s32 x, y, w, h;
	box2i_to_xywh(split->box, &x, &y, &w, &h);
	pgui_txt(gui, imprintf("[%d %d %d %d]", x, y, w, h));
}

static
void draw_split_node(gui_t *gui, const gui_split_t *split)
{
	if (gui__split_is_leaf(split)) {
		pgui_tree_leaf(gui);
		draw_split_node_box(gui, split);
	} else {
		const b32 recurse = pgui_tree_node_begin(gui);
		draw_split_node_box(gui, split);
		if (recurse) {
			gui_split_t *child = split->children.first;
			while (child) {
				draw_split_node(gui, child);
				child = child->siblings.next;
			}
			pgui_tree_node_end(gui);
		}
	}
}

static
b32 buf_eq(char *a, char* b, u32 n)
{
	return memcmp(a, b, n) == 0;
}

static
s32 pgui_npt_txt_transaction(gui_t *gui, char *lbl, u32 n,
                             const char *hint, gui_npt_flags_e flags)
{
	char buf[n];
	memcpy(buf, lbl, n);

	const s32 result = pgui_npt_txt(gui, B2PS(buf), "Your text here...", 0);
	/* detect changes to the buffer contents */
	if (!buf_eq(buf, lbl, n)) {
		transaction_begin_multi_frame();
		payload_dynamic_from_buffer(buf, g_temp_allocator, payload);
		transaction_enqueue_mutation_buffer(&payload, store_offsetof(store_gui_t, npt));
		transaction_commit();
	}
	return result;
}

static
void draw_widgets(gui_t *gui, r32 row_height, r32 hx)
{
	const s32 cols[] = { 100, 0 };
	const s32 cols_npt[] = { 100, 80 };
	const s32 cols_plus_minus[] = {100, 30, 0, 30};

	store_gui_t *store = (store_gui_t *)store_instance_from_kind(STORE_KIND_GUI);
	/* it's conventient to set the store kind for many potential transactions,
	   but this can be changed at any time, even in the middle of a transaction */
	transaction_set_store_kind(store->kind);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Undo");
	gui_style_push_ptr(gui, btn.hint, "Undo");
	if (pgui_btn_txt(gui, "Undo")) {
		transaction_undo();
		log_debug(imprintf("undo stack size: %d", array_sz(g_transaction_system.undo_stack)));
		log_debug(imprintf("redo stack size: %d", array_sz(g_transaction_system.redo_stack)));
	}
	gui_style_pop(gui);

	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Redo");
	gui_style_push_ptr(gui, btn.hint, "Redo");
	if (pgui_btn_txt(gui, "Redo")) {
		/* not bound to any state change */
		transaction_redo();
		log_debug(imprintf("undo stack size: %d", array_sz(g_transaction_system.undo_stack)));
		log_debug(imprintf("redo stack size: %d", array_sz(g_transaction_system.redo_stack)));
	}
	gui_style_pop(gui);

	pgui_row_empty(gui, hx);

	/* increment */
	s32 increment = store->data.increment;
	pgui_row_cellsv(gui, row_height, cols_plus_minus);
	pgui_txt(gui, "Plus / Minus");

	gui_style_push_widget_s32(gui, npt, text.align, GUI_ALIGN_MIDRIGHT);
	gui_style_push_widget_color(gui, btn, bg_color, g_nocolor);
	gui_style_push(gui, npt.inactive.bg_color, g_nocolor);
	gui_style_push(gui, btn.hot.text.color, gui_style(gui)->btn.active.bg_color);

	if (pgui_btn_txt(gui, "-")) {
		transaction_begin_multi_frame();
		payload_primitive(--increment, s32, payload);
		transaction_enqueue_mutation_primitive(&payload, store_offsetof(store_gui_t, increment));
		transaction_commit();
	}

	gui_style_push_s32(gui, txt.align, GUI_ALIGN_MIDCENTER);
	pgui_txt(gui, imprintf("%d", increment));
	gui_style_pop(gui);

	if (pgui_btn_txt(gui, "+")) {
		transaction_begin_multi_frame();
		payload_primitive(++increment, s32, payload);
		transaction_enqueue_mutation_primitive(&payload, store_offsetof(store_gui_t, increment));
		transaction_commit();
	}

	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop_widget(gui);
	gui_style_pop_widget(gui);

	pgui_row_empty(gui, hx);

	/* toggles */
	b32 checked = store->data.chk;
	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Checkbox");
	gui_style_push_ptr(gui, chk.hint, "Checkbox");
	if (pgui_chk(gui, checked ? "Checked" : "Unchecked", &checked)) {
		{
			transaction_begin();
			payload_primitive(checked, b32, payload);
			transaction_enqueue_mutation_primitive(&payload, store_offsetof(store_gui_t, chk));
			transaction_enqueue_mutation_primitive(&payload, store_offsetof(store_gui_t, chk2));
			transaction_commit();
		}
	}
	gui_style_pop(gui);

	pgui_row_empty(gui, hx);

	b32 checked2 = store->data.chk2;
	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Does Nothing");
	gui_style_push_ptr(gui, chk.hint, "Do Nothing Checkbox");
	pgui_chk(gui, checked2 ? "Checked" : "Unchecked", &checked2);

	gui_style_pop(gui);

	pgui_row_empty(gui, hx);

	/* sliders */
	r32 slider = store->data.slider;
	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Slider");
	gui_style_push_ptr(gui, slider.handle.hint, "Wow!");
	if (pgui_slider_x(gui, &slider)) {
		{
			transaction_begin_multi_frame();
			payload_primitive(slider, r32, payload);
			transaction_enqueue_mutation_primitive(&payload, store_offsetof(store_gui_t, slider));
			transaction_commit();
		}
	}
	gui_style_pop(gui);
	pgui_row_empty(gui, hx);

	r32 slider2 = store->data.slider2;
	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Slider2");
	gui_style_push_ptr(gui, slider.handle.hint, "Wow!");
	if (pgui_slider_x(gui, &slider2)) {
		{
			transaction_begin_multi_frame();
			payload_primitive(slider2, r32, payload);
			transaction_enqueue_mutation_primitive(&payload, store_offsetof(store_gui_t, slider2));
			transaction_commit();
		}
	}
	gui_style_pop(gui);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, 2 * row_height, cols_npt);
	pgui_txt(gui, "Text input");
	gui_style_push_b32(gui, npt.inactive.text.wrap, true);
	gui_style_push_b32(gui, npt.hot.text.wrap, true);
	gui_style_push_b32(gui, npt.active.text.wrap, true);
	gui_style_push_b32(gui, npt.disabled.text.wrap, true);
	pgui_npt_txt_transaction(gui, B2PS(store->data.npt), "Your text here...", 0);

	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);

	/* trees */
	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Tree");
	pgui_txt(gui, "Splits");
	if (gui->root_split) {
		static gui_tree_node_t nodes[16] = {0};
		pgui_tree_begin(gui, B2PC(nodes), 20, 20);
		draw_split_node(gui, gui->root_split);
		pgui_tree_end(gui);
	}

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Tree");
	pgui_txt(gui, "Test");
	{
		static gui_tree_node_t nodes[24] = {0};
		static int items[3][2][3] = {0};
		pgui_tree_begin(gui, B2PC(nodes), 20, 20);
		for (u32 i = 0; i < countof(items); ++i) {
			const b32 show_i = pgui_tree_node_begin(gui);
			pgui_txt(gui, imprint_u32(i));
			if (show_i) {
				for (u32 j = 0; j < countof(items[i]); ++j) {
					const b32 show_j = pgui_tree_node_begin(gui);
					pgui_txt(gui, imprint_u32(j));
					if (show_j) {
						for (u32 k = 0; k < countof(items[i][j]); ++k) {
							pgui_tree_leaf(gui);
							pgui_txt(gui, imprint_u32(k));
						}
						pgui_tree_node_end(gui);
					}
				}
				pgui_tree_node_end(gui);
			}
		}
		pgui_tree_end(gui);
	}
}

static
void draw_widget_panel(gui_t *gui)
{
	pgui_panel_grid_begin(gui, GUI_GRID_FLEX_VERTICAL);

	draw_widgets(gui, 20, 5);

	for (u32 i = 0; i < 10; ++i) {
		pgui_row_empty(gui, 5);
		pgui_row(gui, 20, 1);
		pgui_txt(gui, imprint_u32(i));
	}

	pgui_panel_grid_end(gui);
}

static
void draw_widgets_no_panel(gui_t *gui, box2i bbox)
{
	const v2i padding = { 10, 10 };
	const box2i bbox_padded = {
		.min = v2i_add(bbox.min, padding),
		.max = v2i_sub(bbox.max, padding),
	};
	const v2i p = bbox_padded.min;
	const v2i dim = box2i_get_extent(bbox_padded);
	gui_grid_t grid;
	r32 h, hx;

	pgui_grid_begin(gui, &grid, p.x, p.y + dim.y, dim.x, 0);
	h  = 20;
	hx = 5;

	draw_widgets(gui, h, hx);

	pgui_grid_end(gui, &grid);
}

static
const char *log_level_str(s32 level)
{
	switch (level) {
	case LOG_DEBUG:
		return "[DEBUG]";
	break;
	case LOG_INFO:
		return "[INFO ]";
	break;
	case LOG_WARNING:
		return "[WARN ]";
	break;
	case LOG_ERROR:
		return "[ERROR]";
	break;
	case LOG_FATAL:
		return "[FATAL]";
	break;
	default:
		return "[XXXXX]";
	break;
	}
}

static
void draw_log_panel(gui_t *gui, const struct log_data *data)
{
	const s32 s = gui_style(gui)->txt.size;
	const s32 h = s * 4 / 3;
	const s32 lw = gui_txt_width(gui, log_level_str(LOG_DEBUG), s) + 10;
  const s32 cols[] = { lw, 0 };
	u32 i, end;

	if (data->cnt == 0)
		return;

	pgui_panel_grid_begin(gui, GUI_GRID_FLEX_VERTICAL);

	i = data->start;
	end = (data->start + data->cnt) % LOG_LINE_COUNT;
	do {
		pgui_row_cellsv(gui, h, cols);
    pgui_txt(gui, log_level_str(data->level[i]));
		pgui_txt(gui, data->buf[i]);
		i = (i + 1) % LOG_LINE_COUNT;
	} while (i != end);

	pgui_panel_grid_end(gui);
}

static
void draw_about_panel(gui_t *gui)
{
	const s32 cols[] = { 0, 180, 0 };
	pgui_panel_grid_begin(gui, GUI_GRID_FLEX_VERTICAL);
	pgui_row_cellsv(gui, 20, cols);
	pgui_spacer(gui);
	pgui_txt(gui, "Violet IMGUI library\ndeveloped by Ryan Griege");
	pgui_spacer(gui);
	pgui_panel_grid_end(gui);
}

int main(int argc, char *const argv[])
{
	window_t *window;
	gui_t *gui;
	b32 quit = false;
	gui_split_t splits[7];
	gui_split_t *root_split, *menu_split, *main_split;
	gui_panel_t widget_panel, log_panel, about_panel;

	g_transaction_system = transaction_system_create(megastore__create(g_allocator), g_allocator);

	struct log_data log_data = { 0 };
	gui_panel_t *panels[PANEL_COUNT] = {
		&widget_panel,
		&log_panel,
		&about_panel,
	};
	u32 frame_time;

	vlt_init(VLT_THREAD_MAIN);

	log_add_stream(LOG_ALL, buffer_logger, &log_data);
	log_add_stream(LOG_ALL, file_logger, stdout);
	window = window_create(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME,
	                       WINDOW_BORDERLESS | WINDOW_RESIZABLE | WINDOW_CENTERED);
	log_remove_stream(file_logger, stdout);
	if (!window)
		goto err_window;

	gui = window->gui;
	mouse_press_debug(gui, true);

	gui_set_splits(gui, B2PC(splits));
	gui_create_root_split(gui, &root_split);
	gui_split2v(gui, root_split, &menu_split, MENU_HEIGHT, &main_split, 0);
	gui_split2h(gui, main_split, &widget_panel.split, 0.5f, &log_panel.split,
	            GUI_SPLIT_FULL);

	pgui_panel_init_in_split(gui, &widget_panel, PANEL_WIDGETS,
	                         widget_panel.split, "Widgets", GUI_PANEL_FULL);
	pgui_panel_init_in_split(gui, &log_panel, PANEL_LOG,
	                         log_panel.split, "Log", GUI_PANEL_FULL);
	pgui_panel_init_centered(gui, &about_panel, PANEL_ABOUT,
	                         200, 100, "About", GUI_PANEL_FULL);
	pgui_panel_add_tab(&widget_panel, &about_panel);

	while (!quit) {
		vlt_mem_advance_gen();
		if (!window_begin_frame(window))
			break;

		gui_style_push_r32(gui, panel.padding, 0);
		draw_menu_bar(gui, menu_split->box, &quit, panels);
		gui_style_pop(gui);

		for (u32 i = 0; i < countof(panels); ++i) {
			switch (panels[i]->id) {
			case PANEL_WIDGETS:
				gui_style_push_color(gui, panel.tab.inactive.bg_color, g_green);
				if (pgui_panel(gui, &widget_panel)) {
					draw_widget_panel(gui);
					pgui_panel_finish(gui, &widget_panel);
				}
				gui_style_pop(gui);
			break;
			case PANEL_LOG:
				if (pgui_panel(gui, &log_panel)) {
					draw_log_panel(gui, &log_data);
					pgui_panel_finish(gui, &log_panel);
				}
			break;
			case PANEL_ABOUT:
				gui_style_push_color(gui, panel.tab.inactive.bg_color, g_fuchsia);
				if (pgui_panel(gui, &about_panel)) {
					draw_about_panel(gui);
					pgui_panel_finish(gui, &about_panel);
				}
				gui_style_pop(gui);
			break;
			}
		}

		if (widget_panel.closed)
			draw_widgets_no_panel(gui, main_split->box);

		if (key_pressed(gui, KB_Q) && !gui_any_widget_has_focus(gui))
			quit = true;

		qsort(B2PC(panels), sizeof(panels[0]), pgui_panel_sortp);

		window_end_frame(window);
		frame_time = timepoint_diff_milli(gui_frame_start(gui), timepoint_create());
		if (frame_time < 1000.f / FPS_CAP)
			time_sleep_milli((u32)(1000.f / FPS_CAP) - frame_time);
	}

	log_add_stream(LOG_ALL, file_logger, stdout);

	window_destroy(window);
	log_remove_stream(file_logger, stdout);

err_window:
	transaction_system_destroy(&g_transaction_system);

	log_remove_stream(buffer_logger, &log_data);
	vlt_destroy(VLT_THREAD_MAIN);
	return 0;
}
