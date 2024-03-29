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

#define VSON_IMPLEMENTATION
#define LOCALIZE_IMPLEMENTATION

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

#include "violet/vson.h"
#include "violet/localize.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MENU_HEIGHT 20
#define APP_NAME "UI Sample"

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

struct widget_data
{
	b32 chk;
	u32 select;
	u32 mselect;
	u32 dropdown;
	r32 slider;
	color_t color;
	char npt[64];
};

struct log_data
{
	char buf[LOG_LINE_COUNT][LOG_LINE_SZ];
  s32 level[LOG_LINE_COUNT];
	u32 start, cnt;
};


static b32 g_widget_grid_flex = true;
static b32 g_translate = false;

void localization_table_overwrite_string(char *prev, const char *curr)
{
	const size_t prev_len = strlen(prev);
	const size_t curr_len = strlen(curr);
	strcpy(prev, curr);
	if (curr_len < prev_len)
		memset(&prev[curr_len], 0, prev_len - curr_len);
}

b32 localization_table_find_space(const localization_table_t *table,
                                  size_t size, u32 *index)
{
	const size_t end = LOCALIZE_MAX_STR_BYTES;
	size_t pos = end - 1;
	while (pos > 0 && table->chars[pos-1] == '\0')
		--pos;
	*index = (u32)pos;
	return end - pos >= size + 1;
}

void localization_table_compact(localization_table_t *table)
{
	char *chars = amalloc(LOCALIZE_MAX_STR_BYTES, g_temp_allocator);
	size_t end = 1; /* skip first byte so 'empty' slots point to empty string */

	memset(chars, 0, sizeof(LOCALIZE_MAX_STR_BYTES));

	for (u32 i = 0; i < countof(table->slots); ++i) {
		if (table->slots[i].id != 0) {
			const char *str = &table->chars[table->slots[i].index];
			strcpy(&chars[end], str);
			end += strlen(str) + 1;
		}
	}

	memcpy(table->chars, chars, LOCALIZE_MAX_STR_BYTES);

	afree(chars, g_temp_allocator);
}

b32 localization_table_save_translation(localization_table_t *table,
                                        const char *prev, const char *curr)
{
	const u32 id = hash_compute(prev);
	localized_string_t *slot;
	u32 index;

	if (!localization_table_find_slot(table, id, &slot) || slot->id == 0)
		return false;

	if (strlen(curr) <= strlen(prev)) {
		localization_table_overwrite_string(&table->chars[slot->index], curr);
		return true;
	}
	
	if (!localization_table_find_space(table, strlen(curr), &index)) {
		localization_table_compact(table);
		if (!localization_table_find_space(table, strlen(curr), &index))
			return false;
	}

	memset(&table->chars[slot->index], 0, strlen(prev));
	strcpy(&table->chars[index], curr);
	slot->index = index;
	return true;
}

void gui_npt_txt_translate(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	static char s_translation[64] = "";
	char buf[64];
	const gui_npt_flags_e flags = GUI_NPT_SELECT_ON_FOCUS | GUI_NPT_COMPLETE_ON_DEFOCUS;
	const u64 id = gui_widget_id(gui, x, y);
	const u32 n  = sizeof(s_translation);
	if (gui_widget_focused(gui, id)) {
		if (!gui_npt_txt(gui, x, y, w, h, s_translation, n, txt, flags))
			{} /* skip */
		else if (localization_table_save_translation(&g_localization_table, txt, s_translation))
			{} // (void)localize_save_language(fname);
		else
			log_error("failed to find string \"%s\"", txt);
	} else if (gui_any_widget_has_focus(gui)) {
		strbcpy(buf, txt);
		gui_npt_txt(gui, x, y, w, h, buf, n, txt, flags);
		if (gui_widget_focused(gui, id))
			strbcpy(s_translation, txt);
	} else {
		strbcpy(buf, txt);
		gui_npt_txt(gui, x, y, w, h, buf, n, txt, flags);
	}
}

void gui_txt_translate(gui_t *gui, s32 x, s32 y, s32 size, const char *txt, color_t c,
                       gui_align_e align)
{
	gui_style_push(gui, npt.inactive.text.color, c);
	gui_npt_txt_translate(gui, x, y, 100, 20, txt);
	gui_style_pop(gui);
}

void gui_txt_styled_translate(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                              const char *txt, const gui_text_style_t *style)
{
	gui_npt_txt_translate(gui, x, y, w, h, txt);
}

s32 gui_btn_txt_translate(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	gui_npt_txt_translate(gui, x, y, w, h, txt);
	return GUI_BTN_NONE;
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
	const s32 cols[] = { 40, 40, 40, 0, 20, 20, 20 };

	const color_t btn_inactive_bg_color = gui_style(gui)->btn.inactive.bg_color;
	s32 x, y, w, h;
	gui_grid_t grid;

	gui_style_push(gui, dropdown.btn.inactive.bg_color, gui_style(gui)->panel.bg_color);

	box2i_to_xywh(bbox, &x, &y, &w, &h);
	pgui_grid_begin(gui, &grid, x, y, w, h);
	pgui_row_cellsv(gui, 0, cols);

	if (pgui_menu_begin(gui, "Fil\u20ac", 80, 4)) {
		if (pgui_btn_txt(gui, "Open")) log_debug("Open");
		gui_style_push(gui, dropdown.btn.inactive.bg_color, btn_inactive_bg_color);
		if (pgui_menu_begin(gui, "Open Recent", 100, 3)) {
			if (pgui_btn_txt(gui, "La Nina")) log_debug("La Nina");
			if (pgui_btn_txt(gui, "La Pinta")) log_debug("La Pinta");
			if (pgui_btn_txt(gui, "La Santa Maria")) log_debug("La Santa Maria");
			pgui_menu_end(gui);
		}
    gui_style_pop(gui);
		if (pgui_btn_txt(gui, "Save")) log_debug("Save");
		if (pgui_btn_txt(gui, "Save as")) log_debug("Save as");
		pgui_menu_end(gui);
	}

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

	if (pgui_menu_begin(gui, "Test", 100, 2)) {
		pgui_chk(gui, "flex widget grid", &g_widget_grid_flex);
		pgui_chk(gui, "translation mode", &g_translate);
		pgui_menu_end(gui);
	}

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
void draw_widgets(gui_t *gui, r32 row_height, r32 hx, struct widget_data *data)
{
	const s32 cols[] = { 100, 0 };
	const s32 cols_npt[] = { 100, 80 };
	const s32 cols4[] = { 100, 0, 0, 0 };

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Button");
	gui_style_push_ptr(gui, btn.hint, "Button");
	pgui_btn_img(gui, window_get_img(gui_window(gui), "logo.png"), GUI_IMG_SCALED);
	gui_style_pop(gui);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Button");
	gui_style_push_ptr(gui, btn.hint, "Button");
	pgui_btn_txt(gui, LOCALIZE("Button Label"));
	gui_style_pop(gui);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Checkbox");
	gui_style_push_ptr(gui, chk.hint, "Checkbox");
	pgui_chk(gui, data->chk ? "Checked" : "Unchecked", &data->chk);
	gui_style_pop(gui);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols4);
	pgui_txt(gui, "Select");
	for (u32 i = 0; i < 3; ++i)
		pgui_select(gui, data->select == i ? "Selected" : "Unselected", &data->select, i);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Dropdown");
	if (pgui_dropdown_begin(gui, &data->dropdown, 5)) {
		for (u32 i = 0; i < 5; ++i)
			pgui_dropdown_item(gui, i, imprintf("Option %u", i + 1));
		pgui_dropdown_end(gui);
	}
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Slider");
	gui_style_push_ptr(gui, slider.handle.hint, "Wow!");
	pgui_slider_x(gui, &data->slider);
	gui_style_pop(gui);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, row_height, cols);
	pgui_txt(gui, "Color Picker");
	pgui_color_picker8(gui, 280, 200, &data->color);
	pgui_row_empty(gui, hx);

	pgui_row_cellsv(gui, 2 * row_height, cols_npt);
	pgui_txt(gui, "Text input");
	gui_style_push_b32(gui, npt.inactive.text.wrap, true);
	gui_style_push_b32(gui, npt.hot.text.wrap, true);
	gui_style_push_b32(gui, npt.active.text.wrap, true);
	gui_style_push_b32(gui, npt.disabled.text.wrap, true);
	pgui_npt_txt(gui, B2PS(data->npt), "Your text here...", 0);
	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);

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
void draw_widget_panel(gui_t *gui, struct widget_data *data)
{
	pgui_panel_grid_begin(gui, GUI_GRID_FLEX_VERTICAL);

	draw_widgets(gui, 20, 5, data);

	for (u32 i = 0; i < 10; ++i) {
		pgui_row_empty(gui, 5);
		pgui_row(gui, 20, 1);
		pgui_txt(gui, imprint_u32(i));
	}

	pgui_panel_grid_end(gui);
}

static
void draw_widgets_no_panel(gui_t *gui, box2i bbox, struct widget_data *data)
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

	if (g_widget_grid_flex) {
		pgui_grid_begin(gui, &grid, p.x, p.y + dim.y, dim.x, 0);
		h  = 20;
		hx = 5;
	} else {
		const s32 rows[] = { 20, 5, 20, 5, 20, 5, 20, 5, 20, 5, 20, 5, 100 };
		pgui_grid_begin(gui, &grid, p.x, p.y, dim.x, dim.y);
		pgui_col_cellsv(gui, 0, rows);
		h = 0;
		hx = 0;
	}

	draw_widgets(gui, h, hx, data);

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
	struct widget_data widget_data = { .color = gi_blue, .npt = "Why are you the way that you are?" };
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

	localize_set_language_default("en_US.vson");
	if (!localize_set_language("en_US.vson"))
		goto err_lang;

	gui_set_splits(gui, B2PC(splits));
	gui_create_root_split(gui, &root_split);
	gui_split2v(gui, root_split, &menu_split, MENU_HEIGHT, &main_split, 0);
#if 1
	gui_split2h(gui, main_split, &widget_panel.split, 0.5f, &log_panel.split,
	            GUI_SPLIT_FULL);

	pgui_panel_init_in_split(gui, &widget_panel, PANEL_WIDGETS,
	                         widget_panel.split, "Widgets", GUI_PANEL_FULL);
	pgui_panel_init_in_split(gui, &log_panel, PANEL_LOG,
	                         log_panel.split, "Log", GUI_PANEL_FULL);
	pgui_panel_init_centered(gui, &about_panel, PANEL_ABOUT,
	                         200, 100, "About", GUI_PANEL_FULL);
	pgui_panel_add_tab(&widget_panel, &about_panel);
#else
	pgui_panel_init_centered(gui, &widget_panel, 100, 100, "Widgets", GUI_PANEL_FULL);
	widget_panel.closed = true;
	pgui_panel_init_centered(gui, &log_panel, 100, 100, "Log", GUI_PANEL_FULL);
	log_panel.closed = true;
	pgui_panel_init_centered(gui, &about_panel, PANEL_ABOUT,
	                         200, 100, "About", GUI_PANEL_FULL);
	pgui_panel_close(gui, &about_panel);
#endif

	while (!quit) {
		vlt_mem_advance_gen();
		if (!window_begin_frame(window))
			break;

		gui_style_push_r32(gui, panel.padding, 0);
		draw_menu_bar(gui, menu_split->box, &quit, panels);
		gui_style_pop(gui);

#if 0
		if (g_translate) {
			gui_interface_t *cb = gui_interface(gui);
			cb->txt = gui_txt_translate;
			cb->txt_styled = gui_txt_styled_translate;
			cb->btn_txt = gui_btn_txt_translate;
		}
#endif

		for (u32 i = 0; i < countof(panels); ++i) {
			switch (panels[i]->id) {
			case PANEL_WIDGETS:
				gui_style_push_color(gui, panel.tab.inactive.bg_color, g_green);
				if (pgui_panel(gui, &widget_panel)) {
					draw_widget_panel(gui, &widget_data);
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

#if 0
		if (g_translate) {
			gui_interface_t *cb = gui_interface(gui);
			cb->txt = gui_txt;
			cb->txt_styled = gui_txt_styled;
			cb->btn_txt = gui_btn_txt;
		}
#endif

		if (widget_panel.closed) {
			draw_widgets_no_panel(gui, main_split->box, &widget_data);
			gui_circ(gui, 200, 200, 40, g_nocolor, g_red);
			const v2f poly[] = {
				{ 20, 20 },
				{ 20, 40 },
				{ 40, 40 },
				{ 40, 60 },
				{ 20, 60 },
				{ 20, 80 },
				{ 60, 80 },
				{ 60, 20 }
			};
			gui_polyf(gui, B2PC(poly), g_fuchsia, g_green);
		}

		if (key_pressed(gui, KB_Q) && !gui_any_widget_has_focus(gui))
			quit = true;

		qsort(B2PC(panels), sizeof(panels[0]), pgui_panel_sortp);

		window_end_frame(window);
		frame_time = timepoint_diff_milli(gui_frame_start(gui), timepoint_create());
		if (frame_time < 1000.f / FPS_CAP)
			time_sleep_milli((u32)(1000.f / FPS_CAP) - frame_time);
	}

	log_add_stream(LOG_ALL, file_logger, stdout);
err_lang:
	window_destroy(window);
	log_remove_stream(file_logger, stdout);
err_window:
	log_remove_stream(buffer_logger, &log_data);
	vlt_destroy(VLT_THREAD_MAIN);
	return 0;
}
