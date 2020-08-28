#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core.h"
#include "violet/color.h"
#include "violet/key.h"

/* Texture */

typedef enum gui_blend
{
	GUI_BLEND_NRM,
	GUI_BLEND_ADD,
	GUI_BLEND_MUL,
	GUI_BLEND_MAX = GUI_BLEND_MUL,
} gui_blend_e;

typedef struct gui_texture
{
	u32 handle;
	u32 width;
	u32 height;
	u32 blend;
} gui_texture_t;

m3f gui_texture_get_transform(const gui_texture_t *texture, s32 x, s32 y,
                              r32 sx, r32 sy, r32 rotation);

/* Image */

typedef struct gui_texture gui_img_t;

/* Font */

typedef enum gui_align
{
	GUI_ALIGN_LEFT      = 0x01,
	GUI_ALIGN_CENTER    = 0x02,
	GUI_ALIGN_RIGHT     = 0x04,
	GUI_ALIGN_TOP       = 0x10,
	GUI_ALIGN_MIDDLE    = 0x20,
	GUI_ALIGN_BOTTOM    = 0x40,

	GUI_ALIGN_TOPLEFT   = GUI_ALIGN_TOP    | GUI_ALIGN_LEFT,
	GUI_ALIGN_MIDLEFT   = GUI_ALIGN_MIDDLE | GUI_ALIGN_LEFT,
	GUI_ALIGN_BOTLEFT   = GUI_ALIGN_BOTTOM | GUI_ALIGN_LEFT,
	GUI_ALIGN_TOPCENTER = GUI_ALIGN_TOP    | GUI_ALIGN_CENTER,
	GUI_ALIGN_MIDCENTER = GUI_ALIGN_MIDDLE | GUI_ALIGN_CENTER,
	GUI_ALIGN_BOTCENTER = GUI_ALIGN_BOTTOM | GUI_ALIGN_CENTER,
	GUI_ALIGN_TOPRIGHT  = GUI_ALIGN_TOP    | GUI_ALIGN_RIGHT,
	GUI_ALIGN_MIDRIGHT  = GUI_ALIGN_MIDDLE | GUI_ALIGN_RIGHT,
	GUI_ALIGN_BOTRIGHT  = GUI_ALIGN_BOTTOM | GUI_ALIGN_RIGHT,
} gui_align_e;

#define GUI_ALIGN_VERTICAL   (GUI_ALIGN_TOP | GUI_ALIGN_MIDDLE | GUI_ALIGN_BOTTOM)
#define GUI_ALIGN_HORIZONTAL (GUI_ALIGN_LEFT | GUI_ALIGN_CENTER | GUI_ALIGN_RIGHT)

void gui_align_anchor(s32 x, s32 y, s32 w, s32 h, gui_align_e align, s32 *px, s32 *py);

typedef struct gui_font_metrics
{
	s32 ascent, descent, line_gap, newline_dist;
} gui_font_metrics_t;

typedef struct gui_char_quad
{
	gui_texture_t texture;
	r32 x0, y0, s0, t0;
	r32 x1, y1, s1, t1;
	r32 advance;
} gui_char_quad_t;

typedef void*(*gui_font_f)(void *handle, s32 size);
typedef b32(*gui_font_metrics_f)(void *font, gui_font_metrics_t *metrics);
typedef b32(*gui_char_quad_f)(void *font, s32 codepoint, r32 x, r32 y, gui_char_quad_t *quad);

typedef struct gui_fonts
{
	void *handle;
	gui_font_f get_font;
	gui_font_metrics_f get_metrics;
	gui_char_quad_f get_char_quad;
} gui_fonts_t;


/* General Gui */

typedef struct gui gui_t;

gui_t *gui_create(s32 w, s32 h, u32 texture_white, u32 texture_white_dotted,
                  gui_fonts_t fonts);
void   gui_destroy(gui_t *gui);
void   gui_dim(const gui_t *gui, s32 *x, s32 *y);
void   gui_begin_frame(gui_t *gui);
void   gui_end_frame(gui_t *gui);
void   gui_end_frame_ex(gui_t *gui, u32 target_frame_milli,
                        u32 idle_frame_milli, u32 idle_start_milli);

#ifndef GUI_CLIPBOARD_SIZE
#define GUI_CLIPBOARD_SIZE 64
#endif

b32   gui_text_input_active(const gui_t *gui);
b32   gui_has_clipboard_text(const gui_t *gui);
void  gui_get_clipboard_text(const gui_t *gui, char *text);
u32   gui_cursor(const gui_t *gui);
void *gui_window(gui_t *gui);
void  gui_set_window(gui_t *gui, void *window);

timepoint_t gui_frame_start(const gui_t *gui);
u32         gui_frame_time_milli(const gui_t *gui);
timepoint_t gui_last_input_time(const gui_t *gui);


typedef enum gui_mouse_button
{
	MB_LEFT      = 1 << 0,
	MB_MIDDLE    = 1 << 1,
	MB_RIGHT     = 1 << 2,
	MB_X1        = 1 << 3,
	MB_X2        = 1 << 4,
	MB_WHEELUP   = 1 << 5,
	MB_WHEELDOWN = 1 << 6,
	MB_WHEEL     = MB_WHEELUP | MB_WHEELDOWN,
} gui_mouse_button_e;

/* Input */

void gui_events_begin(gui_t *gui);
void gui_event_add_update(gui_t *gui);
void gui_event_set_mouse_pos(gui_t *gui, s32 x, s32 y);
void gui_event_set_mouse_btn(gui_t *gui, gui_mouse_button_e button);
void gui_event_add_text_input(gui_t *gui, const char *text);
void gui_event_set_keyboard(gui_t *gui, const u8 *keys, u32 key_cnt);
void gui_event_add_clipboard(gui_t *gui, const char *text);
void gui_event_add_window_leave(gui_t *gui);
void gui_event_set_window_dim(gui_t *gui, s32 w, s32 h);
void gui_events_end(gui_t *gui);

/* done after regular events since it uses a gui drag widget */
void gui_window_drag(gui_t *gui);
b32  gui_window_dragging(const gui_t *gui);
void gui_window_drag_end(gui_t *gui);

#ifndef GUI_MOUSE_DOUBLE_CLICK_INTERVAL
#define GUI_MOUSE_DOUBLE_CLICK_INTERVAL 500 /* windows default */
#endif

void mouse_pos(const gui_t *gui, s32 *x, s32 *y);
void mouse_pos_last(const gui_t *gui, s32 *x, s32 *y);
void mouse_pos_press(const gui_t *gui, s32 *x, s32 *y);
b32  mouse_pos_changed(const gui_t *gui);
void mouse_pos_delta(const gui_t *gui, s32 *x, s32 *y);
b32  mouse_pressed(const gui_t *gui, u32 mask);
b32  mouse_pressed_bg(const gui_t *gui, u32 mask);
b32  mouse_down(const gui_t *gui, u32 mask);
b32  mouse_down_bg(const gui_t *gui, u32 mask);
b32  mouse_released(const gui_t *gui, u32 mask);
b32  mouse_released_bg(const gui_t *gui, u32 mask);
b32  mouse_covered(const gui_t *gui);
void mouse_cover(gui_t *gui, u64 widget_id);
b32  mouse_over_bg(const gui_t *gui);
b32  mouse_double(const gui_t *gui, u32 mask);
b32  mouse_scroll(const gui_t *gui, s32 *dir);
b32  mouse_scroll_bg(const gui_t *gui, s32 *dir);
void mouse_press_debug(gui_t *gui, b32 enabled);

b32 key_down(const gui_t *gui, gui_key_t key);
b32 key_down_any(const gui_t *gui);
b32 key_pressed(const gui_t *gui, gui_key_t key);
b32 key_pressed_any(const gui_t *gui);
b32 key_released(const gui_t *gui, gui_key_t key);
b32 key_released_any(const gui_t *gui);
b32 key_mod(const gui_t *gui, gui_key_mod_t mod);
b32 key_toggled(const gui_t *gui, gui_key_toggle_t toggle);
const u8 *keyboard_state(const gui_t *gui);


/* Direct write access to vertex buffers */

typedef enum gui_draw_call_type
{
	GUI_DRAW_POINTS,
	GUI_DRAW_LINE_STRIP,
	GUI_DRAW_LINE_LOOP,
	GUI_DRAW_LINES,
	GUI_DRAW_TRIANGLE_STRIP,
	GUI_DRAW_TRIANGLE_FAN,
	GUI_DRAW_TRIANGLES,
	GUI_DRAW_QUAD_STRIP,
	GUI_DRAW_QUADS,
	GUI_DRAW_POLYGON,
	GUI_DRAW_COUNT
} gui_draw_call_type_e;

b32  gui_begin(gui_t *gui, u32 num_verts, gui_draw_call_type_e type);
b32  gui_begin_tex(gui_t *gui, u32 num_verts, gui_draw_call_type_e type,
                   u32 tex, gui_blend_e blend);
void gui_vertf(gui_t *gui, r32 x, r32 y, color_t c, r32 u, r32 v);
void gui_end(gui_t *gui);

/* Primitives */

typedef enum gui_img_scale
{
	GUI_IMG_SCALED,
	GUI_IMG_CENTERED,
} gui_img_scale_e;

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c);
void gui_linef(gui_t *gui, r32 x0, r32 y0, r32 x1, r32 y1, r32 w, color_t c);
void gui_tri(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
             color_t fill, color_t stroke);
void gui_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t fill, color_t stroke);
void gui_rect_mcolor(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     color_t bl, color_t br, color_t tr, color_t tl);
void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t stroke);
void gui_arc(gui_t *gui, s32 x, s32 y, s32 r, r32 angle_start, r32 angle_end,
             color_t fill, color_t stroke);
void gui_trisf(gui_t *gui, const v2f *v, u32 n, color_t fill);
void gui_poly(gui_t *gui, const v2i *v, u32 n, color_t fill, color_t stroke);
void gui_polyf(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t stroke);
void gui_polyline(gui_t *gui, const v2i *v, u32 n, color_t stroke);
void gui_polylinef(gui_t *gui, const v2f *v, u32 n, r32 w, color_t stroke);
void gui_img(gui_t *gui, s32 x, s32 y, const gui_img_t *img);
void gui_img_ex(gui_t *gui, s32 x, s32 y, const gui_img_t *img, r32 sx, r32 sy,
                r32 rotation, r32 opacity);
void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const gui_img_t *img,
                   gui_img_scale_e scale);
void gui_txt(gui_t *gui, s32 x, s32 y, s32 size, const char *txt, color_t c,
             gui_align_e align);
void gui_txt_dim(const gui_t *gui, s32 x, s32 y, s32 size, const char *txt,
                 gui_align_e align, s32 *px, s32 *py, s32 *pw, s32 *ph);
s32  gui_txt_width(const gui_t *gui, const char *txt, s32 size);

void gui_mask_push(gui_t *gui, s32 x, s32 y, s32 w, s32 h);
void gui_mask_pop(gui_t *gui);


typedef struct gui_line_style gui_line_style_t;
typedef struct gui_text_style gui_text_style_t;
typedef struct gui_element_style gui_element_style_t;

void gui_line_styled(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                     const gui_line_style_t *style);
void gui_txt_styled(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, const gui_text_style_t *style);
u32  gui_wrap_txt(const gui_t *gui, char *txt, s32 padding, s32 size, r32 max_width);

typedef void(*gui_pen_t)(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         const gui_element_style_t *style);

void gui_pen_null(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const gui_element_style_t *style);
void gui_pen_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const gui_element_style_t *style);
void gui_pen_circ(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const gui_element_style_t *style);


b32  gui_point_visible(const gui_t *gui, s32 x, s32 y);
b32  gui_box_visible(const gui_t *gui, s32 x, s32 y, s32 w, s32 h);



/* Render buffer */

typedef struct gui_draw_call
{
	u32 idx;
	u32 cnt;
	u32 type;
	u32 tex;
	u32 blend;
} gui_draw_call_t;

typedef struct gui_layer
{
	u32 draw_call_idx, draw_call_cnt;
	s32 x, y, w, h;
	s32 pri;
} gui_layer_t;

typedef enum gui_cursor
{
	GUI_CURSOR_DEFAULT,
	GUI_CURSOR_RESIZE_NS,
	GUI_CURSOR_RESIZE_EW,
	GUI_CURSOR_TEXT_INPUT,
	GUI_CURSOR_BUTTON,
	GUI_CURSOR_COUNT
} gui_cursor_e;

typedef struct gui_render_output
{
	u32 num_layers;
	const gui_layer_t *layers;
	u32 num_verts;
	struct {
		const v2f *pos;
		const color_t *color;
		const v2f *tex_coord;
	} verts;
	u32 num_draw_calls;
	const gui_draw_call_t *draw_calls;
} gui_render_output_t;

u32  gui_num_layers(const gui_t *gui);
u32  gui_vert_cnt(const gui_t *gui);
u32  gui_draw_call_cnt(const gui_t *gui);
u32  gui_culled_vert_cnt(const gui_t *gui);
u32  gui_culled_draw_call_cnt(const gui_t *gui);
u32  gui_culled_widget_cnt(const gui_t *gui);
void gui_get_render_output(const gui_t *gui, gui_render_output_t *output);



/* Widgets */

/* Limit for input boxes & dropdowns */
#ifndef GUI_TXT_MAX_LENGTH
#define GUI_TXT_MAX_LENGTH 128
#endif

#ifndef GUI_POPUP_STACK_SIZE
#define GUI_POPUP_STACK_SIZE 2
#endif

#define GUI_FOCUS_STACK_SIZE (GUI_POPUP_STACK_SIZE + 1)

#ifndef GUI_HINT_TIMER
#define GUI_HINT_TIMER 1000
#endif

typedef enum gui_npt_flags
{
	GUI_NPT_PASSWORD              = 1 << 0,
	GUI_NPT_SELECT_ON_FOCUS       = 1 << 1,
	GUI_NPT_COMPLETE_ON_ENTER     = 1 << 2, /* always enabled */
	GUI_NPT_COMPLETE_ON_TAB       = 1 << 3,
	GUI_NPT_COMPLETE_ON_CLICK_OUT = 1 << 4,
	GUI_NPT_COMPLETE_ON_ESCAPE    = 1 << 5,
	GUI_NPT_COMPLETE_ON_UNCHANGED = 1 << 6,
	GUI_NPT_COMPLETE_ON_DEFOCUS   = GUI_NPT_COMPLETE_ON_TAB
	                              | GUI_NPT_COMPLETE_ON_CLICK_OUT
	                              | GUI_NPT_COMPLETE_ON_ESCAPE
	                              | GUI_NPT_COMPLETE_ON_UNCHANGED,
} gui_npt_flags_e;

typedef struct gui_npt_filter
{
	b8 ascii[127];
	b8 non_ascii;
} gui_npt_filter_t;
typedef const gui_npt_filter_t* gui_npt_filter_p;

typedef enum gui_btn_val
{
	GUI_BTN_NONE,
	GUI_BTN_PRESS,
	GUI_BTN_HOLD,
} gui_btn_e;

extern const gui_npt_filter_t g_gui_npt_filter_print;
extern const gui_npt_filter_t g_gui_npt_filter_numeric;
extern const gui_npt_filter_t g_gui_npt_filter_hex;

b32 gui_npt_filter(gui_npt_filter_p filter, s32 codepoint);
const char *gui_npt_val_buf(const gui_t *gui);
char       *gui_npt_val_buf_mut(gui_t *gui);

typedef struct gui_widget_bounds
{
	box2i bbox;
	box2i children;
	struct gui_widget_bounds *prev;
} gui_widget_bounds_t;

void gui_widget_bounds_push(gui_t *gui, gui_widget_bounds_t *bounds, b32 propogate);
void gui_widget_bounds_pop(gui_t *gui, gui_widget_bounds_t *bounds, b32 propogate);
void gui_widget_bounds_extend(gui_t *gui, s32 x, s32 y, s32 w, s32 h);

typedef struct gui_scroll_area
{
	v2i pos;
	v2i dim;
	v2i scroll;
	gui_widget_bounds_t widget_bounds;
	box2i last_children_bbox;
	struct gui_scroll_area *prev;
} gui_scroll_area_t;

/* returns GUI_NPT_COMPLETE_ON_XXX if completed using an enabled completion method
 * or 0 otherwise */
s32  gui_npt_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                 const char *hint, gui_npt_flags_e flags);
s32  gui_npt_txt_ex(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                    const char *hint, gui_npt_flags_e flags, gui_npt_filter_p filter);
s32  gui_npt_val(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, u32 n,
                 gui_npt_flags_e flags, gui_npt_filter_p filter);
s32  gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt);
s32  gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const gui_img_t *img,
                 gui_img_scale_e scale);
s32  gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen);
s32  gui_btn_color(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t color);
b32  gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, b32 *val);
b32  gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen, b32 *val);
b32  gui_slider_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
b32  gui_slider_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
b32  gui_range_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                 r32 min, r32 max);
b32  gui_range_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                 r32 min, r32 max);
b32  gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                const char *txt, u32 *val, u32 opt);
b32  gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                 const char *txt, u32 *val, u32 opt);
b32  gui_dropdown_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        u32 *val, u32 num_items);
b32  gui_dropdown_item(gui_t *gui, u32 id, const char *txt);
void gui_dropdown_end(gui_t *gui);
typedef void(*gui_drag_f)(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                          s32 offset_x, s32 offset_y, void *udata);
void gui_drag_func_default(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                           s32 offset_x, s32 offset_y, void *udata);
void gui_drag_func_horizontal(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                              s32 offset_x, s32 offset_y, void *udata);
void gui_drag_func_vertical(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                            s32 offset_x, s32 offset_y, void *udata);
b32  gui_drag(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, gui_mouse_button_e mb);
b32  gui_dragf(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, gui_mouse_button_e mb,
               gui_drag_f cb, void *udata);
b32  gui_drag_rect(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, gui_mouse_button_e mb);
b32  gui_drag_rectf(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, gui_mouse_button_e mb,
                    gui_drag_f cb, void *udata);
b32  gui_drag_circ(gui_t *gui, s32 *x, s32 *y, s32 r, gui_mouse_button_e mb);
b32  gui_drag_circf(gui_t *gui, s32 *x, s32 *y, s32 r, gui_mouse_button_e mb,
                    gui_drag_f cb, void *udata);
b32  gui_menu_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, s32 item_w, u32 num_items);
void gui_menu_end(gui_t *gui);
b32  gui_color_picker_sv(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c);
b32  gui_color_picker_h(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c);
b32  gui_color_picker_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            s32 pw, s32 ph, colorf_t c);
b32  gui_color_picker_popup_default(gui_t *gui, colorf_t *c);
void gui_color_picker_end(gui_t *gui);
b32  gui_color_picker8_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             s32 pw, s32 ph, color_t c);
b32  gui_color_picker8_popup_default(gui_t *gui, color_t *c);
void gui_color_picker8_end(gui_t *gui);
b32  gui_color_picker(gui_t *gui, s32 s, s32 y, s32 w, s32 h,
                      s32 pw, s32 ph, colorf_t *c);
b32  gui_color_picker8(gui_t *gui, s32 s, s32 y, s32 w, s32 h,
                       s32 pw, s32 ph, color_t *c);
void gui_scroll_area_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           gui_scroll_area_t *scroll_area);
void gui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area);

u64  gui_widget_id(const gui_t *gui, s32 x, s32 y);
void gui_widget_focus_next(gui_t *gui);
void gui_widget_deactivate(gui_t *gui, u64 id);
b32  gui_widget_hot(const gui_t *gui, u64 id);
b32  gui_widget_active(const gui_t *gui, u64 id);
b32  gui_widget_focused(const gui_t *gui, u64 id);
b32  gui_any_widget_hot(const gui_t *gui);
b32  gui_any_widget_active(const gui_t *gui);
b32  gui_any_widget_has_focus(const gui_t *gui);
b32  gui_any_widget_had_focus(const gui_t *gui);
b32  gui_locked(const gui_t *gui);
void gui_lock(gui_t *gui);
void gui_unlock(gui_t *gui);
/* NOTE: I usually hate 'conditional' methods, but this cleans up usage code */
void gui_lock_if(gui_t *gui, b32 cond, u32 *lock);
void gui_unlock_if(gui_t *gui, u32 lock);
/* Scales splits, panels, and panel grids.  A value of 100 means unscaled.
 * Be careful calling this between frame_begin & frame_end. */
void gui_set_scale(gui_t *gui, s32 scale);
s32  gui_scale_val(const gui_t *gui, s32 val);
s32  gui_scale_val_inverse(const gui_t *gui, s32 val);


/* Grid layout */

/* equally distribute remaining space */
#define GUI_GRID_FLEX 0
/* consume percentage (usually out of 100 - see below) of remaining space */
#define GUI_GRID_PCNT(x) (-x)

#ifndef GUI_GRID_MAX_DEPTH
#define GUI_GRID_MAX_DEPTH 8
#endif

#ifndef GUI_GRID_MAX_CELLS
#define GUI_GRID_MAX_CELLS 128
#endif

typedef enum gui_grid_flex
{
	GUI_GRID_FLEX_NONE       = 0x0,
	GUI_GRID_FLEX_HORIZONTAL = 0x1,
	GUI_GRID_FLEX_VERTICAL   = 0x2,
	GUI_GRID_FLEX_FULL       = 0x3,
} gui_grid_flex_e;

typedef struct gui_grid_strip
{
	b32 vertical;
	s32 *current_cell;
	s32 *max_cell;
	u32 num_cells;
	v2i dim;
} gui_grid_strip_t;

typedef struct gui_grid
{
	s32 cells[GUI_GRID_MAX_CELLS];
	gui_grid_strip_t strips[GUI_GRID_MAX_DEPTH];
	u32 depth;
	v2i pos; /* top left */
	v2i dim;
	v2i start; /* top left */
	gui_widget_bounds_t widget_bounds;
	struct gui_grid *prev;
} gui_grid_t;

void pgui_grid_begin(gui_t *gui, gui_grid_t *grid, s32 x, s32 y, s32 w, s32 h);
void pgui_grid_end(gui_t *gui, gui_grid_t *grid);
u32  pgui_grid_depth(const gui_t *gui);
void pgui_grid_dimensions(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h);

/* Cell values are evaluated roughly as follows:
 * 1. Subtract pixel values from the total row/column dimension.
 * 2. Evaluate GUI_GRID_PCNT values on the remaining dimension.
 * 3. Split the remaining dimension evenly amongst GUI_GRID_FLEX cells.
 *
 * Most of the time, it is not advisable to mix GUI_GRID_PCNT and GUI_GRID_FLEX values,
 * and GUI_GRID_PCNT values should total 100 when used.  However, the following rules
 * apply in the cases where these aren't observed:
 *
 * If the GUI_GRID_PCNT's total 100, any GUI_GRID_FLEX cells will have 0 dimension.
 *
 * If the GUI_GRID_PCNT's exceed 100, then they will be used as relative weights
 * instead of actual percentages, and any GUI_GRID_FLEX cells will have 0 dimension.
 *
 * If the GUI_GRID_PCNT's are less than 100 and any GUI_GRID_FLEX cells are present,
 * then GUI_GRID_PCNT's will be used as percentages, and the remaining percentage
 * will be distributed evenly amongst GUI_GRID_FLEX cells.
 *
 * If the GUI_GRID_PCNT's are less than 100 and no GUI_GRID_FLEX cells are present,
 * then the GUI_GRID_PCNT's will be used as relative weights instead of percentages. */
void pgui_row(gui_t *gui, s32 height, u32 num_cells);
void pgui_row_cells(gui_t *gui, s32 height, const s32 *cells, u32 num_cells);
#define pgui_row_cellsv(gui, height, cells) \
	pgui_row_cells(gui, height, cells, countof(cells))
void pgui_row_empty(gui_t *gui, s32 height);
void pgui_row_centered(gui_t *gui, s32 height, s32 width);

void pgui_col(gui_t *gui, s32 width, u32 num_cells);
void pgui_col_cells(gui_t *gui, s32 width, const s32 *cells, u32 num_cells);
#define pgui_col_cellsv(gui, width, cells) \
	pgui_col_cells(gui, width, cells, countof(cells))
void pgui_col_empty(gui_t *gui, s32 width);
void pgui_col_centered(gui_t *gui, s32 width, s32 height);

void pgui_cell(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h);
void pgui_cell_consume(gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h);
void pgui_cell_merge_next(gui_t *gui);
u64  pgui_next_widget_id(const gui_t *gui);

void pgui_spacer(gui_t *gui);
void pgui_spacer_blank(gui_t *gui);
void pgui_txt(gui_t *gui, const char *str);
void pgui_img(gui_t *gui, const gui_img_t *img, gui_img_scale_e scale);
s32  pgui_btn_txt(gui_t *gui, const char *lbl);
s32  pgui_btn_img(gui_t *gui, const gui_img_t *img, gui_img_scale_e scale);
s32  pgui_btn_pen(gui_t *gui, gui_pen_t pen);
s32  pgui_btn_color(gui_t *gui, color_t color);
b32  pgui_chk(gui_t *gui, const char *lbl, b32 *val);
b32  pgui_chk_pen(gui_t *gui, gui_pen_t pen, b32 *val);
s32  pgui_npt_txt(gui_t *gui, char *lbl, u32 n, const char *hint,
                  gui_npt_flags_e flags);
s32  pgui_npt_txt_ex(gui_t *gui, char *lbl, u32 n, const char *hint,
                     gui_npt_flags_e flags, gui_npt_filter_p filter);
s32  pgui_npt_val(gui_t *gui, const char *txt, u32 n,
                  gui_npt_flags_e flags, gui_npt_filter_p filter);
b32  pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt);
b32  pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt);
b32  pgui_dropdown_begin(gui_t *gui, u32 *val, u32 num_items);
b32  pgui_dropdown_item(gui_t *gui, u32 id, const char *txt);
void pgui_dropdown_end(gui_t *gui);
b32  pgui_slider_x(gui_t *gui, r32 *val);
b32  pgui_slider_y(gui_t *gui, r32 *val);
b32  pgui_range_x(gui_t *gui, r32 *val, r32 min, r32 max);
b32  pgui_range_y(gui_t *gui, r32 *val, r32 min, r32 max);
b32  pgui_menu_begin(gui_t *gui, const char *txt, s32 item_w, u32 num_items);
void pgui_menu_end(gui_t *gui);
b32  pgui_color_picker_sv(gui_t *gui, colorf_t *color);
b32  pgui_color_picker_h(gui_t *gui, colorf_t *color);
b32  pgui_color_picker(gui_t *gui, s32 pw, s32 ph, colorf_t *c);
b32  pgui_color_picker8(gui_t *gui, s32 pw, s32 ph, color_t *c);
void pgui_scroll_area_begin(gui_t *gui, gui_scroll_area_t *scroll_area);
void pgui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area);


typedef struct gui_tree_node
{
	u8 depth;
	b8 expanded;
} gui_tree_node_t;

void pgui_tree_begin(gui_t *gui, gui_tree_node_t *nodes, u32 max_nodes,
                     s32 row_height, s32 indent);
void pgui_tree_end(gui_t *gui);
b32  pgui_tree_node_begin(gui_t *gui);
void pgui_tree_node_end(gui_t *gui);
void pgui_tree_leaf(gui_t *gui);
void gui_pen_tree_collapse(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           const gui_element_style_t *style);
void gui_pen_tree_restore(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                          const gui_element_style_t *style);


/* Splits */

#ifndef GUI_SPLIT_RESIZE_BORDER
#define GUI_SPLIT_RESIZE_BORDER 4
#endif

#ifndef GUI_SPLIT_ALIGN_LAST_TO_END
#define GUI_SPLIT_ALIGN_LAST_TO_END 1
#endif

typedef enum gui_split_flags
{
	GUI_SPLIT_RESIZABLE = 0x1,
	GUI_SPLIT_TEMPORARY = 0x2,
	GUI_SPLIT_DIVISIBLE = 0x4,
	GUI_SPLIT_FULL      = 0x7,
} gui_split_flags_e;

typedef struct gui_split
{
	b32 in_use;
	s32 flags;
	struct gui_split *parent; /* un-owned, only NULL for root */
	struct {
		struct gui_split *prev; /* un-owned, can be NULL */
		struct gui_split *next; /* un-owned, can be NULL */
	} siblings;
	struct {
		b32 vertical;
		struct gui_split *first; /* un-owned, can be NULL */
		struct gui_split *last;  /* un-owned, can be NULL */
	} children;
	r32 size;
	box2i box;
	struct gui_panel *panel; /* un-owned, can be NULL */
} gui_split_t;

void gui_set_splits(gui_t *gui, gui_split_t splits[], u32 num_splits);
b32  gui_create_root_split(gui_t *gui, gui_split_t **root);
b32  gui_split2h(gui_t *gui, gui_split_t *parent, gui_split_t **left, r32 size,
                 gui_split_t **right, gui_split_flags_e flags);
b32  gui_split2v(gui_t *gui, gui_split_t *parent, gui_split_t **top, r32 size,
                 gui_split_t **bottom, gui_split_flags_e flags);
b32  gui_splitNh(gui_t *gui, gui_split_t *parent, gui_split_t **child,
                 r32 size, gui_split_flags_e flags);
b32  gui_splitNv(gui_t *gui, gui_split_t *parent, gui_split_t **child,
                 r32 size, gui_split_flags_e flags);
void gui_splits_compute(gui_t *gui);
void gui_splits_render(gui_t *gui);


/* Panels */

#ifndef GUI_SCROLL_RATE
#define GUI_SCROLL_RATE 20
#endif

#ifndef GUI_PANEL_RESIZE_BORDER
#define GUI_PANEL_RESIZE_BORDER 4
#endif

#ifndef GUI_PANEL_TITLEBAR_HEIGHT
#define GUI_PANEL_TITLEBAR_HEIGHT 20
#endif

#ifndef GUI_PANEL_MAX_TAB_WIDTH
#define GUI_PANEL_MAX_TAB_WIDTH 100
#endif

#ifndef GUI_PANEL_MIN_DIM
#define GUI_PANEL_MIN_DIM 80
#endif

typedef enum gui_panel_flags
{
	GUI_PANEL_TITLEBAR    = 0x01,
	GUI_PANEL_DRAGGABLE   = 0x02,
	GUI_PANEL_RESIZABLE   = 0x04,
	GUI_PANEL_CLOSABLE    = 0x08,
	GUI_PANEL_COLLAPSABLE = 0x10,
	GUI_PANEL_SCROLLABLE  = 0x20,
	GUI_PANEL_DOCKABLE    = 0x40,
	GUI_PANEL_FULL        = 0x7f,
} gui_panel_flags_e;

typedef struct gui_panel
{
	struct { s32 x, y, w, h; } unscaled;
	struct { s32 x, y, w, h; };
	const char *title; /* un-owned, can be NULL */
	s32 flags;
	u32 id;
	intptr userdata;
	gui_scroll_area_t scroll_area;
	s32 pri;
	struct gui_panel *prev, *next; /* un-owned, can be NULL */
	gui_split_t *split; /* un-owned, can be NULL */
	b32 closed;
	b32 collapsed;
	b32 tabbed_out;
	s32 scale_anchor;
} gui_panel_t;

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, u32 id,
                     s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_e flags);
void pgui_panel_init_centered(gui_t *gui, gui_panel_t *panel, u32 id,
                              s32 w, s32 h,
                              const char *title, gui_panel_flags_e flags);
void pgui_panel_init_in_split(gui_t *gui, gui_panel_t *panel, u32 id,
                              gui_split_t *split,
                              const char *title, gui_panel_flags_e flags);
void pgui_panel_add_tab(gui_panel_t *panel, gui_panel_t *tab);
void pgui_panel_select_tab(gui_panel_t *panel);
b32  pgui_panel(gui_t *gui, gui_panel_t *panel);
void pgui_panel_collapse(gui_panel_t *panel);
void pgui_panel_restore(gui_panel_t *panel);
void pgui_panel_open(gui_t *gui, gui_panel_t *panel);
void pgui_panel_close(gui_t *gui, gui_panel_t *panel);
void pgui_panel_toggle(gui_t *gui, gui_panel_t *panel);
void pgui_panel_finish(gui_t *gui, gui_panel_t *panel);
b32  pgui_panel_content_visible(const gui_panel_t *panel);
s32  pgui_panel_body_height(const gui_t *gui, const gui_panel_t *panel);
void pgui_panel_to_front(gui_t *gui, gui_panel_t *panel);
int  pgui_panel_sort(const void *lhs, const void *rhs);
int  pgui_panel_sortp(const void *lhs, const void *rhs);
void pgui_panel_grid_begin(gui_t *gui, gui_grid_flex_e flex);
void pgui_panel_grid_end(gui_t *gui);

void gui_pen_window_minimize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             const gui_element_style_t *style);
void gui_pen_window_maximize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             const gui_element_style_t *style);
void gui_pen_window_close(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                          const gui_element_style_t *style);
void gui_pen_panel_drag(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        const gui_element_style_t *style);
void gui_pen_panel_collapse(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            const gui_element_style_t *style);
void gui_pen_panel_restore(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           const gui_element_style_t *style);
void gui_pen_panel_close(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         const gui_element_style_t *style);

void pgui_scroll_area_grid_begin(gui_t *gui, gui_grid_t *grid, gui_grid_flex_e flex);
void pgui_scroll_area_grid_end(gui_t *gui, gui_grid_t *grid);

/* Style */

typedef struct gui_line_style
{
	r32 thickness;
	r32 dash_len;
	color_t color;
} gui_line_style_t;

typedef struct gui_text_style
{
	s32 size;
	color_t color;
	s32 align;
	s32 padding;
	b32 wrap;
} gui_text_style_t;

typedef struct gui_element_style
{
	gui_line_style_t line;
	gui_text_style_t text;
	color_t bg_color;
	color_t outline_color;
} gui_element_style_t;

typedef struct gui_widget_style
{
	gui_pen_t pen;
	gui_element_style_t inactive;
	gui_element_style_t hot;
	gui_element_style_t active;
	gui_element_style_t disabled; /* only colors are used */
	const char *hint;
} gui_widget_style_t;

typedef struct gui_slider_style
{
	gui_widget_style_t handle;
	gui_widget_style_t track;
	b32 track_narrow;
} gui_slider_style_t;

typedef struct gui_padding_style
{
	s32 top;
	s32 right;
	s32 bottom;
	s32 left;
} gui_padding_style_t;

typedef struct gui_scroll_area_style
{
	color_t bg_color;
	gui_padding_style_t padding;
	s32 scrollbar_track_width;
	gui_slider_style_t scrollbar;
} gui_scroll_area_style_t;

typedef struct gui_dropdown_style
{
	gui_widget_style_t btn;
	struct {
		color_t color;
		s32 width;
	} shadow;
} gui_dropdown_style_t;

typedef struct gui_color_picker_style
{
	gui_widget_style_t btn;
	struct {
		color_t color;
		s32 width;
	} shadow;
} gui_color_picker_style_t;

typedef struct gui_panel_style
{
	color_t bg_color;
	color_t border_color;
	gui_element_style_t titlebar;
	gui_widget_style_t  drag;
	gui_widget_style_t  tab;
	gui_widget_style_t  close;
	gui_widget_style_t  collapse;
	gui_widget_style_t  restore;
	gui_slider_style_t  scrollbar;
	color_t cell_bg_color;
	color_t cell_border_color;
	gui_padding_style_t padding;
} gui_panel_style_t;

typedef struct gui_style
{
	color_t                  bg_color;
	gui_line_style_t         line;
	gui_text_style_t         txt;
	gui_widget_style_t       npt;
	gui_widget_style_t       btn;
	gui_widget_style_t       chk;
	gui_slider_style_t       slider;
	gui_widget_style_t       select;
	gui_dropdown_style_t     dropdown;
	gui_color_picker_style_t color_picker;
	gui_widget_style_t       drag;
	gui_element_style_t      hint;
	gui_scroll_area_style_t  scroll_area;
	gui_widget_style_t       tree;
	gui_panel_style_t        panel;
	gui_line_style_t         split;
} gui_style_t;

extern const gui_style_t g_gui_style_default;
extern const gui_style_t g_gui_style_invis;

gui_style_t *gui_style(gui_t *gui);
const gui_style_t *gui_style_c(const gui_t *gui);
void         gui_style_set(gui_t *gui, const gui_style_t *style);

void gui_style_push_(gui_t *gui, const void *value, size_t offset, size_t size);
void gui_style_push_current_(gui_t *gui, size_t offset, size_t size);
void gui_style_push_color_(gui_t *gui, size_t offset, color_t val);
void gui_style_push_b32_(gui_t *gui, size_t offset, b32 val);
void gui_style_push_s32_(gui_t *gui, size_t offset, s32 val);
void gui_style_push_r32_(gui_t *gui, size_t offset, r32 val);
void gui_style_push_pen_(gui_t *gui, size_t offset, gui_pen_t pen);
void gui_style_push_ptr_(gui_t *gui, size_t offset, const void *ptr);
void gui_style_pop(gui_t *gui);

#define gui_style_push(gui, loc, val) \
	gui_style_push_(gui, &(val), offsetof(gui_style_t, loc), sizeof(val))
#define gui_style_push_current(gui, loc) \
	gui_style_push_current_(gui, offsetof(gui_style_t, loc), sizeof(gui_style(gui)->loc))
#define gui_style_push_color(gui, loc, val) \
	gui_style_push_color_(gui, offsetof(gui_style_t, loc), val)
#define gui_style_push_b32(gui, loc, val) \
	gui_style_push_b32_(gui, offsetof(gui_style_t, loc), val)
#define gui_style_push_s32(gui, loc, val) \
	gui_style_push_s32_(gui, offsetof(gui_style_t, loc), val)
#define gui_style_push_r32(gui, loc, val) \
	gui_style_push_r32_(gui, offsetof(gui_style_t, loc), val)
#define gui_style_push_pen(gui, loc, val) \
	gui_style_push_pen_(gui, offsetof(gui_style_t, loc), val)
#define gui_style_push_ptr(gui, loc, val) \
	gui_style_push_ptr_(gui, offsetof(gui_style_t, loc), val)

#ifndef GUI_STYLE_STACK_LIMIT
#define GUI_STYLE_STACK_LIMIT 2048
#endif


#endif // VIOLET_GUI_H


/* Implementation */

#ifdef GUI_IMPLEMENTATION

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "violet/array.h"
#include "violet/fmath.h"
#include "violet/geom.h"
#include "violet/imath.h"
#include "violet/string.h"
#include "violet/utf8.h"

static
u32 gui__popcount(u32 x)
{
	u32 c = x;
	c =  c               - ((c >> 1)  & 0x55555555);
	c = (c & 0x33333333) + ((c >> 2)  & 0x33333333);
	c = (c               +  (c >> 4)) & 0x0f0f0f0f;
	c = (c * 0x01010101) >> 24;
	return c;
}

/* Font */

void gui_align_anchor(s32 x, s32 y, s32 w, s32 h, gui_align_e align, s32 *px, s32 *py)
{
	if (align & GUI_ALIGN_CENTER)
		*px = x + w / 2;
	else if (align & GUI_ALIGN_RIGHT)
		*px = x + w;
	else
		*px = x; /* default to GUI_ALIGN_LEFT */
	if (align & GUI_ALIGN_MIDDLE)
		*py = y + h / 2;
	else if (align & GUI_ALIGN_TOP)
		*py = y + h;
	else
		*py = y; /* default to GUI_ALIGN_BOTTOM */
}

static
s32 font__line_width(void *font, const char *txt, gui_char_quad_f get_char_quad)
{
	const char *p = txt;
	char *pnext;
	s32 cp;
	gui_char_quad_t q;
	r32 width = 0;

	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		p = pnext;
		if (cp == '\n')
			goto out;
		else if (get_char_quad(font, cp, width, 0, &q))
			width += q.advance;
	}
out:
	return width;
}

static
s32 font__line_offset_x(void *font, const char *txt,
                        s32 align, s32 padding,
                        gui_char_quad_f get_char_quad)
{
	if (align & GUI_ALIGN_CENTER)
		return -font__line_width(font, txt, get_char_quad) / 2;
	else if (align & GUI_ALIGN_RIGHT)
		return -(font__line_width(font, txt, get_char_quad) + padding);
	else /* default to GUI_ALIGN_LEFT */
		return padding;
}

static
s32 font__offset_y(void *font, const char *txt,
                   s32 align, s32 padding,
                   const gui_font_metrics_t *m)
{
	s32 height;
	if (align & GUI_ALIGN_MIDDLE) {
		height = 1;
		for (const char *c = txt; *c != '\0'; ++c)
			if (*c == '\n')
				++height;
		return   height % 2 == 0
		       ? -m->descent + m->line_gap + m->newline_dist * (height / 2 - 1)
		       :   -m->descent - (m->ascent - m->descent) / 2
		         + m->newline_dist * (height / 2);
	} else if (align & GUI_ALIGN_TOP) {
		return -m->ascent - m->line_gap / 2 - padding;
	} else /* default to GUI_ALIGN_BOTTOM */ {
		height = -m->descent + m->line_gap / 2 + padding;
		for (const char *c = txt; *c != '\0'; ++c)
			if (*c == '\n')
				height += m->newline_dist;
		return height;
	}
}



/* General Gui */

#define gi__gui_line_style_default { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_white, \
}

#define gi__gui_line_style_disabled { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_black, \
}

#define gi__gui_line_style_dark { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_black, \
}

#define gi__gui_line_style_lightblue { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_lightblue, \
}

#define gi__gui_line_style_medblue { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_medblue, \
}

#define gi__gui_line_style_orange { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_orange, \
}

#define gi__gui_text_style_default { \
	.size = 14, \
	.color = gi_white, \
	.align = GUI_ALIGN_MIDLEFT, \
	.padding = 4, \
	.wrap = false, \
}

#define gi__gui_text_style_dark { \
	.size = 14, \
	.color = gi_black, \
	.align = GUI_ALIGN_MIDCENTER, \
	.padding = 4, \
	.wrap = false, \
}

#define gi__gui_btn_text_style_default { \
	.size = 14, \
	.color = gi_white, \
	.align = GUI_ALIGN_MIDCENTER, \
	.padding = 4, \
	.wrap = false, \
}

#define gi__gui_btn_text_style_disabled { \
	.size = 14, \
	.color = gi_black, \
	.align = GUI_ALIGN_MIDCENTER, \
	.padding = 4, \
	.wrap = false, \
}

#define gi__gui_btn_inactive_style_default { \
	.line = gi__gui_line_style_default, \
	.text = gi__gui_btn_text_style_default, \
	.bg_color = gi_grey77, \
	.outline_color = gi_nocolor, \
}

#define gi__gui_btn_disabled_style_default { \
	.line = gi__gui_line_style_disabled, \
	.text = gi__gui_btn_text_style_disabled, \
	.bg_color = gi_black, \
	.outline_color = gi_nocolor, \
}

#define gi__gui_btn_style_default { \
	.pen = gui_pen_rect, \
	.inactive = gi__gui_btn_inactive_style_default, \
	.hot = { \
		.line = gi__gui_line_style_default, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = { .r=0x66, .g=0x66, .b=0x66, .a=0xff }, \
		.outline_color = gi_nocolor, \
	}, \
	.active = { \
		.line = gi__gui_line_style_default, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_orange, \
		.outline_color = gi_nocolor, \
	}, \
	.disabled = gi__gui_btn_disabled_style_default, \
	.hint = NULL, \
}

#define gi__gui_chk_style_default { \
	.pen = gui_pen_rect, \
	.inactive = { \
		.line = gi__gui_line_style_default, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_lightblue, \
		.outline_color = gi_nocolor, \
	}, \
	.hot = { \
		.line = gi__gui_line_style_default, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_medblue, \
		.outline_color = gi_nocolor, \
	}, \
	.active = { \
		.line = gi__gui_line_style_default, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_orange, \
		.outline_color = gi_nocolor, \
	}, \
	.disabled = gi__gui_btn_disabled_style_default, \
	.hint = NULL, \
}

#define gi__gui_slider_style_default { \
	.handle = gi__gui_chk_style_default, \
	.track = { \
		.pen = gui_pen_rect, \
		.inactive = gi__gui_btn_inactive_style_default, \
		.hot = gi__gui_btn_inactive_style_default, \
		.active = gi__gui_btn_inactive_style_default, \
		.disabled = gi__gui_btn_disabled_style_default, \
		.hint = NULL, \
	}, \
	.track_narrow = false, \
}

#define gi__gui_hint_style_default { \
	.line = gi__gui_line_style_default, \
	.text = gi__gui_btn_text_style_default, \
	.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff }, \
	.outline_color = gi_white, \
}

#define gi__gui_dropdown_style_default { \
	.btn = gi__gui_btn_style_default, \
	.shadow = { \
		.color = { .r=0x20, .g=0x20, .b=0x20, .a=0x20 }, \
		.width = 10, \
	}, \
}

#define gi__gui_color_picker_style_default { \
	.btn = gi__gui_btn_style_default, \
	.shadow = { \
		.color = { .r=0x20, .g=0x20, .b=0x20, .a=0x20 }, \
		.width = 10, \
	}, \
}

#define gi__gui_scroll_area_style_default { \
	.bg_color = gi_nocolor, \
	.padding = {0}, \
	.scrollbar_track_width = 5, \
	.scrollbar = gi__gui_slider_style_default, \
}

#define gi__gui_tree_style_default { \
	.pen = gui_pen_rect, \
	.inactive = { \
		.line = gi__gui_line_style_lightblue, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_nocolor, \
		.outline_color = gi_nocolor, \
	}, \
	.hot = { \
		.line = gi__gui_line_style_medblue, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_nocolor, \
		.outline_color = gi_nocolor, \
	}, \
	.active = { \
		.line = gi__gui_line_style_orange, \
		.text = gi__gui_btn_text_style_default, \
		.bg_color = gi_nocolor, \
		.outline_color = gi_nocolor, \
	}, \
	.disabled = { \
		.line = gi__gui_line_style_disabled, \
		.text = gi__gui_btn_text_style_disabled, \
		.bg_color = gi_nocolor, \
		.outline_color = gi_nocolor, \
	}, \
	.hint = NULL, \
}

const gui_style_t g_gui_style_default = {
	.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
	.line = gi__gui_line_style_default,
	.txt = gi__gui_text_style_default,
	.npt = gi__gui_btn_style_default,
	.btn = gi__gui_btn_style_default,
	.chk = gi__gui_chk_style_default,
	.slider = gi__gui_slider_style_default,
	.select = gi__gui_btn_style_default,
	.dropdown = gi__gui_dropdown_style_default,
	.color_picker = gi__gui_color_picker_style_default,
	.drag = gi__gui_chk_style_default,
	.hint = gi__gui_hint_style_default,
	.scroll_area = gi__gui_scroll_area_style_default,
	.tree = gi__gui_tree_style_default,
	.panel = {
		.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xbf },
		.border_color = gi_grey128,
		.titlebar = {
			.line = gi__gui_line_style_default,
			.text = gi__gui_text_style_default,
			.bg_color = { .r=0x66, .g=0x66, .b=0x66, .a=0xff },
			.outline_color = gi_nocolor,
		},
		.drag = {
			.pen = gui_pen_panel_drag,
			.inactive = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = gi_white,
				.outline_color = gi_nocolor,
			},
			.hot = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = { .r=0xcc, .g=0xcc, .b=0xcc, .a=0xff },
				.outline_color = gi_nocolor,
			},
			.active = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = gi_orange,
				.outline_color = gi_nocolor,
			},
			.disabled = { 0 },
			.hint = NULL, \
		},
		.tab = {
			.pen = gui_pen_rect,
			.inactive = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = { .r=0x66, .g=0x66, .b=0x66, .a=0xff },
				.outline_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
			},
			.hot = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = gi_grey77,
				.outline_color = gi_nocolor,
			},
			.active = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
				.outline_color = gi_nocolor,
			},
			.disabled = gi__gui_btn_disabled_style_default,
			.hint = NULL, \
		},
		.close = {
			.pen = gui_pen_panel_close,
			.inactive = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = gi_white,
				.outline_color = gi_nocolor,
			},
			.hot = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = { .r=0xcc, .g=0xcc, .b=0xcc, .a=0xff },
				.outline_color = gi_nocolor,
			},
			.active = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = gi_orange,
				.outline_color = gi_nocolor,
			},
			.disabled = { 0 },
			.hint = NULL, \
		},
		.collapse = {
			.pen = gui_pen_panel_collapse,
			.inactive = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = gi_white,
				.outline_color = gi_nocolor,
			},
			.hot = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = { .r=0xcc, .g=0xcc, .b=0xcc, .a=0xff },
				.outline_color = gi_nocolor,
			},
			.active = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = gi_orange,
				.outline_color = gi_nocolor,
			},
			.disabled = { 0 },
			.hint = NULL, \
		},
		.restore = {
			.pen = gui_pen_panel_restore,
			.inactive = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = gi_white,
				.outline_color = gi_nocolor,
			},
			.hot = {
				.line = gi__gui_line_style_dark,
				.text = gi__gui_text_style_dark,
				.bg_color = { .r=0xcc, .g=0xcc, .b=0xcc, .a=0xff },
				.outline_color = gi_nocolor,
			},
			.active = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_btn_text_style_default,
				.bg_color = gi_orange,
				.outline_color = gi_nocolor,
			},
			.disabled = { 0 },
			.hint = NULL, \
		},
		.scrollbar = gi__gui_slider_style_default,
		.cell_bg_color = gi_nocolor,
		.cell_border_color = gi_nocolor,
		.padding = { 10, 10, 10, 10 },
	},
	.split = {
		.thickness = 1.f,
		.dash_len = 0.f,
		.color = gi_grey128,
	}
};

#define gi__gui_line_style_invis { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_nocolor, \
}

#define gi__gui_text_style_invis { \
	.size = 14, \
	.color = gi_nocolor, \
	.align = GUI_ALIGN_LEFT | GUI_ALIGN_MIDDLE, \
	.padding = 4, \
	.wrap = false, \
}

#define gi__gui_element_style_invis { \
	.line = gi__gui_line_style_invis, \
	.text = gi__gui_text_style_invis, \
	.bg_color = gi_nocolor, \
	.outline_color = gi_nocolor, \
}

#define gi__gui_widget_style_invis { \
	.pen = gui_pen_null, \
	.inactive = gi__gui_element_style_invis, \
	.hot = gi__gui_element_style_invis, \
	.active = gi__gui_element_style_invis, \
	.disabled = gi__gui_element_style_invis, \
	.hint = NULL, \
}

#define gi__gui_slider_style_invis { \
	.handle = gi__gui_widget_style_invis, \
	.track = gi__gui_widget_style_invis, \
	.track_narrow = false, \
}

#define gi__gui_dropdown_style_invis { \
	.btn = gi__gui_widget_style_invis, \
	.shadow = { \
		.color = gi_nocolor, \
		.width = 0, \
	}, \
}

#define gi__gui_color_picker_style_invis { \
	.btn = gi__gui_widget_style_invis, \
	.shadow = { \
		.color = gi_nocolor, \
		.width = 0, \
	}, \
}

#define gi__gui_scroll_area_style_invis { \
	.bg_color = gi_nocolor, \
	.padding = {0}, \
	.scrollbar_track_width = 0, \
	.scrollbar = gi__gui_slider_style_invis, \
}

const gui_style_t g_gui_style_invis = {
	.bg_color     = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
	.line         = gi__gui_line_style_invis,
	.txt          = gi__gui_text_style_invis,
	.npt          = gi__gui_widget_style_invis,
	.btn          = gi__gui_widget_style_invis,
	.chk          = gi__gui_widget_style_invis,
	.slider       = gi__gui_slider_style_invis,
	.select       = gi__gui_widget_style_invis,
	.dropdown     = gi__gui_dropdown_style_invis,
	.color_picker = gi__gui_color_picker_style_invis,
	.drag         = gi__gui_widget_style_invis,
	.hint         = gi__gui_element_style_invis,
	.scroll_area  = gi__gui_scroll_area_style_invis,
	.tree         = gi__gui_widget_style_invis,
	.panel        = {
		.bg_color          = gi_nocolor,
		.border_color      = gi_nocolor,
		.drag              = gi__gui_widget_style_invis,
		.tab               = gi__gui_widget_style_invis,
		.titlebar          = gi__gui_element_style_invis,
		.close             = gi__gui_widget_style_invis,
		.collapse          = gi__gui_widget_style_invis,
		.restore           = gi__gui_widget_style_invis,
		.scrollbar         = gi__gui_slider_style_invis,
		.cell_bg_color     = gi_nocolor,
		.cell_border_color = gi_nocolor,
		.padding           = { 10, 10, 10, 10 },
	},
	.split    = gi__gui_line_style_invis,
};


#ifndef GUI_MAX_VERTS
#define GUI_MAX_VERTS 4096
#endif

#ifndef GUI_MAX_DRAW_CALLS
#define GUI_MAX_DRAW_CALLS 1024
#endif

#ifndef GUI_MAX_LAYERS
#define GUI_MAX_LAYERS 32
#endif

#ifndef GUI_MASK_STACK_LIMIT
#define GUI_MASK_STACK_LIMIT 8
#endif

#define GUI__LAYER_PRIORITY_HINT  2
#define GUI__LAYER_PRIORITY_POPUP 1

typedef enum gui__key_toggle_state
{
	GUI__KBT_OFF,
	GUI__KBT_PRESSED_ON,
	GUI__KBT_RELEASED_ON,
} gui__key_toggle_state_e;

typedef struct gui__repeat
{
	u32 val;
	u32 delay;
	u32 interval;
	u32 timer;
	b32 triggered;
} gui__repeat_t;

typedef struct gui_tree {
	gui_tree_node_t *nodes;
	u32 max_nodes;
	s32 row_height;
	s32 indent;
	u32 node_iter;
	u32 depth;
	u32 insert_nodes_at_depth;
} gui_tree_t;

typedef struct gui_popup
{
	u64 id;
	struct {
		s32 x, y, w, h;
	} mask;
	struct gui_popup *prev;
	b32 close_at_end;
	gui_grid_t grid;
} gui_popup_t;

typedef struct gui_mouse_press {
	timepoint_t time;
	u32 btn;
} gui_mouse_press_t;

typedef struct gui
{
	timepoint_t creation_time;
	timepoint_t frame_start_time;
	timepoint_t last_input_time;
	u32 frame_time_milli;

	/* external */
	u32 texture_white;
	u32 texture_white_dotted;
	gui_fonts_t fonts;

	/* rendering */
	v2f verts[GUI_MAX_VERTS];
	color_t vert_colors[GUI_MAX_VERTS];
	v2f vert_tex_coords[GUI_MAX_VERTS];
	u32 vert_cnt;
	gui_draw_call_t draw_calls[GUI_MAX_DRAW_CALLS];
	u32 draw_call_cnt;
	u32 draw_call_vert_idx;
	gui_layer_t layers[GUI_MAX_LAYERS];
	gui_layer_t *layer;
	box2i masks[GUI_MASK_STACK_LIMIT];
	box2i *mask;
	u32 culled_draw_calls;
	u32 culled_vertices;
	u32 culled_widgets;
	s32 scale;

	void *window;
	v2i window_dim;

	/* mouse */
	v2i mouse_pos;
	v2i mouse_pos_last;
	v2i mouse_pos_press;
	u32 mouse_btn;
	u32 mouse_btn_last;
	gui_mouse_press_t mouse_press_history[3];
	u64 mouse_covered_by_widget_id;
	b32 mouse_debug;
	gui__repeat_t mouse_repeat;
	b32 left_window;
	u32 cursor;

	/* keyboard */
	u8 prev_keys[KB_COUNT];
	u8 keys[KB_COUNT];
	s32 key_toggles[KBT_COUNT]; /* gui__key_toggle_state_e */
	gui__repeat_t key_repeat;
	char text_npt[32];
	char clipboard_in[GUI_CLIPBOARD_SIZE];
	char clipboard_out[GUI_CLIPBOARD_SIZE];

	/* misc input events */
	char drop_file[PATH_MAX];

	/* style */
	gui_style_t style;
	u8 style_stack[GUI_STYLE_STACK_LIMIT];
	u32 style_stack_sz;

	/* general widget state
	 * hot    = hover
	 * active = mouse down
	 * hot    = post-click activation, e.g. text input */
	u32 lock;
	u64 hot_id;
	u64 active_id;
	u64 focus_ids[GUI_FOCUS_STACK_SIZE];
	u64 active_id_at_frame_start;
	u64 focus_id_at_frame_start;
	b32 hot_id_found_this_frame;
	b32 active_id_found_this_frame;
	b32 focus_id_found_this_frame;
	b32 focus_next_widget;
	u64 focus_prev_widget_id;
	u64 prev_widget_id;
	u64 hot_id_last_frame;
	gui_widget_bounds_t *widget_bounds;
	gui_widget_bounds_t default_widget_bounds;

	/* specific widget state */
	struct {
		b32 active;
		u32 cursor; /* byte, NOT GLYPH, offset */
		u32 selection; /* byte, NOT GLYPH, offset */
		u32 initial_txt_hash;
		char val_buf[GUI_TXT_MAX_LENGTH];
		char pw_buf[GUI_TXT_MAX_LENGTH];
	} npt;
	v2i drag_offset;
	struct
	{
		u64 id;
		s32 x, y, w, h;
		u32 *val;
		u32 num_items;
		u32 item_idx;
		b32 triggered_by_key;
		s32 render_state;
		b32 contains_mouse;
		char selected_item_txt[GUI_TXT_MAX_LENGTH];
	} dropdown;
	gui_popup_t popups[GUI_POPUP_STACK_SIZE];
	gui_popup_t *popup;
	b32 dragging_window;
	colorf_t color_picker8_color;
	gui_scroll_area_t *scroll_area;
	gui_tree_t tree;
	s32 hint_timer;
	b32 text_input_exceeds_buffer_size;
	b32 clipboard_input_exceeds_buffer_size;

	/* grid */
	gui_grid_t grid_panel;
	gui_grid_t *grid;

	/* splits */
	gui_split_t *splits;
	u32 num_splits;
	gui_split_t *root_split;
	b32 splits_rendered_this_frame;

	/* panels */
	gui_panel_t *panel;
	b32 is_dragging_dockable_panel;
	s32 next_panel_pri, min_panel_pri;
} gui_t;


/* Font */

static
s32 gui__txt_line_width(const gui_t *gui, const char *txt, s32 size_)
{
	const s32 size = gui_scale_val(gui, size_);
	void *font = gui->fonts.get_font(gui->fonts.handle, size);
	gui_char_quad_f get_char_quad = gui->fonts.get_char_quad;
	return font ? font__line_width(font, txt, get_char_quad) : 0;
}

static
s32 gui__txt_line_offset_x(const gui_t *gui, const char *txt,
                           const gui_text_style_t *style)
{
	const s32 size = gui_scale_val(gui, style->size);
	void *font = gui->fonts.get_font(gui->fonts.handle, size);
	const s32 align = style->align;
	const s32 padding = gui_scale_val(gui, style->padding);
	gui_char_quad_f get_char_quad = gui->fonts.get_char_quad;
	return font ? font__line_offset_x(font, txt, align, padding, get_char_quad) : 0;
}

static
s32 gui__txt_offset_y(const gui_t *gui, const char *txt, const gui_text_style_t *style)
{
	const s32 size = gui_scale_val(gui, style->size);
	void *font = gui->fonts.get_font(gui->fonts.handle, size);
	gui_font_metrics_t metrics;
	const s32 padding = gui_scale_val(gui, style->padding);

	if (!font)
		return 0;

	gui->fonts.get_metrics(font, &metrics);
	return font__offset_y(font, txt, style->align, padding, &metrics);
}

static
v2f gui__txt_start_pos(const gui_t *gui, const char *txt,
                       v2i anchor, const gui_text_style_t *style)
{
	return (v2f) {
		.x = anchor.x + gui__txt_line_offset_x(gui, txt, style),
		.y = anchor.y + gui__txt_offset_y(gui, txt, style),
	};
}

static
void gui__repeat_init(gui__repeat_t *repeat)
{
	repeat->val = 0;
	repeat->delay = 500;
	repeat->interval = 100;
	repeat->timer = repeat->delay;
	repeat->triggered = false;
}

static
void gui__repeat_update(gui__repeat_t *repeat, u32 val, u32 pop, u32 frame)
{
	repeat->triggered = false;
	if (pop == 1) {
		if (repeat->val == val) {
			if (repeat->timer <= frame) {
				repeat->timer = repeat->interval - (frame - repeat->timer);
				repeat->triggered = true;
			} else {
				repeat->timer -= frame;
			}
		} else {
			repeat->timer = repeat->delay;
			repeat->val = val;
			repeat->triggered = true;
		}
	} else {
		repeat->val = 0;
	}
}

static
b32 gui__key_triggered(const gui_t *gui, u32 key)
{
	return gui->key_repeat.triggered && gui->key_repeat.val == key;
}

static void gui__layer_init(gui_t *gui, gui_layer_t *layer, s32 x, s32 y, s32 w, s32 h);
static void gui__layer_new(gui_t *gui);

gui_t *gui_create(s32 w, s32 h, u32 texture_white, u32 texture_white_dotted,
                  gui_fonts_t fonts)
{
	gui_t *gui = calloc(1, sizeof(gui_t));

	gui->creation_time = time_current();
	gui->frame_start_time = gui->creation_time;
	gui->last_input_time = gui->creation_time;
	gui->frame_time_milli = 0;

	gui->texture_white = texture_white;
	gui->texture_white_dotted = texture_white_dotted;
	gui->fonts = fonts;

	memset(gui->prev_keys, 0, KB_COUNT);
	memset(gui->keys, 0, KB_COUNT);
	memset(gui->key_toggles, 0, sizeof(gui->key_toggles));
	gui__repeat_init(&gui->key_repeat);
	memset(gui->text_npt, 0, sizeof(gui->text_npt));
	memset(gui->clipboard_in, 0, sizeof(gui->clipboard_in));
	memset(gui->clipboard_out, 0, sizeof(gui->clipboard_out));

	gui_style_set(gui, &g_gui_style_default);

	gui->style_stack_sz = 0;

	gui->scale = 100;

	gui->window_dim.x = w;
	gui->window_dim.y = h;

	gui->mouse_pos = g_v2i_zero;
	gui->mouse_btn = 0;
	gui->mouse_btn_last = 0;
	gui->mouse_pos_last = g_v2i_zero;
	gui->mouse_pos_press = g_v2i_zero;
	arrclr(gui->mouse_press_history);
	gui->mouse_debug = false;
	gui__repeat_init(&gui->mouse_repeat);
	gui->left_window = false;
	gui->cursor = GUI_CURSOR_DEFAULT;

	gui->lock = 0;
	gui->hot_id = 0;
	gui->active_id = 0;
	arrclr(gui->focus_ids);
	gui->active_id_at_frame_start = 0;
	gui->focus_id_at_frame_start = 0;
	gui->focus_next_widget = false;
	gui->focus_prev_widget_id = 0;
	gui->prev_widget_id = 0;
	gui->hot_id_last_frame = 0;
	gui->widget_bounds = &gui->default_widget_bounds;
	memclr(gui->default_widget_bounds);


	gui->npt.active = false;
	gui->npt.cursor = 0;
	gui->npt.selection = 0;
	gui->npt.initial_txt_hash = 0;
	gui->npt.pw_buf[0] = 0;
	gui->npt.val_buf[0] = 0;
	gui->drag_offset = g_v2i_zero;
	gui->dropdown.id = 0;
	arrclr(gui->popups);
	gui->popup = NULL;
	gui->dragging_window = false;
	gui->color_picker8_color = (colorf_t){0};
	gui->scroll_area = NULL;
	memclr(gui->tree);
	gui->hint_timer = GUI_HINT_TIMER;
	gui->text_input_exceeds_buffer_size = false;
	gui->clipboard_input_exceeds_buffer_size = false;

	gui->grid = NULL;

	gui->splits = NULL;
	gui->num_splits = 0;
	gui->root_split = NULL;
	gui->splits_rendered_this_frame = false;

	gui->panel = NULL;
	gui->is_dragging_dockable_panel = false;
	gui->next_panel_pri = 0;
	gui->min_panel_pri = 0;

	return gui;
}

void gui_destroy(gui_t *gui)
{
	free(gui);
}

void gui_dim(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->window_dim.x;
	*y = gui->window_dim.y;
}

static
b32 gui__key_is_mod(gui_key_t key)
{
	switch (key) {
	case KB_CAPSLOCK:
	case KB_LCTRL:
	case KB_LSHIFT:
	case KB_LALT:
	case KB_LGUI:
	case KB_RCTRL:
	case KB_RSHIFT:
	case KB_RALT:
	case KB_RGUI:
		return true;
	default:
		return false;
	}
}

static
void gui__toggle_key(gui_t *gui, gui_key_toggle_t toggle, gui_key_t key)
{
	if (gui->key_toggles[toggle] == GUI__KBT_OFF && key_pressed(gui, key)) {
		gui->key_toggles[toggle] = GUI__KBT_PRESSED_ON;
	} else if (key_released(gui, key)) {
		if (gui->key_toggles[toggle] == GUI__KBT_PRESSED_ON)
			gui->key_toggles[toggle] = GUI__KBT_RELEASED_ON;
		else
			gui->key_toggles[toggle] = GUI__KBT_OFF;
	}
}

static
b32 gui__get_most_focused_widget(const gui_t *gui, u64 *id)
{
	for (u32 i = countof(gui->focus_ids); i > 0; --i) {
		if (gui->focus_ids[i-1] != 0) {
			*id = gui->focus_ids[i-1];
			return true;
		}
	}
	*id = 0;
	return false;
}

static
b32 gui__is_most_focused_widget(const gui_t *gui, u64 id)
{
	for (u32 i = countof(gui->focus_ids); i > 0; --i)
		if (gui->focus_ids[i-1] != 0)
			return id == gui->focus_ids[i-1];
	return false;
}

static
b32 gui__widget_focus_slot(const gui_t *gui, u64 id, u32 *slot)
{
	for (u32 i = 0; i < countof(gui->focus_ids); ++i) {
		if (gui->focus_ids[i] == id) {
			*slot = i;
			return true;
		}
	}
	*slot = ~0;
	return false;
}

static
b32 gui__find_popup(const gui_t *gui, u64 id, u32 *slot)
{
	for (u32 i = 0; i < countof(gui->popups); ++i) {
		if (gui->popups[i].id == id) {
			*slot = i;
			return true;
		}
	}
	*slot = ~0;
	return false;
}

static
void gui__defocus_widget(gui_t *gui, u64 id)
{
	u32 slot;
	if (gui__widget_focus_slot(gui, id, &slot)) {
		for (u32 i = slot; i < countof(gui->focus_ids); ++i)
			gui->focus_ids[i] = 0;
	} else {
		assert(0);
	}
	if (gui__find_popup(gui, id, &slot))
		for (u32 i = slot; i < countof(gui->popups); ++i)
			gui->popups[i].id = 0;
}

static
void gui__focus_widget(gui_t *gui, u64 id)
{
	gui->focus_id_found_this_frame = true;
	for (u32 i = 0; i < countof(gui->focus_ids); ++i) {
		if (gui->focus_ids[i] == 0) {
			gui->focus_ids[i] = id;
			return;
		}
	}
	assert(0);
}

static
void gui__tab_focus_adjacent_widget(gui_t *gui)
{
	u64 id;
	if (gui__get_most_focused_widget(gui, &id))
		gui__defocus_widget(gui, id);
	if (key_mod(gui, KBM_SHIFT)) {
		gui->focus_prev_widget_id = gui->prev_widget_id;
		gui->focus_id_found_this_frame = true;
		gui->focus_next_widget = false;
	} else {
		gui->focus_next_widget = true;
		gui->focus_prev_widget_id = 0;
	}
}

static
void gui__on_widget_tab_focused(gui_t *gui, u64 id)
{
	assert(gui->focus_next_widget || gui->focus_prev_widget_id == id);

	gui__focus_widget(gui, id);
	gui->focus_next_widget = false;
	gui->focus_prev_widget_id = 0;
	gui->focus_id_found_this_frame = true;
}

void gui_begin_frame(gui_t *gui)
{
	const timepoint_t now = time_current();

	gui->frame_time_milli = time_diff_milli(gui->frame_start_time, now);
	gui->frame_start_time = now;
}

void gui_events_begin(gui_t *gui)
{
	gui->mouse_pos_last = gui->mouse_pos;
	gui->mouse_btn_last = gui->mouse_btn;
	memcpy(gui->prev_keys, gui->keys, KB_COUNT);
	memset(gui->keys, 0, KB_COUNT);

	gui->mouse_btn = 0;
	gui->text_npt[0] = '\0';
	gui->clipboard_in[0] = '\0';
	gui->clipboard_out[0] = '\0';
	gui->left_window = false;
	gui->text_input_exceeds_buffer_size = false;
	gui->clipboard_input_exceeds_buffer_size = false;
	gui->drop_file[0] = '\0';
}

void gui_event_add_update(gui_t *gui)
{
	gui->last_input_time = gui->frame_start_time;
}

void gui_event_set_mouse_pos(gui_t *gui, s32 x, s32 y)
{
	gui->mouse_pos.x = x;
	gui->mouse_pos.y = y;
}

void gui_event_set_mouse_btn(gui_t *gui, gui_mouse_button_e button)
{
	gui->mouse_btn = button;
}

void gui_event_add_text_input(gui_t *gui, const char *text)
{
	if (strlen(gui->text_npt) + strlen(text) + 1 <= countof(gui->text_npt))
		strcat(gui->text_npt, text);
	else
		gui->text_input_exceeds_buffer_size = true;
	if (text[0] != 0)
		gui_event_add_update(gui);
}

void gui_event_set_keyboard(gui_t *gui, const u8 *keys, u32 key_cnt)
{
	assert(key_cnt >= KB_COUNT);
	memcpy(gui->keys, keys, KB_COUNT);
}

void gui_event_add_clipboard(gui_t *gui, const char *text)
{
	if (strlen(gui->clipboard_in) + strlen(text) + 1 <= countof(gui->clipboard_in))
		strcat(gui->clipboard_in, text);
	else
		gui->clipboard_input_exceeds_buffer_size = true;
}

void gui_event_add_window_leave(gui_t *gui)
{
	gui->left_window = true;
	gui_event_add_update(gui);
}

void gui_event_set_window_dim(gui_t *gui, s32 w, s32 h)
{
	if (gui->window_dim.x != w || gui->window_dim.y != h)
		gui_event_add_update(gui);
	gui->window_dim.x = w;
	gui->window_dim.y = h;
}

void gui_event_add_drop_file(gui_t *gui, const char *path)
{
	strbcpy(gui->drop_file, path);
}

void gui_events_end(gui_t *gui)
{
	if (gui->left_window && !gui->dragging_window) {
		gui->mouse_btn = 0;
		gui->hot_id = 0;
		gui->active_id = 0;
	}

	if (mouse_pressed(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT))
		gui->mouse_pos_press = gui->mouse_pos;
	gui__repeat_update(&gui->mouse_repeat, gui->mouse_btn,
	                   gui__popcount(gui->mouse_btn),
	                   gui->frame_time_milli);
	if (mouse_down(gui, ~0))
		gui_event_add_update(gui);
	if (gui->mouse_btn != gui->mouse_btn_last)
		gui_event_add_update(gui);
	if (!v2i_equal(gui->mouse_pos_last, gui->mouse_pos))
		gui_event_add_update(gui);

	if (mouse_pressed(gui, ~0)) {
		for (u32 i = countof(gui->mouse_press_history); i > 1; --i)
			gui->mouse_press_history[i-1] = gui->mouse_press_history[i-2];
		gui->mouse_press_history[0].time = gui->frame_start_time;
		gui->mouse_press_history[0].btn  = gui->mouse_btn;
	}

	{
		u32 key = 0;
		u32 cnt = 0;
		for (u32 i = 1; i < KB_COUNT; ++i) {
			if (gui->keys[i] && !gui__key_is_mod(i)) {
				key = i;
				++cnt;
			}
		}
		gui__repeat_update(&gui->key_repeat, key, cnt, gui->frame_time_milli);
	}

	gui__toggle_key(gui, KBT_CAPS, KB_CAPSLOCK);
	gui__toggle_key(gui, KBT_SCROLL, KB_SCROLLLOCK);
	gui__toggle_key(gui, KBT_NUM, KB_NUMLOCK_OR_CLEAR);

	/* events are done; finish initializing frame */

	gui->mouse_covered_by_widget_id = ~0;

	gui->lock = 0;

	/* Should rarely hit these */
	if (gui->hot_id != 0 && !gui->hot_id_found_this_frame) {
		log_warn("hot widget %" PRIu64 " was not drawn", gui->hot_id);
		gui->hot_id = 0;
	}
	gui->hot_id_found_this_frame = false;
	if (gui->active_id != 0 && !gui->active_id_found_this_frame) {
		log_warn("active widget %" PRIu64 " was not drawn", gui->active_id);
		gui->active_id = 0;
	}
	gui->active_id_found_this_frame = false;
	if (gui->focus_ids[0] != 0 && !gui->focus_id_found_this_frame) {
		for (u32 i = 0; i < countof(gui->focus_ids); ++i)
			if (gui->focus_ids[i] != 0)
				log_warn("focus widget %" PRIu64 " was not drawn", gui->focus_ids[i]);
		arrclr(gui->focus_ids);
		arrclr(gui->popups);
		gui->npt.active = false;
	}
	gui->focus_id_found_this_frame = false;

	if (gui__key_triggered(gui, KB_TAB) && gui->focus_ids[0] == 0)
		gui__tab_focus_adjacent_widget(gui);

	gui->active_id_at_frame_start = gui->active_id;
	gui->focus_id_at_frame_start = gui->focus_ids[0];

	if (gui->active_id != 0)
		gui->hint_timer = GUI_HINT_TIMER;
	else if (gui->hot_id != 0 && gui->hot_id == gui->hot_id_last_frame)
		gui->hint_timer = max(gui->hint_timer - (s32)gui->frame_time_milli,
		                      -GUI_HINT_TIMER);
	else
		gui->hint_timer = min(gui->hint_timer + (s32)gui->frame_time_milli,
		                      GUI_HINT_TIMER);

	gui->hot_id_last_frame = gui->hot_id;

	gui->widget_bounds = &gui->default_widget_bounds;
	memclr(gui->default_widget_bounds);
	box2i_from_xywh(&gui->default_widget_bounds.bbox, 0, 0,
	                gui->window_dim.x, gui->window_dim.y);

	gui->vert_cnt = 0;
	gui->draw_call_cnt = 0;
	gui->draw_call_vert_idx = 0;
	memclr(gui->layers);
	gui->layer = &gui->layers[0];
	gui__layer_init(gui, gui->layer, 0, 0, gui->window_dim.x, gui->window_dim.y);
	memclr(gui->masks);
	gui->mask = &gui->masks[0];
	box2i_from_xywh(gui->mask, 0, 0, gui->window_dim.x, gui->window_dim.y);

	gui->culled_draw_calls = 0;
	gui->culled_vertices   = 0;
	gui->culled_widgets    = 0;

	/* popup */
	for (u32 i = countof(gui->popups); i > 0; --i) {
		const u32 j = i - 1;
		if (gui->popups[j].id != 0) {
			const s32 x = gui->popups[j].mask.x;
			const s32 y = gui->popups[j].mask.y;
			const s32 w = gui->popups[j].mask.w;
			const s32 h = gui->popups[j].mask.h;
			box2i box;
			box2i_from_xywh(&box, x, y, w, h);

			assert(gui->focus_ids[j] == gui->popups[j].id);

			/* catch mouse_covered_by_panel */
			if (box2i_contains_point(box, gui->mouse_pos)) {
				mouse_cover(gui, gui->popups[j].id);
				break;
			}
		}
	}
	gui->popup = NULL;

	/* ensure this is set every frame when dragging the window (perf) */
	gui->dragging_window = false;

	gui->cursor = GUI_CURSOR_DEFAULT;
	if (gui->root_split) {
		gui_splits_compute(gui);
		gui->splits_rendered_this_frame = false;
	}

	gui->is_dragging_dockable_panel = false;
	gui->next_panel_pri = 0;
	gui->min_panel_pri = 0;

	if (gui->mouse_debug && mouse_pressed(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT))
		gui_circ(gui, gui->mouse_pos.x, gui->mouse_pos.y, 10, g_yellow, g_nocolor);

	/* kinda wasteful, but ensures split resizers & mouse debug are drawn on top */
	gui__layer_new(gui);
}

void gui_window_drag(gui_t *gui)
{
	gui->dragging_window = true;
	gui->mouse_pos = gui->mouse_pos_last;
}

b32 gui_window_dragging(const gui_t *gui)
{
	return gui->dragging_window;
}

void gui_window_drag_end(gui_t *gui)
{
	gui->dragging_window = false;
}

static
box2i gui__current_mask(const gui_t *gui)
{
	const gui_layer_t *layer = gui->layer;
	box2i box;
	box2i_from_xywh(&box, layer->x, layer->y, layer->w, layer->h);
	return box;
}

static
void gui__mask_box(const gui_t *gui, box2i *box)
{
	const box2i mask = gui__current_mask(gui);
	box->min = box2i_clamp_point(mask, box->min);
	box->max = box2i_clamp_point(mask, box->max);
}

b32 gui_point_visible(const gui_t *gui, s32 x, s32 y)
{
	const v2i pt = { .x = x, .y = y };
	const box2i mask = gui__current_mask(gui);
	return box2i_contains_point(mask, pt);
}

static
b32 gui__box_visible(const gui_t *gui, box2i box)
{
	const box2i mask = gui__current_mask(gui);
	/* NOTE(rgriege): use 0 for error so adjacent rectangles are not visible */
	return box2i_overlaps_within(mask, box, 0);
}

b32 gui_box_visible(const gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, x, y, w, h);
	return gui__box_visible(gui, box);
}

static
b32 gui__box_half_visible(const gui_t *gui, box2i box)
{
	const box2i mask = gui__current_mask(gui);
	const box2i clipped = box2i_intersection(mask, box);
	return    clipped.max.x - clipped.min.x >= (box.max.x - box.min.x) / 2
	       && clipped.max.y - clipped.min.y >= (box.max.y - box.min.y) / 2;
}

void gui_widget_bounds_push(gui_t *gui, gui_widget_bounds_t *bounds, b32 propogate)
{
	if (propogate)
		box2i_extend_box(&gui->widget_bounds->children, bounds->bbox);
	bounds->prev = gui->widget_bounds;
	gui->widget_bounds = bounds;
}

void gui_widget_bounds_pop(gui_t *gui, gui_widget_bounds_t *bounds, b32 propogate)
{
	assert(gui->widget_bounds == bounds);
	if (gui->widget_bounds->prev) {
		if (propogate)
			box2i_extend_box(&gui->widget_bounds->prev->children, gui->widget_bounds->bbox);
		gui->widget_bounds = gui->widget_bounds->prev;
	} else {
		assert(false);
		gui->widget_bounds = &gui->default_widget_bounds;
	}
}

void gui_widget_bounds_extend(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, x, y, w, h);
	box2i_extend_box(&gui->widget_bounds->children, box);
}

b32 gui_begin(gui_t *gui, u32 num_verts, gui_draw_call_type_e type)
{
	return gui_begin_tex(gui, num_verts, type, 0, GUI_BLEND_NRM);
}

b32 gui_begin_tex(gui_t *gui, u32 num_verts, gui_draw_call_type_e type,
                  u32 tex, gui_blend_e blend)
{
	assert(num_verts > 0);
	if (   gui->vert_cnt + num_verts <= GUI_MAX_VERTS
	    && gui->draw_call_cnt < GUI_MAX_DRAW_CALLS) {
		gui_draw_call_t *draw_call = &gui->draw_calls[gui->draw_call_cnt];
		draw_call->idx   = gui->vert_cnt;
		draw_call->cnt   = num_verts;
		draw_call->type  = type;
		draw_call->tex   = tex != 0 ? tex : gui->texture_white;
		draw_call->blend = blend;
		gui->draw_call_vert_idx = 0;
		return true;
	} else {
		return false;
	}
}

static
void gui__vertf(gui_t *gui, r32 x, r32 y, color_t c, r32 u, r32 v)
{
	const u32 idx_local = gui->draw_call_vert_idx++;
	const u32 idx       = gui->vert_cnt + idx_local;
	if (idx_local < (u32)gui->draw_calls[gui->draw_call_cnt].cnt) {
		gui->verts[idx].x           = x;
		gui->verts[idx].y           = y;
		gui->vert_colors[idx]       = c;
		gui->vert_tex_coords[idx].x = u;
		gui->vert_tex_coords[idx].y = v;
	} else {
		assert(false);
	}
}

void gui_vertf(gui_t *gui, r32 x, r32 y, color_t c, r32 u, r32 v)
{
	box2i_extend_point(&gui->widget_bounds->children, (v2i){ (s32)x, (s32)y });
	gui__vertf(gui, x, y, c, u, v);
}

void gui_end(gui_t *gui)
{
	assert(gui->draw_call_vert_idx == gui->draw_calls[gui->draw_call_cnt].cnt);
	gui->vert_cnt += gui->draw_calls[gui->draw_call_cnt].cnt;
	++gui->draw_call_cnt;
	gui->draw_call_vert_idx = 0;
}

static
box2i gui__vert_bounds(const v2f *v, u32 n)
{
	box2f bbox_f;
	polyf_bounding_box(v, n, &bbox_f);
	return (box2i) {
		.min = { .x = (s32)bbox_f.min.x, .y = (s32)bbox_f.min.y },
		.max = { .x = (s32)bbox_f.max.x, .y = (s32)bbox_f.max.y },
	};
}

static
void gui__triangles(gui_t *gui, const v2f *v, u32 n, color_t fill)
{
	const box2i bbox = gui__vert_bounds(v, n);

	box2i_extend_box(&gui->widget_bounds->children, bbox);

	if (!gui__box_visible(gui, bbox)) {
		gui->culled_draw_calls += 1;
		gui->culled_vertices   += n;
		return;
	}

	if (color_equal(fill, g_nocolor))
		return;

	assert(n % 3 == 0);

	if (gui_begin(gui, n, GUI_DRAW_TRIANGLES)) {
		for (u32 i = 0; i < n; ++i)
			gui__vertf(gui, v[i].x, v[i].y, fill, 0.f, 0.f);
		gui_end(gui);
	}
}

/* NOTE(rgriege): this input vertices may point to the gui.verts output */
static
void gui__poly(gui_t *gui, const v2f *v, u32 n, gui_draw_call_type_e type,
               color_t fill, color_t stroke, b32 closed)
{
	const box2i bbox = gui__vert_bounds(v, n);

	box2i_extend_box(&gui->widget_bounds->children, bbox);

	if (!gui__box_visible(gui, bbox)) {
		const u32 s = color_equal(stroke, g_nocolor) + color_equal(fill, g_nocolor);
		gui->culled_draw_calls += s;
		gui->culled_vertices   += s * n;
		return;
	}

	if (!color_equal(fill, g_nocolor) && gui_begin(gui, n, type)) {
		for (u32 i = 0; i < n; ++i)
			gui__vertf(gui, v[i].x, v[i].y, fill, 0.f, 0.f);
		gui_end(gui);
	}

	if (!color_equal(stroke, g_nocolor)) {
		const r32 dash_len = gui->style.line.dash_len;
		if (dash_len != 0.f) {
			if (gui_begin_tex(gui, closed ? n + 1 : n, GUI_DRAW_LINE_STRIP,
			                  gui->texture_white_dotted, GUI_BLEND_NRM)) {
				r32 dist = 0.f;
				for (u32 i = 0; i < n; ++i) {
					gui__vertf(gui, v[i].x, v[i].y, stroke, dist / dash_len, 0.f);
					dist += v2f_dist(v[i], v[(i+1)%n]);
				}
				if (closed)
					gui__vertf(gui, v[0].x, v[0].y, stroke, dist / dash_len, 0.f);
				gui_end(gui);
			}
		} else {
			const gui_draw_call_type_e line_type
				= closed ? GUI_DRAW_LINE_LOOP : GUI_DRAW_LINE_STRIP;
			if (gui_begin(gui, n, line_type)) {
				for (u32 i = 0; i < n; ++i)
					gui__vertf(gui, v[i].x, v[i].y, stroke, 0.f, 0.f);
				gui_end(gui);
			}
		}
	}
}

m3f gui_texture_get_transform(const gui_texture_t *texture, s32 x, s32 y,
                              r32 sx, r32 sy, r32 rotation)
{
	const v2f dim = {
		((r32)texture->width) * sx,
		((r32)texture->height) * sy 
	};

	m3f transform = m3f_init_translation((v2f){ x, y });

	if (rotation != 0.f) {
		const v2f center = v2f_scale(dim, 0.5f);
		transform = m3f_mul_m3(transform, m3f_init_translation(center));
		transform = m3f_mul_m3(transform, m3f_init_rotation(rotation));
		transform = m3f_mul_m3(transform, m3f_init_translation(v2f_inverse(center)));
	}

	return m3f_mul_m3(transform, m3f_init_scale(dim));
}

static
void gui__texture(gui_t *gui, const gui_texture_t *texture, s32 x, s32 y,
                  r32 sx, r32 sy, r32 rotation, color_t color)
{
	static const v2f corners[] = {
		{ 0.f, 0.f },
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f }
	};
	const m3f transform = gui_texture_get_transform(texture, x, y, sx, sy, rotation);
	v2f points[4];
	box2i bbox;

	for (u32 i = 0; i < countof(points); ++i)
		points[i] = m3f_mul_v2(transform, corners[i]);

	bbox = gui__vert_bounds(B2PC(points));

	box2i_extend_box(&gui->widget_bounds->children, bbox);

	if (!gui__box_visible(gui, bbox)) {
		gui->culled_draw_calls += 1;
		gui->culled_vertices   += countof(points);
		return;
	}

	if (texture->blend == GUI_BLEND_MUL) {
		const r32 brightness = ((r32)color.a) / 255.f;
		color.r *= brightness;
		color.g *= brightness;
		color.b *= brightness;
	}

	if (gui_begin_tex(gui, countof(points), GUI_DRAW_TRIANGLE_FAN,
	                  texture->handle, texture->blend)) {
		for (u32 i = 0; i < countof(points); ++i)
			gui__vertf(gui, points[i].x, points[i].y, color, corners[i].u, corners[i].v);
		gui_end(gui);
	}
}

static
void gui__char(gui_t *gui, const gui_char_quad_t *q, color_t color)
{
	const box2i bbox = {
		.min = { (s32)q->x0, (s32)q->y0 },
		.max = { (s32)q->x1, (s32)q->y1 }
	};

	box2i_extend_box(&gui->widget_bounds->children, bbox);

	if (!gui__box_visible(gui, bbox)) {
		gui->culled_draw_calls += 1;
		gui->culled_vertices   += 4;
		return;
	}

	if (gui_begin_tex(gui, 4, GUI_DRAW_TRIANGLE_FAN,
	                  q->texture.handle, q->texture.blend)) {
		gui__vertf(gui, q->x0, q->y1, color, q->s0, q->t0);
		gui__vertf(gui, q->x0, q->y0, color, q->s0, q->t1);
		gui__vertf(gui, q->x1, q->y0, color, q->s1, q->t1);
		gui__vertf(gui, q->x1, q->y1, color, q->s1, q->t0);
		gui_end(gui);
	}
}

static void gui__layer_complete_current(gui_t *gui);
static int gui__layer_sort(const void *lhs, const void *rhs);

void gui_end_frame(gui_t *gui)
{
	assert(gui->grid == NULL);
	assert(gui->lock == 0);

	if (gui->root_split && !gui->splits_rendered_this_frame)
		gui_splits_render(gui);

	gui__layer_complete_current(gui);

	const u32 n_layers = gui_num_layers(gui);
	/* move hints/popups to the back of the array
	 * can't use qsort - not guaranteed to be stable */
	isort(gui->layers, n_layers, sizeof(gui->layers[0]), gui__layer_sort);
	/* front-to-back -> back-to-front */
	reverse(gui->layers, sizeof(gui->layers[0]), n_layers);
}

void gui_end_frame_ex(gui_t *gui, u32 target_frame_milli,
                      u32 idle_frame_milli, u32 idle_start_milli)
{
	u32 frame_milli;

	gui_end_frame(gui);
	frame_milli = time_diff_milli(gui_frame_start(gui), time_current());

	if (frame_milli > target_frame_milli)
		log_warn("long frame: %ums", frame_milli);
	else if (  time_diff_milli(gui_last_input_time(gui), gui_frame_start(gui))
	         > idle_start_milli)
		time_sleep_milli(idle_frame_milli - frame_milli);
	else
		time_sleep_milli(target_frame_milli - frame_milli);
}

b32 gui_text_input_active(const gui_t *gui)
{
	return gui->npt.active;
}

#ifdef __APPLE__
#define GUI__KBM_CLIPBOARD KBM_GUI
#else
#define GUI__KBM_CLIPBOARD KBM_CTRL
#endif

b32 gui_has_clipboard_text(const gui_t *gui)
{
	return gui->clipboard_out[0] != 0;
}

void gui_get_clipboard_text(const gui_t *gui, char *text)
{
	assert(gui_has_clipboard_text(gui));
	strcpy(text, gui->clipboard_out);
}

b32 gui_has_drop_file(const gui_t *gui)
{
	return gui->drop_file[0] != 0;
}

u32 gui_cursor(const gui_t *gui)
{
	return gui->cursor;
}

void *gui_window(gui_t *gui)
{
	return gui->window;
}

void gui_set_window(gui_t *gui, void *window)
{
	gui->window = window;
}

timepoint_t gui_frame_start(const gui_t *gui)
{
	return gui->frame_start_time;
}

u32 gui_frame_time_milli(const gui_t *gui)
{
	return gui->frame_time_milli;
}

timepoint_t gui_last_input_time(const gui_t *gui)
{
	return gui->last_input_time;
}

/* Input */

void mouse_pos(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->mouse_pos.x;
	*y = gui->mouse_pos.y;
}

void mouse_pos_last(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->mouse_pos_last.x;
	*y = gui->mouse_pos_last.y;
}

void mouse_pos_press(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->mouse_pos_press.x;
	*y = gui->mouse_pos_press.y;
}

b32 mouse_pos_changed(const gui_t *gui)
{
	return !v2i_equal(gui->mouse_pos, gui->mouse_pos_last);
}

void mouse_pos_delta(const gui_t *gui, s32 *x, s32 *y)
{
	const v2i delta = v2i_sub(gui->mouse_pos, gui->mouse_pos_last);
	*x = delta.x;
	*y = delta.y;
}

b32 mouse_pressed(const gui_t *gui, u32 mask)
{
	return (gui->mouse_btn & mask) && !(gui->mouse_btn_last & mask);
}

b32 mouse_pressed_bg(const gui_t *gui, u32 mask)
{
	return mouse_pressed(gui, mask) && mouse_over_bg(gui);
}

b32 mouse_down(const gui_t *gui, u32 mask)
{
	return gui->mouse_btn & mask;
}

b32 mouse_down_bg(const gui_t *gui, u32 mask)
{
	return mouse_down(gui, mask) && mouse_over_bg(gui);
}

b32 mouse_released(const gui_t *gui, u32 mask)
{
	return !(gui->mouse_btn & mask) && (gui->mouse_btn_last & mask);
}

b32 mouse_released_bg(const gui_t *gui, u32 mask)
{
	return mouse_released(gui, mask) && mouse_over_bg(gui);
}

static
b32 mouse__covered_by_current_popup(const gui_t *gui)
{
	return gui->popup && gui->mouse_covered_by_widget_id == gui->popup->id;
}

static
b32 gui__popup_in_current_stack(const gui_t *gui, const gui_popup_t *popup)
{
	const gui_popup_t *p = gui->popup;
	while (p) {
		if (p == popup)
			return true;
		p = p->prev;
	}
	return false;
}

static
b32 mouse__covered_by_popup(const gui_t *gui)
{
	for (u32 i = 0; i < countof(gui->popups); ++i)
		if (   gui->popups[i].id == gui->mouse_covered_by_widget_id
		    && !gui__popup_in_current_stack(gui, &gui->popups[i]))
			return true;
	return false;
}

b32 mouse_covered(const gui_t *gui)
{
	return gui->mouse_covered_by_widget_id != ~0
	    && !mouse__covered_by_current_popup(gui);
}

void mouse_cover(gui_t *gui, u64 widget_id)
{
	assert(!mouse_covered(gui));
	gui->mouse_covered_by_widget_id = widget_id;
}

b32 mouse_over_bg(const gui_t *gui)
{
	return gui->active_id == 0
	    && gui->active_id_at_frame_start == 0
	    && !mouse_covered(gui);
}

b32 mouse_double(const gui_t *gui, u32 mask)
{
	const gui_mouse_press_t *history = gui->mouse_press_history;
	return (history[1].btn & mask)
	    &&   time_diff_milli(history[1].time, history[0].time)
	       < GUI_MOUSE_DOUBLE_CLICK_INTERVAL
	    && (   !(history[2].btn & mask)
	        ||   time_diff_milli(history[2].time, history[1].time)
	           > GUI_MOUSE_DOUBLE_CLICK_INTERVAL);
}

b32 mouse_scroll(const gui_t *gui, s32 *dir)
{
	if (gui->mouse_btn & MB_WHEELUP) {
		*dir = 1;
		return true;
	} else if (gui->mouse_btn & MB_WHEELDOWN) {
		*dir = -1;
		return true;
	} else {
		*dir = 0;
		return false;
	}
}

b32 mouse_scroll_bg(const gui_t *gui, s32 *dir)
{
	if (mouse_over_bg(gui)) {
		return mouse_scroll(gui, dir);
	} else {
		*dir = 0;
		return false;
	}
}

void mouse_press_debug(gui_t *gui, b32 enabled)
{
	gui->mouse_debug = enabled;
}

b32 key_down(const gui_t *gui, gui_key_t key)
{
	return gui->keys[key];
}

b32 key_down_any(const gui_t *gui)
{
	for (u32 i = 0; i < KB_COUNT; ++i)
		if (gui->keys[i])
			return true;
	return false;
}

b32 key_pressed(const gui_t *gui, gui_key_t key)
{
	return gui->keys[key] && !gui->prev_keys[key];
}

b32 key_pressed_any(const gui_t *gui)
{
	for (u32 i = 0; i < KB_COUNT; ++i)
		if (gui->keys[i] && !gui->prev_keys[i])
			return true;
	return false;
}

b32 key_released(const gui_t *gui, gui_key_t key)
{
	return !gui->keys[key] && gui->prev_keys[key];
}

b32 key_released_any(const gui_t *gui)
{
	for (u32 i = 0; i < KB_COUNT; ++i)
		if (!gui->keys[i] && gui->prev_keys[i])
			return true;
	return false;
}

b32 key_mod(const gui_t *gui, gui_key_mod_t mod)
{
	return key_down(gui, KB_LCTRL + mod) || key_down(gui, KB_RCTRL + mod);
}

b32 key_toggled(const gui_t *gui, gui_key_toggle_t toggle)
{
	return gui->key_toggles[toggle] != GUI__KBT_OFF;
}

const u8 *keyboard_state(const gui_t *gui)
{
	return gui->keys;
}


/* Primitives */

static
void gui__line_wide(gui_t *gui, r32 x0, r32 y0, r32 x1, r32 y1,
                    r32 w, color_t c)
{
	v2f poly[4] = {
		{ x0, y0 },
		{ x0, y0 },
		{ x1, y1 },
		{ x1, y1 }
	};
	v2f dir = v2f_scale(v2f_dir(poly[0], poly[2]), w / 2.f);
	v2f perp = v2f_lperp(dir);

	poly[0] = v2f_sub(v2f_sub(poly[0], dir), perp);
	poly[1] = v2f_add(v2f_sub(poly[1], dir), perp);
	poly[2] = v2f_add(v2f_add(poly[2], dir), perp);
	poly[3] = v2f_sub(v2f_add(poly[3], dir), perp);

	gui__poly(gui, poly, 4, GUI_DRAW_TRIANGLE_FAN, c, g_nocolor, false);
}

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c)
{
	assert(w >= 1);
	if (w == 1) {
		/* NOTE(rgriege): fixes blurry lines due to anti-aliasing problem */
		const v2f poly[2] = {
			{ x0 + 0.5f, y0 + 0.5f },
			{ x1 + 0.5f, y1 + 0.5f }
		};
		gui__poly(gui, poly, 2, GUI_DRAW_TRIANGLE_FAN, g_nocolor, c, false);
	} else {
		gui__line_wide(gui, x0, y0, x1, y1, w, c);
	}
}

void gui_linef(gui_t *gui, r32 x0, r32 y0, r32 x1, r32 y1, r32 w, color_t c)
{
	assert(w >= 1);
	if (fabsf(w - 1) < 0.01f) {
		const v2f poly[2] = { { x0, y0 }, { x1, y1 } };
		gui__poly(gui, poly, 2, GUI_DRAW_TRIANGLE_FAN, g_nocolor, c, false);
	} else {
		gui__line_wide(gui, x0, y0, x1, y1, w, c);
	}
}

void gui_tri(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
             color_t fill, color_t stroke)
{
	const v2f poly[3] = {
		{ x0, y0 },
		{ x1, y1 },
		{ x2, y2 },
	};
	gui__poly(gui, poly, 3, GUI_DRAW_TRIANGLE_FAN, fill, stroke, true);
}

void gui_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t fill, color_t stroke)
{
	v2f poly[4] = {
		{ x,     y },
		{ x + w, y },
		{ x + w, y + h },
		{ x,     y + h },
	};
	gui__poly(gui, poly, 4, GUI_DRAW_TRIANGLE_FAN, fill, stroke, true);
}

void gui_rect_mcolor(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     color_t bl, color_t br, color_t tr, color_t tl)
{
	const v2i vbl = { x,     y };
	const v2i vbr = { x + w, y };
	const v2i vtr = { x + w, y + h };
	const v2i vtl = { x,     y + h };
	const box2i rect = { .min = vbl, .max = vtr };

	box2i_extend_box(&gui->widget_bounds->children, rect);

	if (!gui__box_visible(gui, rect)) {
		gui->culled_draw_calls += 1;
		gui->culled_vertices   += 4;
		return;
	}

	if (gui_begin(gui, 4, GUI_DRAW_TRIANGLE_FAN)) {
		gui__vertf(gui, vbl.x, vbl.y, bl, 0.f, 0.f);
		gui__vertf(gui, vbr.x, vbr.y, br, 0.f, 0.f);
		gui__vertf(gui, vtr.x, vtr.y, tr, 0.f, 0.f);
		gui__vertf(gui, vtl.x, vtl.y, tl, 0.f, 0.f);
		gui_end(gui);
	}
}

void gui_arc(gui_t *gui, s32 x, s32 y, s32 r, r32 angle_start, r32 angle_end,
             color_t fill, color_t stroke)
{
	const u32 num_verts = arc_poly_sz(r, angle_start, angle_end);
	if (gui->vert_cnt + num_verts <= GUI_MAX_VERTS) {
		v2f *verts = &gui->verts[gui->vert_cnt];
		arc_to_poly(x, y, r, angle_start, angle_end, verts, num_verts, false);
		gui__poly(gui, verts, num_verts, GUI_DRAW_TRIANGLE_FAN, fill, stroke, false);
	}
}

void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t stroke)
{
	const u32 num_verts = arc_poly_sz(r, 0, fPI * 2.f);
	if (gui->vert_cnt + num_verts <= GUI_MAX_VERTS) {
		v2f *verts = &gui->verts[gui->vert_cnt];
		arc_to_poly(x, y, r, 0, fPI * 2.f, verts, num_verts, true);
		gui__poly(gui, verts, num_verts, GUI_DRAW_TRIANGLE_FAN, fill, stroke, true);
	}
}

void gui_trisf(gui_t *gui, const v2f *v, u32 n, color_t fill)
{
	gui__triangles(gui, v, n, fill);
}

void gui_poly(gui_t *gui, const v2i *v, u32 n, color_t fill, color_t stroke)
{
	if (gui->vert_cnt + n <= GUI_MAX_VERTS) {
		v2f *verts = &gui->verts[gui->vert_cnt];
		for (u32 i = 0; i < n; ++i) {
			verts[i].x = v[i].x;
			verts[i].y = v[i].y;
		}
		gui_polyf(gui, verts, n, fill, stroke);
	}
}

void gui_polyf(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t stroke)
{
	if (n == 3 || fill.a == 0 || polyf_is_convex(v, n)) {
		gui__poly(gui, v, n, GUI_DRAW_TRIANGLE_FAN, fill, stroke, true);
	} else {
		if (gui->vert_cnt + triangulate_reserve_sz(n) <= GUI_MAX_VERTS) {
			v2f *verts = &gui->verts[gui->vert_cnt];
			u32 n_verts = 0;
			if (triangulate(v, n, verts, &n_verts))
				gui__triangles(gui, verts, n_verts, fill);
		}
		if (stroke.a != 0)
			gui__poly(gui, v, n, GUI_DRAW_TRIANGLE_FAN, g_nocolor, stroke, true);
	}
}

static
void gui__polyline_corner_offset(v2f a, v2f b, v2f c, r32 d, v2f *p)
{
	const v2f dir1 = v2f_dir(a, b);
	const v2f dir2 = v2f_dir(b, c);
	const v2f perp1 = v2f_scale(v2f_lperp(dir1), d);
	const v2f perp2 = v2f_scale(v2f_lperp(dir2), d);
	const v2f ax1 = v2f_add(a, perp1);
	const v2f bx1 = v2f_add(b, perp1);
	const v2f bx2 = v2f_add(b, perp2);
	const v2f cx2 = v2f_add(c, perp2);
	if (!fmath_line_intersect(ax1, bx1, bx2, cx2, p))
		*p = v2f_add(b, perp1);
	/* hacky fix for spikes created by very small angles.
	 * clamps to the corner offset of a ~10 degree angle. */
	else if (v2f_dist(*p, b) > d * 6.f)
		*p = v2f_fmadd(b, v2f_bisect(perp1, perp2), d * 6.f);
}

void gui_polyline(gui_t *gui, const v2i *v, u32 n, color_t stroke)
{
	if (gui->vert_cnt + n <= GUI_MAX_VERTS) {
		v2f *verts = &gui->verts[gui->vert_cnt];
		for (u32 i = 0; i < n; ++i) {
			verts[i].x = v[i].x;
			verts[i].y = v[i].y;
		}
		gui__poly(gui, verts, n, GUI_DRAW_TRIANGLE_FAN, g_nocolor, stroke, false);
	}
}

void gui_polylinef(gui_t *gui, const v2f *v, u32 n, r32 w, color_t stroke)
{
	assert(n >= 2 && w >= 1.f);
	if (w == 1.f) {
		gui__poly(gui, v, n, GUI_DRAW_TRIANGLE_FAN, g_nocolor, stroke, false);
	} else if (gui->vert_cnt + 2*n <= GUI_MAX_VERTS) {
		const r32 w2    = w / 2.f;
		const v2f dir0  = v2f_scale(v2f_dir(v[0], v[1]), w2);
		const v2f perp0 = v2f_lperp(dir0);
		const v2f dirn  = v2f_scale(v2f_dir(v[n-2], v[n-1]), w2);
		const v2f perpn = v2f_lperp(dirn);
		v2f *verts = &gui->verts[gui->vert_cnt];

		verts[0] = v2f_sub(v2f_sub(v[0], dir0), perp0);
		verts[1] = v2f_add(v2f_sub(v[0], dir0), perp0);

		for (u32 i = 1; i < n-1; ++i) {
			gui__polyline_corner_offset(v[i-1], v[i], v[i+1], w2, &verts[2*i]);
			verts[2*i+1] = v2f_add(v[i], v2f_sub(v[i], verts[2*i]));
		}

		verts[2*n-2] = v2f_sub(v2f_add(v[n-1], dirn), perpn);
		verts[2*n-1] = v2f_add(v2f_add(v[n-1], dirn), perpn);

		gui__poly(gui, verts, 2*n, GUI_DRAW_TRIANGLE_STRIP, stroke, g_nocolor, true);
	}
}

void gui_img(gui_t *gui, s32 x, s32 y, const gui_img_t *img)
{
	gui_img_ex(gui, x, y, img, 1.f, 1.f, 0.f, 1.f);
}

void gui_img_ex(gui_t *gui, s32 x, s32 y, const gui_img_t *img, r32 sx, r32 sy,
                r32 rotation, r32 opacity)
{
	if (!img) {
		assert(false);
		return;
	}

	color_t color = g_white;
	color.a = opacity * 255;
	gui__texture(gui, img, x, y, sx, sy, rotation, color);
}

void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const gui_img_t *img,
                   gui_img_scale_e scale)
{
	if (!img) {
		assert(false);
		return;
	}

	switch (scale) {
	case GUI_IMG_CENTERED:;
		const s32 x_off = (w - (s32)img->width) / 2;
		const s32 y_off = (h - (s32)img->height) / 2;
		if (x_off >= 0 && y_off >= 0) {
			gui_img_ex(gui, x + x_off, y + y_off, img, 1.f, 1.f, 0.f, 1.f);
			break;
		}
	case GUI_IMG_SCALED:;
		const r32 sx = (r32)w / img->width;
		const r32 sy = (r32)h / img->height;
		gui_img_ex(gui, x, y, img, sx, sy, 0.f, 1.f);
	break;
	}
}

static
void gui__add_codepoint_to_line_width(const gui_t *gui, void *font, s32 cp, r32 *line_width)
{
	gui_char_quad_t q;
	if (gui->fonts.get_char_quad(font, cp, *line_width, 0, &q))
		*line_width += q.advance;
}

u32 gui_wrap_txt(const gui_t *gui, char *txt, s32 padding_, s32 size_, r32 max_width)
{
	char *p = txt;
	char *pnext;
	const s32 padding = gui_scale_val(gui, padding_);
	const s32 size = gui_scale_val(gui, size_);
	s32 cp = utf8_next_codepoint(p, &pnext);
	r32 line_width = padding;
	u32 num_lines = 1;

	void *font = gui->fonts.get_font(gui->fonts.handle, size);
	if (!font)
		return 0;

	while (cp != 0) {
		char *p_space_before_word = NULL;
		r32 line_width_before_word;

		if (cp == ' ') {
			p_space_before_word = p;
			gui__add_codepoint_to_line_width(gui, font, cp, &line_width);
			p = pnext;
			cp = utf8_next_codepoint(p, &pnext);
		} else if (cp == '\n') {
			p = pnext;
			cp = utf8_next_codepoint(p, &pnext);
			line_width = padding;
		}
		line_width_before_word = line_width;

		while (cp != ' ' && cp != '\n' && cp != 0) {
			gui__add_codepoint_to_line_width(gui, font, cp, &line_width);
			p = pnext;
			cp = utf8_next_codepoint(p, &pnext);
		}
		if (cp == '\n')
			num_lines++;

		if (line_width >= max_width) {
			if (p_space_before_word) {
				*p_space_before_word = '\n';
				num_lines++;
			}
			line_width = padding + line_width - line_width_before_word;
		}
	}
	return num_lines;
}

static
void gui__txt_get_cursor_pos(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             const char *txt, u32 cursor,
                             const gui_text_style_t *style,
                             s32 *cx, s32 *cy)
{
	void *font;
	gui_font_metrics_t font_metrics;
	v2i anchor;
	v2f pos;
	gui_char_quad_t q;
	str_t wrapped = NULL;
	const s32 size = gui_scale_val(gui, style->size);
	const char *display = txt;
	const char *p, *target;
	char *pnext;
	s32 cp;

	font = gui->fonts.get_font(gui->fonts.handle, size);
	if (!font)
		return;

	gui->fonts.get_metrics(font, &font_metrics);

	if (style->wrap) {
		wrapped = str_dup(txt, g_temp_allocator);
		gui_wrap_txt(gui, wrapped, style->padding, style->size, w);
		display = wrapped;
	}

	gui_align_anchor(x, y, w, h, style->align, &anchor.x, &anchor.y);
	pos = gui__txt_start_pos(gui, display, anchor, style);

	if (cursor == 0)
		goto out;

	p = display;
	target = &display[cursor];
	while (p < target && (cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (cp == '\n') {
			pos.y -= font_metrics.newline_dist;
			pos.x = anchor.x + gui__txt_line_offset_x(gui, pnext, style);
		} else if (gui->fonts.get_char_quad(font, cp, pos.x, pos.y, &q)) {
			pos.x += q.advance;
		}
		p = pnext;
	}

out:
	*cx = pos.x;
	*cy = pos.y;
	if (wrapped)
		str_destroy(&wrapped);
}

static
void gui__txt_sequence(gui_t *gui, s32 x_anchor, r32 x0, r32 y0,
                       const char *txt, u32 max_len,
                       const gui_text_style_t *style,
                       r32 *x1, r32 *y1)
{
	void *font;
	gui_font_metrics_t font_metrics;
	r32 x = x0, y = y0;
	gui_char_quad_t q;
	const s32 size = gui_scale_val(gui, style->size);
	const char *p = txt;
	char *pnext;
	s32 cp;

	font = gui->fonts.get_font(gui->fonts.handle, size);
	if (!font)
		goto out;

	gui->fonts.get_metrics(font, &font_metrics);

	while (p - txt < max_len && (cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (cp == '\n') {
			y -= font_metrics.newline_dist;
			x = x_anchor + gui__txt_line_offset_x(gui, pnext, style);
		} else if (gui->fonts.get_char_quad(font, cp, x, y, &q)) {
			gui__char(gui, &q, style->color);
			x += q.advance;
		}
		p = pnext;
	}
out:
	*x1 = x;
	*y1 = y;
}

static
void gui__txt(gui_t *gui, s32 x0, s32 y0, const char *txt,
              const gui_text_style_t *style)
{
	const v2i anchor = { x0, y0 };
	const v2f pos = gui__txt_start_pos(gui, txt, anchor, style);
	r32 x1, y1;
	gui__txt_sequence(gui, anchor.x, pos.x, pos.y, txt, ~0, style, &x1, &y1);
}

static
u32 gui__txt_get_cursor_at_pos(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                               const char *txt, v2i mouse,
                               const gui_text_style_t *style)
{
	void *font;
	gui_font_metrics_t font_metrics;
	v2i anchor;
	v2f pos;
	gui_char_quad_t q;
	u32 closest_pos;
	s32 closest_dist, dist;
	v2i pp;
	str_t wrapped = NULL;
	const s32 size = gui_scale_val(gui, style->size);
	const char *display = txt;
	const char *p;
	char *pnext;
	s32 cp;

	if (style->wrap) {
		wrapped = str_dup(txt, g_temp_allocator);
		gui_wrap_txt(gui, wrapped, style->padding, style->size, w);
		display = wrapped;
	}

	font = gui->fonts.get_font(gui->fonts.handle, size);
	if (!font)
		return 0;

	gui->fonts.get_metrics(font, &font_metrics);

	gui_align_anchor(x, y, w, h, style->align, &anchor.x, &anchor.y);
	pos = gui__txt_start_pos(gui, display, anchor, style);

	v2i_set(&pp, pos.x, pos.y + font_metrics.ascent / 2);
	closest_pos = 0;
	closest_dist = v2i_dist_sq(pp, mouse);

	p = display;
	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		p = pnext;
		if (cp == '\n') {
			pos.y -= font_metrics.newline_dist;
			pos.x = anchor.x + gui__txt_line_offset_x(gui, p, style);
		} else if (gui->fonts.get_char_quad(font, cp, pos.x, pos.y, &q)) {
			pos.x += q.advance;
		} else {
			continue;
		}
		v2i_set(&pp, roundf(pos.x), roundf(pos.y + font_metrics.ascent / 2));
		dist = v2i_dist_sq(pp, mouse);
		if (dist < closest_dist) {
			closest_dist = dist;
			closest_pos = (u32)(p - display);
		}
	}
	if (wrapped)
		str_destroy(&wrapped);
	return closest_pos;
}

void gui_txt(gui_t *gui, s32 x, s32 y, s32 size, const char *txt,
             color_t c, gui_align_e align)
{
	const gui_text_style_t style = {
		.size = size,
		.color = c,
		.align = align,
		.padding = 0,
		.wrap = false,
	};
	gui__txt(gui, x, y, txt, &style);
}

void gui_txt_dim(const gui_t *gui, s32 x, s32 y, s32 size_, const char *txt,
                 gui_align_e align, s32 *px, s32 *py, s32 *pw, s32 *ph)
{
	const v2i anchor = { x, y };
	const char *p = txt;
	char *pnext;
	s32 cp;
	void *font;
	v2f pos;
	ivalf x_range, y_range;
	gui_char_quad_t q;
	const s32 size = gui_scale_val(gui, size_);
	const gui_text_style_t style = {
		.size = size_,
		.color = g_nocolor,
		.align = align,
		.padding = 0,
		.wrap = false,
	};
	gui_font_metrics_t font_metrics;

	font = gui->fonts.get_font(gui->fonts.handle, size);
	if (!font) {
		*px = 0;
		*py = 0;
		*pw = 0;
		*ph = 0;
		return;
	}

	gui->fonts.get_metrics(font, &font_metrics);

	pos = gui__txt_start_pos(gui, txt, anchor, &style);

	x_range.l = x_range.r = pos.x;
	y_range.l = y_range.r = pos.y;

	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (cp == '\n') {
			pos.y -= font_metrics.newline_dist;
			pos.x = anchor.x + gui__txt_line_offset_x(gui, pnext, &style);
		} else if (gui->fonts.get_char_quad(font, cp, pos.x, pos.y, &q)) {
			x_range.l = min(x_range.l, q.x0);
			x_range.r = max(x_range.r, q.x1);
			y_range.l = min(y_range.l, q.y0);
			y_range.r = max(y_range.r, q.y1);
			pos.x += q.advance;
		}
		p = pnext;
	}
	*px = x_range.l;
	*py = y_range.l;
	*pw = ivalf_length(x_range);
	*ph = ivalf_length(y_range);
}

s32 gui_txt_width(const gui_t *gui, const char *txt, s32 size)
{
	const char *p = txt;
	char *pnext;
	s32 width = 0;
	s32 line_width;
	s32 cp;

	while (*p != 0) {
		line_width = gui__txt_line_width(gui, p, size);
		width = max(width, line_width);
		while ((cp = utf8_next_codepoint(p, &pnext)) != 0 && cp != '\n')
			p = pnext;
		if (cp == '\n')
			p = pnext;
	}
	return width;
}

static
void gui__layer_init(gui_t *gui, gui_layer_t *layer, s32 x, s32 y, s32 w, s32 h)
{
	layer->draw_call_idx = gui->draw_call_cnt;
	layer->x = x;
	layer->y = y;
	layer->w = w;
	layer->h = h;
	layer->pri = 0;
}

static
void gui__layer_complete_current(gui_t *gui)
{
	gui->layer->draw_call_cnt = gui->draw_call_cnt - gui->layer->draw_call_idx;
}

static
void gui__layer_new(gui_t *gui)
{
	const v2i pos = gui->mask->min;
	const v2i dim = box2i_get_extent(*gui->mask);

	gui__layer_complete_current(gui);

	if (gui->layer + 1 < gui->layers + countof(gui->layers)) {
		++gui->layer;
		gui__layer_init(gui, gui->layer, pos.x, pos.y, dim.x, dim.y);
	} else {
		assert(0);
	}
}

void gui_mask_push(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	if (gui->mask + 1 < gui->masks + countof(gui->masks)) {
		++gui->mask;
		box2i_from_xywh(gui->mask, x, y, w, h);
	} else {
		assert(0);
	}
	gui__layer_new(gui);
}

static
void gui_mask_push_box(gui_t *gui, box2i b)
{
	gui_mask_push(gui, b.min.x, b.min.y, b.max.x - b.min.x, b.max.y - b.min.y);
}

void gui_mask_pop(gui_t *gui)
{
	if (gui->mask > gui->masks) {
		--gui->mask;
	} else {
		assert(0);
		box2i_from_xywh(gui->mask, 0, 0, gui->window_dim.x, gui->window_dim.y);
	}

	gui__layer_new(gui);
}

static
int gui__layer_sort(const void *lhs_, const void *rhs_)
{
	const gui_layer_t *lhs = lhs_, *rhs = rhs_;
	return rhs->pri - lhs->pri;
}


void gui_line_styled(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                     const gui_line_style_t *style)
{
	gui_line(gui, x0, y0, x1, y1, style->thickness, style->color);
}

void gui_txt_styled(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, const gui_text_style_t *style)
{
	const u32 len = (u32)strlen(txt);
	array(char) buf;

	gui_align_anchor(x, y, w, h, style->align, &x, &y);

	if (style->wrap) {
		array_init_ex(buf, len + 1, g_temp_allocator);
		memcpy(buf, txt, len + 1);
		gui_wrap_txt(gui, buf, style->padding, style->size, w);
		gui__txt(gui, x, y, buf, style);
		array_destroy(buf);
	} else {
		gui__txt(gui, x, y, txt, style);
	}
}

/* Widgets */

static
b32 gui__allow_new_panel_interaction(const gui_t *gui)
{
	return (gui->hot_id == 0 || !gui->hot_id_found_this_frame)
	    && gui->active_id == 0
	    && !mouse_covered(gui);
}

static
b32 gui__allow_new_interaction(const gui_t *gui)
{
	return (gui->hot_id == 0 || !gui->hot_id_found_this_frame)
	    && gui->active_id == 0
	    && !mouse_covered(gui)
	    && !mouse_down(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT);
}

typedef enum gui__widget_render_state
{
	GUI__WIDGET_RENDER_INACTIVE,
	GUI__WIDGET_RENDER_HOT,
	GUI__WIDGET_RENDER_ACTIVE,
} gui__widget_render_state_e;

static
gui__widget_render_state_e gui__widget_render_state(const gui_t *gui, u64 id,
                                                    b32 triggered,
                                                    b32 checked,
                                                    b32 contains_mouse)
{
	if (gui->active_id == id)
		return contains_mouse ? GUI__WIDGET_RENDER_ACTIVE : GUI__WIDGET_RENDER_HOT;
	else if (gui_widget_focused(gui, id))
		return triggered ? GUI__WIDGET_RENDER_ACTIVE : GUI__WIDGET_RENDER_HOT;
	else if (gui->hot_id == id)
		return GUI__WIDGET_RENDER_HOT;
	else
		return checked ? GUI__WIDGET_RENDER_ACTIVE : GUI__WIDGET_RENDER_INACTIVE;
}

static
gui__widget_render_state_e gui__btn_render_state(const gui_t *gui, u64 id,
                                                 gui_btn_e val, b32 contains_mouse)
{
	return gui__widget_render_state(gui, id, val != GUI_BTN_NONE, false, contains_mouse);
}

static
gui_element_style_t gui__element_style(const gui_t *gui,
                                       gui__widget_render_state_e render_state,
                                       const gui_widget_style_t *widget_style)
{
	gui_element_style_t style;

	switch (render_state) {
	case GUI__WIDGET_RENDER_ACTIVE:
		style = widget_style->active;
	break;
	case GUI__WIDGET_RENDER_HOT:
		style = widget_style->hot;
	break;
	case GUI__WIDGET_RENDER_INACTIVE:
		style = widget_style->inactive;
	break;
	}

	if (gui->lock) {
		style.line.color = color_blend(widget_style->disabled.line.color,
		                               style.line.color);
		style.text.color = color_blend(widget_style->disabled.text.color,
		                               style.text.color);
		style.bg_color = color_blend(widget_style->disabled.bg_color,
		                             style.bg_color);
		style.outline_color = color_blend(widget_style->disabled.outline_color,
		                                  style.outline_color);
	}

	return style;
}

void gui_pen_null(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const gui_element_style_t *style)
{
}

void gui_pen_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const gui_element_style_t *style)
{
	gui_rect(gui, x, y, w, h, style->bg_color, style->outline_color);
}

void gui_pen_circ(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const gui_element_style_t *style)
{
	gui_circ(gui, x + w / 2, y + h / 2, min(w / 2, h / 2), style->bg_color,
	         style->outline_color);
}

u32 gui_num_layers(const gui_t *gui)
{
	return (u32)(gui->layer - &gui->layers[0] + 1);
}

u32 gui_vert_cnt(const gui_t *gui)
{
	return gui->vert_cnt;
}

u32 gui_draw_call_cnt(const gui_t *gui)
{
	return gui->draw_call_cnt;
}

u32 gui_culled_vert_cnt(const gui_t *gui)
{
	return gui->culled_vertices;
}

u32 gui_culled_draw_call_cnt(const gui_t *gui)
{
	return gui->culled_draw_calls;
}

u32 gui_culled_widget_cnt(const gui_t *gui)
{
	return gui->culled_widgets;
}

void gui_get_render_output(const gui_t *gui, gui_render_output_t *output)
{
	output->num_layers = gui_num_layers(gui);
	output->layers = gui->layers;

	output->num_verts = gui->vert_cnt;
	output->verts.pos = gui->verts;
	output->verts.color = gui->vert_colors;
	output->verts.tex_coord = gui->vert_tex_coords;

	output->num_draw_calls = gui->draw_call_cnt;
	output->draw_calls = gui->draw_calls;
}

const gui_npt_filter_t g_gui_npt_filter_print = {
	.ascii = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	},
	.non_ascii = 1,
};

const gui_npt_filter_t g_gui_npt_filter_numeric = {
	.ascii = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	},
	.non_ascii = 0,
};

const gui_npt_filter_t g_gui_npt_filter_hex = {
	.ascii = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
		0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
		0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
	},
	.non_ascii = 0,
};

static
void gui__npt_selection_moved(gui_t *gui)
{
	if (!key_mod(gui, KBM_SHIFT))
		gui->npt.cursor = gui->npt.selection;
}

static
void gui__npt_move_cursor_left(gui_t *gui, const char *txt)
{
	if (gui->npt.selection > 0) {
		const char *cursor = &txt[gui->npt.selection];
		char *prev;
		utf8_prev_codepoint(cursor, &prev);
		gui->npt.selection -= (cursor - prev);
		gui__npt_selection_moved(gui);
	}
}

static
void gui__npt_move_cursor_right(gui_t *gui, const char *txt)
{
	const char *cursor = &txt[gui->npt.selection];
	char *next;
	if (utf8_next_codepoint(cursor, &next) != 0) {
		gui->npt.selection += (next - cursor);
		gui__npt_selection_moved(gui);
	}
}

static
void gui__npt_move_cursor_vertical(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                                   const char *txt, s32 diff, u32 fallback)
{
	const u32 orig_pos = gui->npt.selection;
	const gui_text_style_t *style = &gui->style.npt.active.text;
	v2i cursor;

	gui__txt_get_cursor_pos(gui, x, y, w, h, txt, orig_pos, style, &cursor.x, &cursor.y);
	cursor.y += diff;

	gui->npt.selection = gui__txt_get_cursor_at_pos(gui, x, y, w, h, txt, cursor, style);
	if (gui->npt.selection == orig_pos && orig_pos != fallback)
		gui->npt.selection = fallback;

	gui__npt_selection_moved(gui);
}

static
b32 gui__key_up(const gui_t *gui)
{
#ifndef __APPLE__
	return gui->key_repeat.val == KB_UP
	    || (gui->key_repeat.val == KB_KP_8 && !key_toggled(gui, KBT_NUM));
#else
	return gui->key_repeat.val == KB_UP;
#endif
}

static
b32 gui__key_down(const gui_t *gui)
{
#ifndef __APPLE__
	return gui->key_repeat.val == KB_DOWN
	    || (gui->key_repeat.val == KB_KP_2 && !key_toggled(gui, KBT_NUM));
#else
	return gui->key_repeat.val == KB_DOWN;
#endif
}

static
b32 gui__key_left(const gui_t *gui)
{
#ifndef __APPLE__
	return gui->key_repeat.val == KB_LEFT
	    || (gui->key_repeat.val == KB_KP_4 && !key_toggled(gui, KBT_NUM));
#else
	return gui->key_repeat.val == KB_LEFT;
#endif
}

static
b32 gui__key_right(const gui_t *gui)
{
#ifndef __APPLE__
	return gui->key_repeat.val == KB_RIGHT
	    || (gui->key_repeat.val == KB_KP_6 && !key_toggled(gui, KBT_NUM));
#else
	return gui->key_repeat.val == KB_RIGHT;
#endif
}

static
b32 gui__key_home(const gui_t *gui)
{
#ifndef __APPLE__
	return gui->key_repeat.val == KB_HOME
	    || (gui->key_repeat.val == KB_KP_7 && !key_toggled(gui, KBT_NUM));
#else
	return gui->key_repeat.val == KB_HOME;
#endif
}

static
b32 gui__key_end(const gui_t *gui)
{
#ifndef __APPLE__
	return gui->key_repeat.val == KB_END
	    || (gui->key_repeat.val == KB_KP_1 && !key_toggled(gui, KBT_NUM));
#else
	return gui->key_repeat.val == KB_END;
#endif
}

static
b32 gui__widget_contains_mouse(const gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	return box2i_contains_point(box, gui->mouse_pos)
	    && gui__box_half_visible(gui, box)
	    && !gui->lock
	    && !mouse_covered(gui);
}

static
void gui__widget_handle_focus(gui_t *gui, u64 id, b32 mouse_pos_can_defocus)
{
	if (gui_widget_focused(gui, id)) {
		const b32 most_focused = gui__is_most_focused_widget(gui, id);
		if (gui->lock) {
			gui__defocus_widget(gui, id);
		} else if (gui__key_triggered(gui, KB_TAB) && most_focused) {
			/* NOTE(rgriege): tab focusing skips widgets in popups/menus/dropdowns.
			 * Initially I implemented it this way because it was easier and not
			 * because it was the best behavior. After using it, I'm not so sure.
			 *
			 * However, if you manually focus a widget inside a popup, then tab
			 * will focus other widgets in the popup. */
			gui__tab_focus_adjacent_widget(gui);
		} else if (gui__key_triggered(gui, KB_ESCAPE) && most_focused) {
			gui__defocus_widget(gui, id);
		} else {
			gui->focus_id_found_this_frame = true;
			if (mouse_pressed(gui, ~0) && mouse_pos_can_defocus)
				gui__defocus_widget(gui, id);
		}
	} else if (gui->focus_next_widget && !gui->lock) {
		gui__on_widget_tab_focused(gui, id);
	} else if (gui->focus_prev_widget_id == id) {
		if (gui->lock)
			gui->focus_prev_widget_id = gui->prev_widget_id;
		else
			gui__on_widget_tab_focused(gui, id);
	}
}

static
b32 gui__npt_get_selection(const gui_t *gui, u32 *beg, u32 *end)
{
	if (gui->npt.cursor < gui->npt.selection) {
		*beg = gui->npt.cursor;
		*end = gui->npt.selection;
		return true;
	} else if (gui->npt.selection < gui->npt.cursor) {
		*beg = gui->npt.selection;
		*end = gui->npt.cursor;
		return true;
	} else {
		return false;
	}
}

static
b32 gui__npt_has_selection(const gui_t *gui)
{
	u32 beg, end;
	return gui__npt_get_selection(gui, &beg, &end);
}

static
void gui__npt_select_all(gui_t *gui, const char *txt)
{
	gui->npt.cursor    = 0;
	gui->npt.selection = (u32)strlen(txt);
}

static
void gui__npt_remove_selected(gui_t *gui, char *txt, u32 *len)
{
	u32 beg, end;
	if (gui__npt_get_selection(gui, &beg, &end)) {
		const u32 n = end - beg;
		buf_remove_n(txt, beg, n, *len+1);
		*len -= n;
		gui->npt.cursor = gui->npt.selection = beg;
	}
}

static
void gui__hint_render(gui_t *gui, u64 id, const char *hint)
{
	if (id == gui->hot_id && gui->hint_timer <= 0 && hint) {
		const gui_element_style_t *style = &gui->style.hint;
		const s32 x = gui->mouse_pos.x;
		const s32 y = gui->mouse_pos.y;
		const s32 padding = gui_scale_val(gui, style->text.padding);
		s32 rx, ry, rw, rh;
		gui_widget_bounds_t bounds = {0};
		gui_txt_dim(gui, x, y, style->text.size, hint,
		            GUI_ALIGN_BOTLEFT, &rx, &ry, &rw, &rh);
		rx = (x + rw < gui->window_dim.x) ? x : x - rw;
		ry = (y + rh < gui->window_dim.y) ? y : y - rh;
		rw += 2 * padding;
		rh += 2 * padding;
		gui_mask_push(gui, rx, ry, rw, rh);
		box2i_from_xywh(&bounds.bbox, rx, ry, rw, rh);
		gui_widget_bounds_push(gui, &bounds, false);
		gui->layer->pri = GUI__LAYER_PRIORITY_HINT;
		gui_rect(gui, rx, ry, rw, rh, style->bg_color, style->outline_color);
		gui_txt_styled(gui, rx, ry, rw, rh, hint, &style->text);
		gui_widget_bounds_pop(gui, &bounds, false);
		gui_mask_pop(gui);
	}
}

b32 gui_npt_filter(gui_npt_filter_p filter, s32 codepoint)
{
	if (codepoint < 0)
		return false;
	else if (codepoint < 127)
		return filter->ascii[codepoint];
	else if (codepoint == 127)
		return false;
	else
		return filter->non_ascii;
}

s32 gui_npt_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                const char *hint, gui_npt_flags_e flags)
{
	return gui_npt_txt_ex(gui, x, y, w, h, txt, n, hint, flags,
	                      &g_gui_npt_filter_print);
}

static
s32 gui__font_newline_dist(const gui_t *gui, void *font)
{
	if (font) {
		gui_font_metrics_t metrics;
		gui->fonts.get_metrics(font, &metrics);
		return metrics.newline_dist;
	} else {
		return 0;
	}
}

static
void gui__npt_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                  const char *txt, b32 active,
                  const gui_text_style_t *style, color_t bg)
{
	const s32 size = gui_scale_val(gui, style->size);
	void *font = gui->fonts.get_font(gui->fonts.handle, size);
	const char *display = txt;
	gui_font_metrics_t metrics;
	gui_text_style_t style_inverse = *style;
	str_t wrapped = NULL;
	u32 beg, end;
	v2i anchor;
	v2f pos;
	s32 cp;
	const char *p;
	char *pnext;
	r32 rx, ry, rh;
	b32 inside_selection;
	gui_char_quad_t q;
	const char *txt0, *txt1, *txt2;
	u32 len0, len1, len2;

	if (!active || !gui__npt_get_selection(gui, &beg, &end)) {
		gui_txt_styled(gui, x, y, w, h, txt, style);
		return;
	}

	if (!font)
		return;

	gui->fonts.get_metrics(font, &metrics);
	rh = metrics.ascent - metrics.descent;
	style_inverse.color = bg;

	if (style->wrap) {
		wrapped = str_dup(txt, g_temp_allocator);
		gui_wrap_txt(gui, wrapped, style->padding, style->size, w);
		display = wrapped;
	}

	/* render the selection background(s) */
	gui_align_anchor(x, y, w, h, style->align, &anchor.x, &anchor.y);
	pos = gui__txt_start_pos(gui, display, anchor, style);
	rx = pos.x;
	inside_selection = false;
	p = display;
	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (p - display == beg) {
			inside_selection = true;
			rx = pos.x;
		}
		if (p - display == end) {
			ry = pos.y + metrics.descent;
			gui_rect(gui, rx, ry, pos.x - rx, rh, style->color, g_nocolor);
			inside_selection = false;
		}
		if (cp == '\n') {
			if (inside_selection) {
				ry = pos.y + metrics.descent;
				gui_rect(gui, rx, ry, pos.x - rx, rh, style->color, g_nocolor);
			}
			pos.y -= metrics.newline_dist;
			pos.x = anchor.x + gui__txt_line_offset_x(gui, pnext, style);
			rx = pos.x;
		} else if (gui->fonts.get_char_quad(font, cp, pos.x, pos.y, &q)) {
			pos.x += q.advance;
		}
		p = pnext;
	}
	if (inside_selection) {
		ry = pos.y + metrics.descent;
		gui_rect(gui, rx, ry, pos.x - rx, rh, style->color, g_nocolor);
	}

	/* compute & render the text sequences */
	txt0 = display;
	len0 = beg;
	txt1 = &display[beg];
	len1 = end - beg;
	txt2 = &display[end];
	len2 = (u32)strlen(display) - end;

	pos = gui__txt_start_pos(gui, display, anchor, style);
	gui__txt_sequence(gui, anchor.x, pos.x, pos.y, txt0, len0, style, &pos.x, &pos.y);
	gui__txt_sequence(gui, anchor.x, pos.x, pos.y, txt1, len1, &style_inverse, &pos.x, &pos.y);
	gui__txt_sequence(gui, anchor.x, pos.x, pos.y, txt2, len2, style, &pos.x, &pos.y);

	if (wrapped)
		str_destroy(&wrapped);
}

static
gui_btn_e gui__btn_logic(gui_t *gui, u64 id, gui_mouse_button_e mb, b32 contains_mouse);

s32 gui_npt_txt_ex(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                   const char *hint, gui_npt_flags_e flags, gui_npt_filter_p filter)
{
	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_active = gui->active_id == id;
	const b32 was_focused = gui_widget_focused(gui, id);
	const gui_widget_style_t *style = &gui->style.npt;
	b32 contains_mouse;
	s32 complete = 0;
	gui__widget_render_state_e render_state;
	gui_element_style_t elem_style;
	const char *display_txt;

	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return 0;
	}

	contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	if (gui__btn_logic(gui, id, MB_LEFT, contains_mouse) == GUI_BTN_PRESS && !was_focused)
		gui__focus_widget(gui, id);

	/* mouse & tab-focus selection handling */
	if (gui->active_id == id && !was_active) {
		/* make sure to clear selection first */
		gui->npt.cursor = gui->npt.selection = 0;
	}
	if (contains_mouse) {
		if (gui->active_id == id) {
			const gui_text_style_t *txt_style = &style->active.text;
			if (mouse_pressed(gui, MB_LEFT)) {
				if (mouse_double(gui, MB_LEFT)) {
					gui__npt_select_all(gui, txt);
				} else {
					gui->npt.cursor = gui__txt_get_cursor_at_pos(gui, x, y, w, h, txt,
					                                             gui->mouse_pos, txt_style);
					gui->npt.selection = gui->npt.cursor;
				}
			} else if (mouse_down(gui, MB_LEFT) && !mouse_double(gui, MB_LEFT)) {
				gui->npt.selection = gui__txt_get_cursor_at_pos(gui, x, y, w, h, txt,
				                                                gui->mouse_pos, txt_style);
			}
		} else if (   was_active
		           && mouse_released(gui, MB_LEFT)
		           && !was_focused
		           && (flags & GUI_NPT_SELECT_ON_FOCUS)
		           && gui->npt.selection == gui->npt.cursor) {
				gui__npt_select_all(gui, txt);
		}
	}

	if (gui_widget_focused(gui, id) && !was_focused) {
		if (!was_active) {
			/* handle focus from tabbing */
			if (flags & GUI_NPT_SELECT_ON_FOCUS)
				gui__npt_select_all(gui, txt);
			else
				gui->npt.cursor = gui->npt.selection = 0;
		}
	}

	/* keyboard input */
	if (gui_widget_focused(gui, id)) {
		const s32 size = gui_scale_val(gui, style->active.text.size);
		void *font = gui->fonts.get_font(gui->fonts.handle, size);
		u32 len = (u32)strlen(txt);
		gui->npt.cursor    = clamp(0, gui->npt.cursor,    len);
		gui->npt.selection = clamp(0, gui->npt.selection, len);
		if (strlen(gui->text_npt) > 0 && !key_mod(gui, GUI__KBM_CLIPBOARD)) {
			gui__npt_remove_selected(gui, txt, &len);
			const char *p = gui->text_npt;
			char *pnext;
			s32 cp;
			u32 cp_sz;
			if (gui->text_input_exceeds_buffer_size)
				log_warn("text input exceeds buffer size");
			while (   (cp = utf8_next_codepoint(p, &pnext)) != 0
			       && (cp_sz = (u32)(pnext - p))
			       && len + cp_sz < n) {
				if (gui_npt_filter(filter, cp)) {
					/* move all bytes after the cursor (inc NUL) forward by cp_sz */
					for (u32 i = len + cp_sz; i > gui->npt.cursor + cp_sz - 1; --i)
						txt[i] = txt[i-cp_sz];
					memcpy(&txt[gui->npt.cursor], p, cp_sz);
					len += cp_sz;
					gui->npt.cursor += cp_sz;
					gui->npt.selection = gui->npt.cursor;
				}
				p = pnext;
			}
		} else if (gui->key_repeat.triggered) {
			const u32 key_idx = gui->key_repeat.val;
			if (key_idx == KB_BACKSPACE) {
				const b32 has_selection = gui__npt_has_selection(gui);
				gui__npt_remove_selected(gui, txt, &len);
				if (!has_selection && gui->npt.cursor > 0) {
					char *cursor = &txt[gui->npt.cursor], *prev;
					utf8_prev_codepoint(cursor, &prev);
					gui->npt.cursor -= (cursor - prev);
					gui->npt.selection = gui->npt.cursor;
					buf_remove_n(txt, gui->npt.cursor, cursor - prev, len+1);
				}
			} else if (key_idx == KB_DELETE) {
				const b32 has_selection = gui__npt_has_selection(gui);
				gui__npt_remove_selected(gui, txt, &len);
				char *cursor = &txt[gui->npt.cursor], *next;
				if (!has_selection && utf8_next_codepoint(cursor, &next) != 0)
					buf_remove_n(txt, gui->npt.cursor, next - cursor, len+1);
			} else if (key_idx == KB_RETURN || key_idx == KB_KP_ENTER) {
				gui->npt.selection = gui->npt.cursor;
				gui__npt_remove_selected(gui, txt, &len);
				gui__defocus_widget(gui, id);
				complete = GUI_NPT_COMPLETE_ON_ENTER;
			} else if (key_idx == KB_A && key_mod(gui, GUI__KBM_CLIPBOARD)) {
				gui__npt_select_all(gui, txt);
			} else if (key_idx == KB_C && key_mod(gui, GUI__KBM_CLIPBOARD)) {
				u32 beg, end;
				if (gui__npt_get_selection(gui, &beg, &end)) {
					const u32 sz = min(end - beg, sizeof(gui->clipboard_out) - 1);
					memcpy(gui->clipboard_out, &txt[beg], sz);
					gui->clipboard_out[sz] = 0;
				}
			} else if (key_idx == KB_X && key_mod(gui, GUI__KBM_CLIPBOARD)) {
				u32 beg, end;
				if (gui__npt_get_selection(gui, &beg, &end)) {
					const u32 sz = min(end - beg, sizeof(gui->clipboard_out) - 1);
					memcpy(gui->clipboard_out, &txt[beg], sz);
					gui->clipboard_out[sz] = 0;
					gui__npt_remove_selected(gui, txt, &len);
				}
			} else if (key_idx == KB_V && key_mod(gui, GUI__KBM_CLIPBOARD)) {
				u32 sz;
				gui__npt_remove_selected(gui, txt, &len);
				if ((sz = (u32)strlen(gui->clipboard_in)) > 0 && len + sz < n) {
					if (gui->clipboard_input_exceeds_buffer_size)
						log_warn("clipboard input exceeds buffer size");
					memmove(&txt[gui->npt.cursor + sz],
					        &txt[gui->npt.cursor],
					        len - gui->npt.cursor + 1);
					memcpy(&txt[gui->npt.cursor], gui->clipboard_in, sz);
					gui->npt.cursor += sz;
					gui->npt.selection = gui->npt.cursor;
				}
			} else if (gui__key_up(gui)) {
				const s32 dy = gui__font_newline_dist(gui, font);
				u32 beg, end;
				if (key_mod(gui, KBM_SHIFT) || !gui__npt_get_selection(gui, &beg, &end))
					gui__npt_move_cursor_vertical(gui, x, y, w, h, txt, dy, 0);
				else
					gui->npt.cursor = gui->npt.selection = beg;
			} else if (gui__key_down(gui)) {
				const s32 dy = -gui__font_newline_dist(gui, font);
				u32 beg, end;
				if (key_mod(gui, KBM_SHIFT) || !gui__npt_get_selection(gui, &beg, &end))
					gui__npt_move_cursor_vertical(gui, x, y, w, h, txt, dy, len);
				else
					gui->npt.cursor = gui->npt.selection = end;
			} else if (gui__key_left(gui)) {
				u32 beg, end;
				if (key_mod(gui, KBM_SHIFT) || !gui__npt_get_selection(gui, &beg, &end))
					gui__npt_move_cursor_left(gui, txt);
				else
					gui->npt.cursor = gui->npt.selection = beg;
			} else if (gui__key_right(gui)) {
				u32 beg, end;
				if (key_mod(gui, KBM_SHIFT) || !gui__npt_get_selection(gui, &beg, &end))
					gui__npt_move_cursor_right(gui, txt);
				else
					gui->npt.cursor = gui->npt.selection = end;
			} else if (gui__key_home(gui)) {
				gui->npt.selection = 0;
				gui__npt_selection_moved(gui);
			} else if (gui__key_end(gui)) {
				gui->npt.selection = len;
				gui__npt_selection_moved(gui);
			}
		}
	}

	if (gui_widget_focused(gui, id) && !was_focused)
		gui->npt.initial_txt_hash = hashn_compute(txt, n);
	if (gui_widget_focused(gui, id) != was_focused)
		gui->npt.active = gui_widget_focused(gui, id);

	/* completion state */
	if (!gui_widget_focused(gui, id) && was_focused) {
		if (complete || gui->lock) {
		} else if (   (flags & GUI_NPT_COMPLETE_ON_TAB)
		           && gui__key_triggered(gui, KB_TAB)) {
			complete = GUI_NPT_COMPLETE_ON_TAB;
		} else if (   (flags & GUI_NPT_COMPLETE_ON_CLICK_OUT)
		           && mouse_pressed(gui, ~0)
		           && !contains_mouse) {
			complete = GUI_NPT_COMPLETE_ON_CLICK_OUT;
		} else if (   (flags & GUI_NPT_COMPLETE_ON_ESCAPE)
		           && gui__key_triggered(gui, KB_ESCAPE)) {
			complete = GUI_NPT_COMPLETE_ON_ESCAPE;
		} else if (   (flags & GUI_NPT_COMPLETE_ON_UNCHANGED)
		           || gui->npt.initial_txt_hash != hashn_compute(txt, n)) {
			complete = GUI_NPT_COMPLETE_ON_UNCHANGED;
		}
	}

	/* rendering */
	render_state = gui__widget_render_state(gui, id, true, false,
	                                        contains_mouse || gui_widget_focused(gui, id));
	elem_style = gui__element_style(gui, render_state, style);
	style->pen(gui, x, y, w, h, &elem_style);
	display_txt = txt;

	if (flags & GUI_NPT_PASSWORD) {
		assert(strlen(display_txt) < GUI_TXT_MAX_LENGTH);
		const char *p = display_txt;
		char *pnext;
		u32 i = 0;
		while (i < GUI_TXT_MAX_LENGTH-1 && utf8_next_codepoint(p, &pnext) != 0) {
			gui->npt.pw_buf[i++] = '*';
			p = pnext;
		}
		gui->npt.pw_buf[i] = '\0';
		display_txt = gui->npt.pw_buf;
	}
	if (display_txt[0] != 0) {
		const b32 active = (id == gui->active_id || gui_widget_focused(gui, id));
		gui__npt_txt(gui, x, y, w, h, display_txt, active,
		             &elem_style.text, elem_style.bg_color);
	}
	if (gui_widget_focused(gui, id)) {
		if (   !gui__npt_has_selection(gui)
		    && time_diff_milli(gui->creation_time, gui->frame_start_time) % 1000 < 500) {
			const color_t color = elem_style.text.color;
			const s32 size = gui_scale_val(gui, elem_style.text.size);
			void *font = gui->fonts.get_font(gui->fonts.handle, size);
			if (font) {
				gui_font_metrics_t metrics;
				gui->fonts.get_metrics(font, &metrics);
				gui__txt_get_cursor_pos(gui, x, y, w, h, display_txt,
				                        gui->npt.cursor, &elem_style.text, &x, &y);
				x += 1;
				gui_line(gui, x, y + metrics.descent, x, y + metrics.ascent, 1, color);
			}
		}
	} else if (txt[0] == 0 && hint) {
		gui_txt_styled(gui, x, y, w, h, hint, &elem_style.text);
	}
	if (contains_mouse)
		gui->cursor = GUI_CURSOR_TEXT_INPUT;
	gui__hint_render(gui, id, gui->style.npt.hint);
	gui->prev_widget_id = id;
	return complete;
}

const char *gui_npt_val_buf(const gui_t *gui)
{
	return gui->npt.val_buf;
}

char *gui_npt_val_buf_mut(gui_t *gui)
{
	return gui->npt.val_buf;
}

s32 gui_npt_val(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, u32 n_,
                gui_npt_flags_e flags, gui_npt_filter_p filter)
{
	const u64 id = gui_widget_id(gui, x, y);
	const u32 n  = min(n_, sizeof(gui->npt.val_buf));
	if (gui_widget_focused(gui, id)) {
		return gui_npt_txt_ex(gui, x, y, w, h, gui->npt.val_buf, n,
		                      "", flags, filter);
	} else if (gui_any_widget_has_focus(gui)) {
		char buf[GUI_TXT_MAX_LENGTH];
		strbcpy(buf, txt);
		gui_npt_txt_ex(gui, x, y, w, h, buf, n, "", flags, filter);
		if (gui_widget_focused(gui, id))
			strbcpy(gui->npt.val_buf, txt);
		return 0;
	} else {
		strbcpy(gui->npt.val_buf, txt);
		return gui_npt_txt_ex(gui, x, y, w, h, gui->npt.val_buf, n,
		                      "", flags, filter);
	}
}

static
gui_btn_e gui__btn_logic_ex(gui_t *gui, u64 id, gui_mouse_button_e mb,
                            b32 mouse_pos_can_focus, b32 mouse_pos_can_defocus)
{
	gui_btn_e retval = GUI_BTN_NONE;

	gui__widget_handle_focus(gui, id, mouse_pos_can_defocus);

	if (   gui__is_most_focused_widget(gui, id)
	    && !gui->lock
	    && gui->key_repeat.triggered
	    && (   gui->key_repeat.val == KB_RETURN
	        || gui->key_repeat.val == KB_KP_ENTER))
		retval = key_pressed(gui, gui->key_repeat.val) ? GUI_BTN_PRESS : GUI_BTN_HOLD;

	if (gui->active_id == id) {
		gui->active_id_found_this_frame = true;
		if (mouse_released(gui, mb)) {
			if (mouse_pos_can_focus)
				retval = GUI_BTN_PRESS;
			gui->active_id = 0;
		} else if (mouse_pos_can_focus && gui->mouse_repeat.triggered) {
			retval = GUI_BTN_HOLD;
		}
	} else if (gui->hot_id == id) {
		if (mouse_pos_can_defocus || mouse_covered(gui)) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, mb)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->active_id_found_this_frame = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui__allow_new_interaction(gui) && mouse_pos_can_focus) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}
	gui->prev_widget_id = id;
	return retval;
}

static
gui_btn_e gui__btn_logic(gui_t *gui, u64 id, gui_mouse_button_e mb, b32 contains_mouse)
{
	return gui__btn_logic_ex(gui, id, mb, contains_mouse, !contains_mouse);
}

static
void gui__btn_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, gui__widget_render_state_e render_state,
                     b32 contains_mouse,
                     const gui_widget_style_t *widget_style)
{
	const gui_element_style_t style = gui__element_style(gui, render_state, widget_style);
	widget_style->pen(gui, x, y, w, h, &style);
	gui_txt_styled(gui, x, y, w, h, txt, &style.text);
	if (contains_mouse)
		gui->cursor = GUI_CURSOR_BUTTON;
}

s32 gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return 0;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const gui_btn_e ret = gui__btn_logic(gui, id, MB_LEFT, contains_mouse);

	const gui__widget_render_state_e render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, contains_mouse, &gui->style.btn);
	gui__hint_render(gui, id, gui->style.btn.hint);

	return ret;
}

s32 gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const gui_img_t *img,
                gui_img_scale_e scale)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return 0;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const gui_btn_e ret = gui__btn_logic(gui, id, MB_LEFT, contains_mouse);
	const gui__widget_render_state_e render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, &style);
	gui_img_boxed(gui, x, y, w, h, img, scale);
	if (contains_mouse)
		gui->cursor = GUI_CURSOR_BUTTON;
	gui__hint_render(gui, id, gui->style.btn.hint);
	return ret;
}

s32 gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return 0;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const gui_btn_e ret = gui__btn_logic(gui, id, MB_LEFT, contains_mouse);
	const gui__widget_render_state_e render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, &style);
	pen(gui, x, y, w, h, &style);
	if (contains_mouse)
		gui->cursor = GUI_CURSOR_BUTTON;
	gui__hint_render(gui, id, gui->style.btn.hint);
	return ret;
}

static
void gui__style_for_btn_color(gui_t *gui, color_t color)
{
	gui->style.btn.hot.outline_color = g_white;
	gui->style.btn.inactive.bg_color = color;
	gui->style.btn.hot.bg_color      = color;
	gui->style.btn.active.bg_color   = color;
}

s32 gui_btn_color(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t color)
{
	s32 result;
	gui_style_push_current(gui, btn);
	gui__style_for_btn_color(gui, color);
	result = gui_btn_txt(gui, x, y, w, h, "");
	gui_style_pop(gui);
	return result;
}

b32 gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, b32 *val)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	b32 toggled = false;
	gui__widget_render_state_e render_state;

	if (gui__btn_logic(gui, id, MB_LEFT, contains_mouse) == GUI_BTN_PRESS) {
		*val = !*val;
		toggled = true;
	}

	render_state = gui__widget_render_state(gui, id, toggled, *val, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, contains_mouse, &gui->style.chk);
	gui__hint_render(gui, id, gui->style.chk.hint);
	return toggled;
}

b32 gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen, b32 *val)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	b32 toggled = false;
	gui__widget_render_state_e render_state;
	gui_element_style_t style;

	if (gui__btn_logic(gui, id, MB_LEFT, contains_mouse) == GUI_BTN_PRESS) {
		*val = !*val;
		toggled = true;
	}

	render_state = gui__widget_render_state(gui, id, toggled, *val, contains_mouse);
	style = gui__element_style(gui, render_state, &gui->style.chk);
	gui->style.chk.pen(gui, x, y, w, h, &style);
	pen(gui, x, y, w, h, &style);
	if (contains_mouse)
		gui->cursor = GUI_CURSOR_BUTTON;
	gui__hint_render(gui, id, gui->style.chk.hint);
	return toggled;
}

typedef enum gui__slider_orientation
{
	GUI__SLIDER_X,
	GUI__SLIDER_Y,
} gui__slider_orientation_e;

static
void gui__slider_move_x(s32 x, s32 w, s32 *hx, s32 hw, s32 dx, r32 *val)
{
	const r32 min = x+hw/2;
	const r32 max = x+w-hw/2;
	const s32 pos = *hx+dx+hw/2;
	*val = fmath_clamp(0, (pos-min)/(max-min), 1.f);
	*hx = x+(w-hw)*(*val);
}

static
void gui__slider_move_y(s32 y, s32 h, s32 *hy, s32 hh, s32 dy, r32 *val)
{
	const r32 min = y+hh/2;
	const r32 max = y+h-hh/2;
	const s32 pos = *hy+dy+hh/2;
	*val = fmath_clamp(0, (pos-min)/(max-min), 1.f);
	*hy = y+(h-hh)*(*val);
}

b32 gui__slider(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val, s32 hnd_len,
                gui__slider_orientation_e orientation)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_hot = (gui->hot_id == id);
	s32 hx, hy, hw, hh;
	b32 contains_mouse;
	b32 triggered_by_key = false;

	*val = clamp(0.f, *val, 1.f);

	if (orientation == GUI__SLIDER_X) {
		hw = hnd_len;
		hh = h;
		hx = x+(w-hw)*(*val);
		hy = y;
	} else {
		hw = w;
		hh = hnd_len;
		hx = x;
		hy = y+(h-hh)*(*val);
	}

	contains_mouse = gui__widget_contains_mouse(gui, hx, hy, hw, hh);
	gui__btn_logic(gui, id, MB_LEFT, contains_mouse);

	if (gui_widget_focused(gui, id)) {
	  if (!gui->lock && gui->key_repeat.triggered) {
			if (orientation == GUI__SLIDER_X) {
				if (gui__key_left(gui)) {
					gui__slider_move_x(x, w, &hx, hw, -hw, val);
					triggered_by_key = true;
				} else if (gui__key_right(gui)) {
					gui__slider_move_x(x, w, &hx, hw,  hw, val);
					triggered_by_key = true;
				}
			} else {
				if (gui__key_up(gui)) {
					gui__slider_move_y(y, h, &hy, hh,  hh, val);
					triggered_by_key = true;
				} else if (gui__key_down(gui)) {
					gui__slider_move_y(y, h, &hy, hh, -hh, val);
					triggered_by_key = true;
				}
			}
		}
	}
	if (gui->active_id == id) {
		if (was_hot) {
			gui->drag_offset.x = gui->mouse_pos.x - hx;
			gui->drag_offset.y = gui->mouse_pos.y - hy;
		} else {
			if (orientation == GUI__SLIDER_X) {
				const s32 dx = gui->mouse_pos.x - gui->drag_offset.x - hx;
				gui__slider_move_x(x, w, &hx, hw, dx, val);
			} else {
				const s32 dy = gui->mouse_pos.y - gui->drag_offset.y - hy;
				gui__slider_move_y(y, h, &hy, hh, dy, val);
			}
		}
	}


	const gui__widget_render_state_e render_state
		= gui__widget_render_state(gui, id, triggered_by_key, false, true);
	const gui_element_style_t style_track =
		gui__element_style(gui, render_state, &gui->style.slider.track);
	const gui_element_style_t style_handle =
		gui__element_style(gui, render_state, &gui->style.slider.handle);
	if (orientation == GUI__SLIDER_X) {
		const s32 dx = (gui->style.slider.track_narrow) ? hw : 0;
		gui->style.slider.track.pen(gui, x + dx/2, y, w - dx, h, &style_track);
	} else {
		const s32 dy = (gui->style.slider.track_narrow) ? hh : 0;
		gui->style.slider.track.pen(gui, x, y + dy/2, w, h - dy, &style_track);
	}
	gui->style.slider.handle.pen(gui, hx, hy, hw, hh, &style_handle);
	if (contains_mouse)
		gui->cursor = GUI_CURSOR_BUTTON;
	gui__hint_render(gui, id, gui->style.slider.handle.hint);
	return gui->active_id == id || triggered_by_key;
}

b32 gui_slider_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val)
{
	return gui__slider(gui, x, y, w, h, val, h, GUI__SLIDER_X);
}

b32 gui_slider_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val)
{
	return gui__slider(gui, x, y, w, h, val, w, GUI__SLIDER_Y);
}

b32 gui_range_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                r32 min, r32 max)
{
	r32 slider_val = (*val - min) / (max - min);
	b32 result = gui_slider_x(gui, x, y, w, h, &slider_val);
	*val = (max - min) * slider_val + min;
	return result;
}

b32 gui_range_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                r32 min, r32 max)
{
	r32 slider_val = (*val - min) / (max - min);
	b32 result = gui_slider_y(gui, x, y, w, h, &slider_val);
	*val = (max - min) * slider_val + min;
	return result;
}

b32 gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
               const char *txt, u32 *val, u32 opt)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_selected = *val == opt;
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	gui__widget_render_state_e render_state;
	b32 selected = false;

	if (   gui__btn_logic(gui, id, MB_LEFT, contains_mouse) == GUI_BTN_PRESS
	    && !was_selected) {
		*val = opt;
		selected = true;
	}

	render_state = gui__widget_render_state(gui, id, selected, *val == opt,
	                                        contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, contains_mouse, &gui->style.select);
	gui__hint_render(gui, id, gui->style.select.hint);
	return selected;
}

b32 gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                const char *txt, u32 *val, u32 opt)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	b32 changed = false;
	gui__widget_render_state_e render_state;

	if (gui__btn_logic(gui, id, MB_LEFT, contains_mouse) == GUI_BTN_PRESS) {
		if (*val & opt) {
			*val &= ~opt;
			changed = true;
		} else {
			*val |= opt;
			changed = true;
		}
	}

	render_state = gui__widget_render_state(gui, id, changed, *val & opt,
	                                        contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, contains_mouse, &gui->style.select);
	gui__hint_render(gui, id, gui->style.select.hint);
	return changed;
}

static
void gui__popup_position(const gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         s32 pw, s32 ph, s32 *px, s32 *py)
{
	const s32 bot = y - ph;
	const s32 top = y + h + ph;
	*px = x;
	if (bot >= 0 || -bot <= -(gui->window_dim.y - top))
		*py = y - ph;
	else
		*py = y + h;
}

static
void gui__popup_btn_logic(gui_t *gui, u64 id, b32 contains_mouse,
                          s32 px, s32 py, s32 pw, s32 ph)
{
	const b32 mouse_pos_can_defocus = !contains_mouse
	                               && !mouse__covered_by_popup(gui);

	if (gui__btn_logic_ex(gui, id, MB_LEFT, contains_mouse,
	                      mouse_pos_can_defocus) == GUI_BTN_PRESS) {
		if (gui_widget_focused(gui, id))
			gui__defocus_widget(gui, id);
		else
			gui__focus_widget(gui, id);
	}
}


static
b32 gui__popup_begin(gui_t *gui, u64 id, s32 x, s32 y, s32 w, s32 h)
{
	u32 slot;
	gui_popup_t *popup = NULL;

	if (gui__find_popup(gui, id, &slot)) {
		popup = &gui->popups[slot];
	} else if (gui__find_popup(gui, 0, &slot)) {
		popup = &gui->popups[slot];
	} else {
		assert(false);
		return false;
	}

	gui_mask_push(gui, x, y, w, h);
	gui->layer->pri = GUI__LAYER_PRIORITY_POPUP;

	popup->id = id;
	popup->mask.x = x;
	popup->mask.y = y;
	popup->mask.w = w;
	popup->mask.h = h;
	popup->prev = gui->popup;
	popup->close_at_end = false;
	gui->popup = popup;
	return true;
}

static
void gui__popup_end(gui_t *gui)
{
	assert(gui->popup);

	/* switch to new layer (mirroring interrupted layer) after last item */
	gui_mask_pop(gui);

	if (gui->popup->close_at_end || gui->focus_next_widget)
		gui__defocus_widget(gui, gui->popup->id);

	if (gui->popup->prev)
		gui->popup->prev->close_at_end |= gui->popup->close_at_end;

	gui->popup = gui->popup->prev;
}

static
void gui__shadow_box(gui_t *gui, s32 px, s32 py, s32 pw, s32 ph, s32 sw, color_t color)
{
	const v2f verts[8] = {
		{ px,           py + ph },
		{ px,           py },
		{ px + pw,      py },
		{ px + pw,      py + ph },
		{ px + pw + sw, py + ph },
		{ px + pw + sw, py - sw },
		{ px - sw,      py - sw },
		{ px - sw,      py + ph },
	};
	if (gui_begin(gui, countof(verts), GUI_DRAW_TRIANGLE_STRIP)) {
		gui_vertf(gui, verts[0].x, verts[0].y, color,     0.f, 0.f);
		gui_vertf(gui, verts[7].x, verts[7].y, g_nocolor, 0.f, 0.f);
		gui_vertf(gui, verts[1].x, verts[1].y, color,     0.f, 0.f);
		gui_vertf(gui, verts[6].x, verts[6].y, g_nocolor, 0.f, 0.f);
		gui_vertf(gui, verts[2].x, verts[2].y, color,     0.f, 0.f);
		gui_vertf(gui, verts[5].x, verts[5].y, g_nocolor, 0.f, 0.f);
		gui_vertf(gui, verts[3].x, verts[3].y, color,     0.f, 0.f);
		gui_vertf(gui, verts[4].x, verts[4].y, g_nocolor, 0.f, 0.f);
		gui_end(gui);
	}
}

b32 gui_dropdown_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                       u32 *val, u32 num_items)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const s32 pw = w;
	const s32 ph = h * num_items;
	s32 px, py;
	b32 triggered_by_key = false;
	gui__widget_render_state_e render_state;

	assert(gui->dropdown.id == 0); /* cannot nest dropdowns */

	gui__popup_position(gui, x, y, w, h, pw, ph, &px, &py);
	gui__popup_btn_logic(gui, id, contains_mouse, px, py, pw, ph);

	if (gui_widget_focused(gui, id) && !gui->lock && gui->key_repeat.triggered) {
		if (gui__key_up(gui) && *val > 0) {
			--*val;
			triggered_by_key = true;
		} else if (gui__key_down(gui) && *val + 1 < num_items) {
			++*val;
			triggered_by_key = true;
		}
	}

	render_state = gui__widget_render_state(gui, id, triggered_by_key,
	                                        false, contains_mouse);

	/* store current dropdown state */
	gui->dropdown.id               = id;
	gui->dropdown.x                = x;
	gui->dropdown.y                = y;
	gui->dropdown.w                = w;
	gui->dropdown.h                = h;
	gui->dropdown.val              = val;
	gui->dropdown.num_items        = num_items;
	gui->dropdown.item_idx         = 0;
	gui->dropdown.triggered_by_key = triggered_by_key;
	gui->dropdown.render_state     = render_state;
	gui->dropdown.contains_mouse   = contains_mouse;
	strcpy(gui->dropdown.selected_item_txt, "");

	if (gui_widget_focused(gui, id)) {
		const s32 sw = gui_scale_val(gui, gui->style.dropdown.shadow.width);

		if (!gui__popup_begin(gui, id, px - sw, py - sw, pw + 2 * sw, ph + sw))
			return false;

		if (sw > 0) {
			const color_t color = gui->style.dropdown.shadow.color;
			gui__shadow_box(gui, px, py, pw, ph, sw, color);
		}

		pgui_grid_begin(gui, &gui->popup->grid, px, py, pw, ph);
		pgui_col(gui, 0, num_items);
	}

	return true;
}

b32 gui_dropdown_item(gui_t *gui, u32 id, const char *txt)
{
	b32 chosen = false;

	assert(gui->dropdown.id != 0);
	assert(gui->dropdown.item_idx < gui->dropdown.num_items);

	if (gui_widget_focused(gui, gui->dropdown.id)) {
		gui_style_push(gui, btn, gui_style(gui)->dropdown.btn);
		if (pgui_btn_txt(gui, txt) == GUI_BTN_PRESS) {
			*gui->dropdown.val = id;
			chosen = true;
		} else if (   gui->dropdown.triggered_by_key
		           && *gui->dropdown.val == id) {
			chosen = true;
		}
		gui_style_pop(gui);
	}

	if (*gui->dropdown.val == id) {
		const size_t n = countof(gui->dropdown.selected_item_txt);
		strncpy(gui->dropdown.selected_item_txt, txt, n);
		gui->dropdown.selected_item_txt[n-1] = '\0';
	}

	++gui->dropdown.item_idx;
	return chosen;
}

void gui_dropdown_end(gui_t *gui)
{
	const s32 x     = gui->dropdown.x;
	const s32 w     = gui->dropdown.w;
	const s32 y     = gui->dropdown.y;
	const s32 h     = gui->dropdown.h;
	const char *txt = gui->dropdown.selected_item_txt;
	const s32 render_state = gui->dropdown.render_state;
	const b32 contains_mouse = gui->dropdown.contains_mouse;

	assert(strlen(txt) > 0);

	if (gui_widget_focused(gui, gui->dropdown.id)) {
		pgui_grid_end(gui, &gui->popup->grid);
		gui__popup_end(gui);
	} else {
		gui__hint_render(gui, gui->dropdown.id, gui->style.dropdown.btn.hint);
	}

	gui__btn_render(gui, x, y, w, h, txt, render_state, contains_mouse, &gui->style.dropdown.btn);

	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.dropdown.btn);
	const gui_element_style_t style_pen = {
		.line = {
			.thickness = 1.f,
			.dash_len = 0.f,
			.color = style.text.color,
		},
		.text = style.text,
		.bg_color = gi_nocolor,
		.outline_color = gi_nocolor,
	};
	const s32 px = (style.text.align & GUI_ALIGN_RIGHT) ? x : x + w - h;
	gui_pen_panel_collapse(gui, px, y, h, h, &style_pen);

	memclr(gui->dropdown);
}

void gui_drag_func_default(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                           s32 offset_x, s32 offset_y, void *udata)
{
	*x = mouse_x + offset_x;
	*y = mouse_y + offset_y;
}

void gui_drag_func_horizontal(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                              s32 offset_x, s32 offset_y, void *udata)
{
	*x = mouse_x + offset_x;
}

void gui_drag_func_vertical(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                            s32 offset_x, s32 offset_y, void *udata)
{
	*y = mouse_y + offset_y;
}

b32 gui_drag(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, gui_mouse_button_e mb)
{
	return gui_dragf(gui, x, y, contains_mouse, mb, gui_drag_func_default, NULL);
}

b32 gui_dragf(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, gui_mouse_button_e mb,
              gui_drag_f cb, void *udata)
{
	b32 triggered_by_key = false;
	u64 id = gui_widget_id(gui, *x, *y);
	const b32 was_hot = (gui->hot_id == id);

	gui__btn_logic(gui, id, mb, contains_mouse);

	/* NOTE(rgriege): need to prioritize active state over focus
	 * so that mouse dragging blocks keyboard movement */
	if (gui->active_id == id) {
		if (was_hot) {
			gui->drag_offset.x = *x - gui->mouse_pos.x;
			gui->drag_offset.y = *y - gui->mouse_pos.y;
		} else {
			cb(x, y, gui->mouse_pos.x, gui->mouse_pos.y,
			   gui->drag_offset.x, gui->drag_offset.y, udata);
			id = gui_widget_id(gui, *x, *y);
			gui->active_id = id;
		}
	} else if (gui_widget_focused(gui, id)) {
	  if (!gui->lock && gui->key_repeat.triggered) {
			const s32 xo = *x, yo = *y;
			if (gui__key_up(gui))
				cb(x, y, *x, *y + 1, 0, 0, udata);
			else if (gui__key_down(gui))
				cb(x, y, *x, *y - 1, 0, 0, udata);
			else if (gui__key_left(gui))
				cb(x, y, *x - 1, *y, 0, 0, udata);
			else if (gui__key_right(gui))
				cb(x, y, *x + 1, *y, 0, 0, udata);
			if (xo != *x || yo != *y) {
				gui__defocus_widget(gui, id);
				id = gui_widget_id(gui, *x, *y);
				gui__focus_widget(gui, id);
				triggered_by_key = true;
			}
		}
	}

	return gui->active_id == id || triggered_by_key;
}

static
void gui__drag_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h, u64 id, b32 dragging)
{
	const gui_widget_style_t *widget_style = &gui->style.drag;
	const s32 state = gui__widget_render_state(gui, id, dragging, false, true);
	const gui_element_style_t style = gui__element_style(gui, state, widget_style);
	widget_style->pen(gui, x, y, w, h, &style);
	gui__hint_render(gui, id, widget_style->hint);
}

static
void gui__drag_rect_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h, b32 dragging)
{
	gui__drag_render(gui, x, y, w, h, gui_widget_id(gui, x, y), dragging);
}

b32 gui_drag_rect(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, gui_mouse_button_e mb)
{
	return gui_drag_rectf(gui, x, y, w, h, mb, gui_drag_func_default, NULL);
}

b32 gui_drag_rectf(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, gui_mouse_button_e mb,
                   gui_drag_f cb, void *udata)
{
	const b32 contains_mouse = gui__widget_contains_mouse(gui, *x, *y, w, h);
	const b32 ret = gui_dragf(gui, x, y, contains_mouse, mb, cb, udata);
	gui__drag_rect_render(gui, *x, *y, w, h, ret);
	return ret;
}

b32 gui_drag_circ(gui_t *gui, s32 *x, s32 *y, s32 r, gui_mouse_button_e mb)
{
	return gui_drag_circf(gui, x, y, r, mb, gui_drag_func_default, NULL);
}

b32 gui_drag_circf(gui_t *gui, s32 *x, s32 *y, s32 r, gui_mouse_button_e mb,
                   gui_drag_f cb, void *udata)
{
	const v2i pos = { *x, *y };
	box2i box;
	box2i_from_center(&box, pos, (v2i){r, r});
	const b32 contains_mouse = v2i_dist_sq(pos, gui->mouse_pos) < r * r
	                        && gui__box_half_visible(gui, box)
	                        && !gui->lock;
	const b32 ret = gui_dragf(gui, x, y, contains_mouse, mb, cb, udata);
	gui_style_push_pen(gui, drag.pen, gui_pen_circ);
	gui__drag_render(gui, *x - r, *y - r, 2 * r, 2 * r, gui_widget_id(gui, *x, *y), ret);
	gui_style_pop(gui);
	return ret;
}

static
b32 gui__resize_horiz(gui_t *gui, s32 *x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, *x, y, w, h);
	if (gui_drag_rectf(gui, x, &y, w, h, MB_LEFT, gui_drag_func_horizontal, NULL)) {
		gui->cursor = GUI_CURSOR_RESIZE_EW;
		return true;
	} else if (box2i_contains_point(box, gui->mouse_pos)) {
		gui->cursor = GUI_CURSOR_RESIZE_EW;
	}
	return false;
}

static
b32 gui__resize_vert(gui_t *gui, s32 x, s32 *y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, x, *y, w, h);
	if (gui_drag_rectf(gui, &x, y, w, h, MB_LEFT, gui_drag_func_vertical, NULL)) {
		gui->cursor = GUI_CURSOR_RESIZE_NS;
		return true;
	} else if (box2i_contains_point(box, gui->mouse_pos)) {
		gui->cursor = GUI_CURSOR_RESIZE_NS;
	}
	return false;
}

b32 gui_menu_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                   const char *txt, s32 item_w, u32 num_items)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const s32 pw = item_w;
	const s32 ph = h * num_items;
	s32 px, py;
	gui__widget_render_state_e render_state;

	if (gui->popup) {
		px = x + w;
		py = y + h - ph;
	} else {
		px = x;
		py = y - ph;
	}

	gui__popup_btn_logic(gui, id, contains_mouse, px, py, pw, ph);

	render_state = gui__widget_render_state(gui, id, false, false, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, contains_mouse, &gui->style.dropdown.btn);

	if (gui_widget_focused(gui, id)) {
		const s32 sw = gui_scale_val(gui, gui->style.dropdown.shadow.width);

		if (!gui__popup_begin(gui, id, px - sw, py - sw, pw + 2 * sw, ph + sw))
			return false;

		if (sw > 0) {
			const color_t color = gui->style.dropdown.shadow.color;
			gui__shadow_box(gui, px, py, pw, ph, sw, color);
		}

		pgui_grid_begin(gui, &gui->popup->grid, px, py, pw, ph);
		pgui_col(gui, 0, num_items);
		return true;
	} else {
		gui__hint_render(gui, id, gui->style.dropdown.btn.hint);
		return false;
	}
}

void gui_menu_end(gui_t *gui)
{
	assert(gui->popup);
	assert(gui->popup->id != 0);
	assert(gui->popup->id == gui->focus_ids[gui->popup - gui->popups]);
	pgui_grid_end(gui, &gui->popup->grid);
	gui__popup_end(gui);
}

b32 gui_color_picker_sv(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const colorf_t c_orig = *c;
	colorf_t cf_hue = { .a = 1.f };
	color_t c_hue;
	r32 hue, sat, val;
	b32 changed = false;
	v2i cursor;
	s32 nx = x, ny = y;

	color_to_hsv(*c, &hue, &sat, &val);
	hsv_to_color(hue, 1.f, 1.f, &cf_hue);
	c_hue = colorf_to_color(cf_hue);

	gui_rect_mcolor(gui, x, y, w, h, g_white, c_hue,   c_hue,     g_white);
	gui_rect_mcolor(gui, x, y, w, h, g_black, g_black, g_nocolor, g_nocolor);

	gui_style_push(gui, drag, g_gui_style_invis.drag);
	if (gui_drag_rect(gui, &nx, &ny, w, h, MB_LEFT)) {
		const u64 old_id = gui_widget_id(gui,  x,  y);
		const u64 new_id = gui_widget_id(gui, nx, ny);
		if (gui__is_most_focused_widget(gui, new_id)) {
			gui__defocus_widget(gui, new_id);
			gui__focus_widget(gui, old_id);
		}
		if (gui->active_id == new_id)
			gui->active_id = old_id;
		if (mouse_down(gui, MB_LEFT)) {
			sat = clamp(0, (r32)(gui->mouse_pos.x - x) / w, 1.f);
			val = clamp(0, (r32)(gui->mouse_pos.y - y) / h, 1.f);
		} else {
			sat = clamp(0, sat + (r32)(nx - x) / w, 1.f);
			val = clamp(0, val + (r32)(ny - y) / h, 1.f);
		}
		hsv_to_color(hue, sat, val, c);
		changed = !colorf_equal(*c, c_orig);
	}
	gui_style_pop(gui);

	cursor.x = x+sat*w;
	cursor.y = y+val*h;
	gui_circ(gui, cursor.x, cursor.y, 6, g_nocolor, g_white);
	gui_circ(gui, cursor.x, cursor.y, 7, g_nocolor, g_black);
	if (gui__widget_contains_mouse(gui, x, y, w, h))
		gui->cursor = GUI_CURSOR_BUTTON;

	return changed;
}

b32 gui_color_picker_h(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	static const color_t rainbow[] = {
		gi_red, gi_yellow, gi_green, gi_cyan, gi_blue, gi_fuchsia, gi_red,
	};

	const colorf_t c_orig = *c;
	const b32 horizontal = (w > h);
	b32 changed = false;
	r32 hue, sat, val;
	s32 cursor;
	s32 nx = x, ny = y;

	if (horizontal) {
		for (u32 i = 0; i < countof(rainbow) - 1; ++i) {
			const color_t left  = rainbow[i];
			const color_t right = rainbow[i+1];
			const s32 xl = x+w*i/6;
			const s32 xr = x+w*(i+1)/6;
			gui_rect_mcolor(gui, xl, y, xr-xl, h, left, right, right, left);
		}
	} else {
		for (u32 i = 0; i < countof(rainbow) - 1; ++i) {
			const color_t top    = rainbow[i];
			const color_t bottom = rainbow[i+1];
			const s32 yt = y+h-h*i/6;
			const s32 yb = y+h-h*(i+1)/6;
			gui_rect_mcolor(gui, x, yb, w, yt-yb, bottom, bottom, top, top);
		}
	}

	color_to_hsv(*c, &hue, &sat, &val);

	gui_style_push(gui, drag, g_gui_style_invis.drag);
	if (gui_drag_rect(gui, &nx, &ny, w, h, MB_LEFT)) {
		const u64 old_id = gui_widget_id(gui,  x,  y);
		const u64 new_id = gui_widget_id(gui, nx, ny);
		if (gui__is_most_focused_widget(gui, new_id)) {
			gui__defocus_widget(gui, new_id);
			gui__focus_widget(gui, old_id);
		}
		if (gui->active_id == new_id)
			gui->active_id = old_id;
		if (mouse_down(gui, MB_LEFT)) {
			if (horizontal)
				hue = clamp(0, (r32)(gui->mouse_pos.x - x) / w, 1.f);
			else
				hue = 1.f-clamp(0, (r32)(gui->mouse_pos.y - y) / h, 1.f);
		} else {
			if (horizontal)
				hue = clamp(0, hue + (r32)(nx - x) / w, 1.f);
			else
				hue = clamp(0, hue - (r32)(ny - y) / h, 1.f);
		}
		hsv_to_color(hue, sat, val, c);
		changed = !colorf_equal(*c, c_orig);
	}
	gui_style_pop(gui);

	if (horizontal) {
		cursor = x+hue*w;
		gui_rect(gui, cursor-1, y, 2, h, g_nocolor, g_white);
		gui_rect(gui, cursor-2, y, 4, h, g_nocolor, g_black);
	} else {
		cursor = y+(1.f-hue)*h;
		gui_rect(gui, x, cursor-1, w, 2, g_nocolor, g_white);
		gui_rect(gui, x, cursor-2, w, 4, g_nocolor, g_black);
	}
	if (gui__widget_contains_mouse(gui, x, y, w, h))
		gui->cursor = GUI_CURSOR_BUTTON;

	return changed;
}

gui_padding_style_t gui__scale_padding(gui_padding_style_t padding, s32 scale);

b32 gui_color_picker_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           s32 pw, s32 ph, colorf_t c)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui_widget_bounds_extend(gui, x, y, w, h);
		++gui->culled_widgets;
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	s32 px, py;
	gui__widget_render_state_e render_state;
	gui_style_t *style = gui_style(gui);
	color_t color = colorf_to_color(c);

	gui__popup_position(gui, x, y, w, h, pw, ph, &px, &py);
	gui__popup_btn_logic(gui, id, contains_mouse, px, py, pw, ph);

	render_state = gui__widget_render_state(gui, id, false, false, contains_mouse);

	gui_style_push_current(gui, btn);
	gui__style_for_btn_color(gui, color);
	gui__btn_render(gui, x, y, w, h, "", render_state, contains_mouse, &gui->style.btn);
	gui_style_pop(gui);

	if (gui_widget_focused(gui, id)) {
		const s32 sw = gui_scale_val(gui, gui->style.color_picker.shadow.width);
		const gui_padding_style_t padding = gui__scale_padding(style->panel.padding, gui->scale);
		const s32 gx = px + padding.left;
		const s32 gy = py + padding.bottom;
		const s32 gw = pw - padding.left - padding.right;
		const s32 gh = ph - padding.bottom - padding.top;

		if (!gui__popup_begin(gui, id, px, py, pw, ph))
			return false;

		if (sw > 0) {
			const color_t shadow_color = gui->style.color_picker.shadow.color;
			gui__shadow_box(gui, px, py, pw, ph, sw, shadow_color);
		}

		pgui_grid_begin(gui, &gui->popup->grid, gx, gy, gw, gh);
		/* NOTE(rgriege): shrink required to pass entire rect through layer */
		gui_rect(gui, px+1, py, pw-1, ph-1, style->panel.bg_color, style->panel.border_color);
		return true;
	} else {
		return false;
	}
}

b32 gui_color_picker_popup_default(gui_t *gui, colorf_t *c)
{
	const s32 cols_major[] = { 0, 80 };
	const s32 rows[] = { 0, 2, 20 };
	const s32 cols[] = { 0, 2, 20 };
	const s32 cols_npt[] = { 20, 0 };
	const s32 rows_npt[] = { 20, 5, 20, 5, 20, 10, 20, 5, 20, 5, 20, 10, 20 };
	const gui_npt_flags_e flags = GUI_NPT_COMPLETE_ON_TAB
	                            | GUI_NPT_COMPLETE_ON_CLICK_OUT;
	gui_npt_filter_p filter = &g_gui_npt_filter_hex;
	b32 changed = false, changed_npt = false;
	color_t color = colorf_to_color(*c);
	u8 ch, cs, cv;

	pgui_row_cellsv(gui, 0, cols_major);

	pgui_col_cellsv(gui, 0, rows);

	pgui_row_cellsv(gui, 0, cols);
	if (pgui_color_picker_sv(gui, c)) changed = true;
	pgui_spacer(gui);
	if (pgui_color_picker_h(gui, c)) changed = true;

	if (changed) color = colorf_to_color(*c);

	pgui_row_empty(gui, 0);

	pgui_row_cellsv(gui, 0, cols);
	gui_style_push_color(gui, panel.cell_bg_color, color);
	pgui_spacer(gui);
	gui_style_pop(gui);
	pgui_spacer(gui);
	pgui_spacer(gui);

	gui_style_push_s32(gui, txt.align, GUI_ALIGN_MIDRIGHT);

	pgui_col_cellsv(gui, 0, rows_npt);

	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "r");
	if (pgui_npt_val(gui, imprintf("%u", color.r), 4, flags, filter)) {
		changed_npt = true;
		color.r = strtoul(gui_npt_val_buf(gui), NULL, 0);
	}
	pgui_row_empty(gui, 0);

	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "g");
	if (pgui_npt_val(gui, imprintf("%u", color.g), 4, flags, filter)) {
		changed_npt = true;
		color.g = strtoul(gui_npt_val_buf(gui), NULL, 0);
	}
	pgui_row_empty(gui, 0);

	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "b");
	if (pgui_npt_val(gui, imprintf("%u", color.b), 4, flags, filter)) {
		changed_npt = true;
		color.b = strtoul(gui_npt_val_buf(gui), NULL, 0);
	}
	pgui_row_empty(gui, 0);

	color_to_hsv8(color, &ch, &cs, &cv);
	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "h");
	if (pgui_npt_val(gui, imprintf("%u", ch), 4, flags, filter)) {
		changed_npt = true;
		ch = strtoul(gui_npt_val_buf(gui), NULL, 0);
		hsv_to_color8(ch, cs, cv, &color);
	}
	pgui_row_empty(gui, 0);

	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "s");
	if (pgui_npt_val(gui, imprintf("%u", cs), 4, flags, filter)) {
		changed_npt = true;
		cs = strtoul(gui_npt_val_buf(gui), NULL, 0);
		hsv_to_color8(ch, cs, cv, &color);
	}
	pgui_row_empty(gui, 0);

	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "v");
	if (pgui_npt_val(gui, imprintf("%u", cv), 4, flags, filter)) {
		changed_npt = true;
		cv = strtoul(gui_npt_val_buf(gui), NULL, 0);
		hsv_to_color8(ch, cs, cv, &color);
	}
	pgui_row_empty(gui, 0);

	pgui_row_cellsv(gui, 0, cols_npt);
	pgui_txt(gui, "#");
	if (pgui_npt_val(gui, imprintf("%.2x%.2x%.2x", color.r, color.g, color.b), 8, flags, filter)) {
		const color_t c_orig = color;
		if (color_from_hex(gui_npt_val_buf(gui), &color))
			changed_npt = true;
		else
			color = c_orig;
	}

	gui_style_pop(gui);

	if (changed_npt) {
		*c = color_to_colorf(color);
		changed = true;
	}

	return changed;
}

void gui_color_picker_end(gui_t *gui)
{
	assert(gui->popup);
	assert(gui->popup->id != 0);
	assert(gui->popup->id == gui->focus_ids[gui->popup - gui->popups]);
	pgui_grid_end(gui, &gui->popup->grid);
	gui__popup_end(gui);
}

b32 gui_color_picker8_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            s32 pw, s32 ph, color_t c)
{
	const u64 id = gui_widget_id(gui, x, y);
	if (gui_widget_focused(gui, id)) {
		return gui_color_picker_begin(gui, x, y, w, h, pw, ph, gui->color_picker8_color);
	} else {
		colorf_t cf = color_to_colorf(c);
		if (gui_color_picker_begin(gui, x, y, w, h, pw, ph, cf)) {
			gui->color_picker8_color = cf;
			return true;
		}
	}
	return false;
}

b32 gui_color_picker8_popup_default(gui_t *gui, color_t *c)
{
	if (gui_color_picker_popup_default(gui, &gui->color_picker8_color)) {
		*c = colorf_to_color(gui->color_picker8_color);
		return true;
	}
	return false;
}

void gui_color_picker8_end(gui_t *gui)
{
	gui_color_picker_end(gui);
}

b32 gui_color_picker(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     s32 pw, s32 ph, colorf_t *c)
{
	b32 changed = false;
	if (gui_color_picker_begin(gui, x, y, w, h, pw, ph, *c)) {
		changed = gui_color_picker_popup_default(gui, c);
		gui_color_picker_end(gui);
	}
	return changed;
}

b32 gui_color_picker8(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                      s32 pw, s32 ph, color_t *c)
{
	b32 changed = false;
	if (gui_color_picker8_begin(gui, x, y, w, h, pw, ph, *c)) {
		changed = gui_color_picker8_popup_default(gui, c);
		gui_color_picker8_end(gui);
	}
	return changed;
}

static
void gui__scroll_area_usable_rect(const gui_t *gui,
                                  const gui_scroll_area_t *scroll_area,
                                  s32 *x, s32 *y, s32 *w, s32 *h)
{
	const gui_scroll_area_style_t *style = &gui->style.scroll_area;
	const s32 scrollbar_track_width = gui_scale_val(gui, style->scrollbar_track_width);
	const gui_padding_style_t padding = gui__scale_padding(style->padding, gui->scale);

	*x = scroll_area->pos.x;
	*y = scroll_area->pos.y;
	*w = scroll_area->dim.x;
	*h = scroll_area->dim.y;

	*x += padding.left;
	*y += padding.bottom;
	*w -= padding.left + padding.right;
	*h -= padding.bottom + padding.top;

	if (   scroll_area->dim.x < box2i_width(scroll_area->last_children_bbox)
	    && scrollbar_track_width > padding.bottom) {
		const s32 extra_track_width = scrollbar_track_width - padding.bottom;
		*y += extra_track_width;
		*h -= extra_track_width;
	}

	if (   scroll_area->dim.y < box2i_height(scroll_area->last_children_bbox)
	    && scrollbar_track_width > padding.right) {
		const s32 extra_track_width = scrollbar_track_width - padding.right;
		*w -= extra_track_width;
	}

	*w = max(*w, 0);
	*h = max(*h, 0);
}

void gui_scroll_area_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           gui_scroll_area_t *scroll_area)
{
	s32 rx, ry, rw, rh;
	box2i mask;

	scroll_area->pos.x = x;
	scroll_area->pos.y = y;
	scroll_area->dim.x = w;
	scroll_area->dim.y = h;

	gui__scroll_area_usable_rect(gui, scroll_area, &rx, &ry, &rw, &rh);

	box2i_from_xywh(&mask, rx, ry, rw, rh);
	gui__mask_box(gui, &mask);
	gui_mask_push_box(gui, mask);

	box2i_from_xywh(&scroll_area->widget_bounds.bbox, rx, ry, rw, rh);
	/* NOTE(rgriege): the center of the scroll area may not actually be used
	 * by a widget, but it's the best candidate. */
	box2i_from_xywh(&scroll_area->widget_bounds.children, rx+rw/2, ry+rh/2, 0, 0);
	gui_widget_bounds_push(gui, &scroll_area->widget_bounds, true);

	scroll_area->prev = gui->scroll_area;
	gui->scroll_area = scroll_area;
}

void gui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area)
{
	const gui_scroll_area_style_t *style = &gui->style.scroll_area;
	const s32 scroll_rate = gui_scale_val(gui, GUI_SCROLL_RATE);
	const s32 track_width = gui_scale_val(gui, style->scrollbar_track_width);
	const v2i pos = scroll_area->pos;
	const v2i dim = scroll_area->dim;
	const v2i prev_scroll = scroll_area->scroll;
	const box2i last_children_bbox = scroll_area->last_children_bbox;
	const b32 contains_mouse
		= gui__widget_contains_mouse(gui, pos.x, pos.y, dim.x, dim.y);
	const color_t bg_color = style->bg_color;
	s32 rx, ry, rw, rh;

	assert(gui->scroll_area == scroll_area);

	gui->scroll_area = gui->scroll_area->prev;

	gui_widget_bounds_pop(gui, &scroll_area->widget_bounds, true);

	gui_mask_pop(gui);

	gui_rect(gui, pos.x, pos.y, dim.x, dim.y, bg_color, g_nocolor);

	gui__scroll_area_usable_rect(gui, scroll_area, &rx, &ry, &rw, &rh);

	if (rw < box2i_width(last_children_bbox)) {
		const s32 needed_l = max(rx - last_children_bbox.min.x, 0);
		const s32 needed_r = max(last_children_bbox.max.x - (rx + rw), 0);
		const s32 needed = needed_l + needed_r;
		scroll_area->scroll.x = -clamp(-needed_l, -scroll_area->scroll.x, needed_r);
		if (key_mod(gui, KBM_SHIFT) && !mouse_covered(gui) && contains_mouse) {
			s32 scroll;
			mouse_scroll(gui, &scroll);
			scroll_area->scroll.x -= scroll * scroll_rate;
			scroll_area->scroll.x = -clamp(-needed_l, -scroll_area->scroll.x, needed_r);
		}
		if (track_width > 0) {
			const s32 x = pos.x;
			const s32 y = pos.y;
			const s32 w = dim.x;
			const s32 h = track_width;
			const s32 handle_size = max(track_width, w * rw / box2i_width(last_children_bbox));
			r32 slider_val = (r32)(-scroll_area->scroll.x + needed_l) / (r32)needed;
			gui_style_push(gui, slider, style->scrollbar);
			gui__slider(gui, x, y, w, h, &slider_val, handle_size, GUI__SLIDER_X);
			gui_style_pop(gui);
			scroll_area->scroll.x = -((s32)(slider_val * (r32)needed) - needed_l);
			scroll_area->scroll.x = -clamp(-needed_l, -scroll_area->scroll.x, needed_r);
		}
	} else {
		scroll_area->scroll.x = 0;
	}

	if (rh < box2i_height(last_children_bbox)) {
		const s32 needed_b = max(ry - last_children_bbox.min.y, 0);
		const s32 needed_t = max(last_children_bbox.max.y - (ry + rh), 0);
		const s32 needed = needed_b + needed_t;
		scroll_area->scroll.y = clamp(-needed_t, scroll_area->scroll.y, needed_b);
		if (!key_mod(gui, KBM_SHIFT) && !mouse_covered(gui) && contains_mouse) {
			s32 scroll;
			mouse_scroll(gui, &scroll);
			scroll_area->scroll.y -= scroll * scroll_rate;
			scroll_area->scroll.y = clamp(-needed_t, scroll_area->scroll.y, needed_b);
		}
		if (track_width > 0) {
			const s32 x = pos.x + dim.x - track_width;
			const s32 y = pos.y;
			const s32 w = track_width;
			const s32 h = dim.y;
			const s32 handle_size = max(track_width, h * rh / box2i_height(last_children_bbox));
			r32 slider_val = 1.f - ((r32)(scroll_area->scroll.y - needed_t) / (r32)needed);
			gui_style_push(gui, slider, style->scrollbar);
			gui__slider(gui, x, y, w, h, &slider_val, handle_size, GUI__SLIDER_Y);
			gui_style_pop(gui);
			scroll_area->scroll.y = (s32)(-(slider_val - 1.f) * (r32)needed) + needed_t;
			scroll_area->scroll.y = clamp(-needed_t, scroll_area->scroll.y, needed_b);
		}
	} else {
		scroll_area->scroll.y = 0;
	}

	scroll_area->last_children_bbox = scroll_area->widget_bounds.children;
	scroll_area->last_children_bbox.min.x -= prev_scroll.x;
	scroll_area->last_children_bbox.max.x -= prev_scroll.x;
	scroll_area->last_children_bbox.min.y -= prev_scroll.y;
	scroll_area->last_children_bbox.max.y -= prev_scroll.y;

	if (contains_mouse && !mouse_covered(gui))
		mouse_cover(gui, gui_widget_id(gui, pos.x, pos.y));
}

void pgui_scroll_area_grid_begin(gui_t *gui, gui_grid_t *grid, gui_grid_flex_e flex)
{
	const gui_scroll_area_t *scroll_area = gui->scroll_area;
	s32 gx, gy, gw, gh;

	assert(scroll_area);

	gui__scroll_area_usable_rect(gui, scroll_area, &gx, &gy, &gw, &gh);

	gx += scroll_area->scroll.x;
	if (flex & GUI_GRID_FLEX_HORIZONTAL)
		gw = 0;

	gy += scroll_area->scroll.y;
	if (flex & GUI_GRID_FLEX_VERTICAL) {
		gy += gh;
		gh = 0;
	}

	pgui_grid_begin(gui, grid, gx, gy, gw, gh);
}

void pgui_scroll_area_grid_end(gui_t *gui, gui_grid_t *grid)
{
	pgui_grid_end(gui, grid);
}

/* IDs:  0......1........X.........UINT_MAX
 *       ^          ^         ^        ^
 *      skip      panels   popups   default
 *
 * 0 is skipped to ensure that no widget id is ever 0,
 * which is used as the invalid id */
#define GUI__DEFAULT_PANEL_ID UINT_MAX
#define GUI__POPUP_PANEL_ID  (UINT_MAX-1)
#define GUI__MAX_PANEL_ID    (UINT_MAX-1-GUI_POPUP_STACK_SIZE-1)

u64 gui__widget_id(const gui_t *gui, u32 base_id, s32 x, s32 y)
{
	return (((u64)x) << 48) | (((u64)y) << 32) | base_id;
}

u64 gui_widget_id(const gui_t *gui, s32 x, s32 y)
{
	if (gui->popup)
		return gui__widget_id(gui, GUI__POPUP_PANEL_ID - (gui->popup - gui->popups), x, y);
	else if (gui->panel)
		return gui__widget_id(gui, gui->panel->id + 1, x, y);
	else
		return gui__widget_id(gui, GUI__DEFAULT_PANEL_ID, x, y);
}

void gui_widget_focus_next(gui_t *gui)
{
	if (   gui->focus_ids[0] == 0
	    && !gui->focus_next_widget
	    && gui->focus_prev_widget_id == 0)
		gui->focus_next_widget = true;
}

void gui_widget_deactivate(gui_t *gui, u64 id)
{
	if (gui->active_id == id)
		gui->active_id = 0;
}

b32 gui_widget_hot(const gui_t *gui, u64 id)
{
	return gui->hot_id == id;
}

b32 gui_widget_active(const gui_t *gui, u64 id)
{
	return gui->active_id == id;
}

b32 gui_widget_focused(const gui_t *gui, u64 id)
{
	for (u32 i = 0; i < countof(gui->focus_ids); ++i)
		if (gui->focus_ids[i] == id)
			return true;
	return false;
}

b32 gui_any_widget_hot(const gui_t *gui)
{
	return gui->hot_id != 0;
}

b32 gui_any_widget_active(const gui_t *gui)
{
	return gui->active_id != 0;
}

b32 gui_any_widget_has_focus(const gui_t *gui)
{
	return gui->focus_ids[0] != 0;
}

b32 gui_any_widget_had_focus(const gui_t *gui)
{
	return gui->focus_id_at_frame_start != 0;
}

b32 gui_locked(const gui_t *gui)
{
	return gui->lock > 0;
}

void gui_lock(gui_t *gui)
{
	++gui->lock;
}

void gui_unlock(gui_t *gui)
{
	assert(gui->lock > 0);
	--gui->lock;
}

void gui_lock_if(gui_t *gui, b32 cond, u32 *lock)
{
	if (cond) {
		gui_lock(gui);
		*lock = gui->lock;
	} else {
		*lock = 0;
	}
}

void gui_unlock_if(gui_t *gui, u32 lock)
{
	if (lock) {
		assert(gui->lock == lock);
		gui_unlock(gui);
	}
}

void gui_set_scale(gui_t *gui, s32 scale)
{
	assert(!gui->panel);
	gui->scale = scale;
}

s32 gui__scale_val(s32 val, s32 scale)
{
	return val * scale / 100;
}

s32 gui_scale_val(const gui_t *gui, s32 val)
{
	return gui__scale_val(val, gui->scale);
}

s32 gui_scale_val_inverse(const gui_t *gui, s32 val)
{
	return val * 100 / gui->scale;
}

gui_padding_style_t gui__scale_padding(gui_padding_style_t padding, s32 scale)
{
	return (gui_padding_style_t) {
		.top    = gui__scale_val(padding.top, scale),
		.right  = gui__scale_val(padding.right, scale),
		.bottom = gui__scale_val(padding.bottom, scale),
		.left   = gui__scale_val(padding.left, scale),
	};
}


/* Grid layout */

static
void gui__grid_init(gui_grid_t *grid, s32 x, s32 y, s32 w, s32 h, gui_grid_t *prev)
{
	grid->prev    = prev;
	grid->depth   = 0;
	grid->pos.x   = x;
	grid->pos.y   = y + h; /* widgets are drawn from high to low */
	grid->dim.x   = w;
	grid->dim.y   = h;
	grid->start.x = grid->pos.x;
	grid->start.y = grid->pos.y;
	box2i_from_xywh(&grid->widget_bounds.bbox, x, y, w, h);
	box2i_from_center(&grid->widget_bounds.children, grid->start, g_v2i_zero);
}

static
void gui__grid_get_next_cell(const gui_grid_t *grid, s32 *x, s32 *y, s32 *w, s32 *h)
{
	const gui_grid_strip_t *strip;
	b32 vertical;
	v2i pos, dim;

	assert(grid->depth > 0);

	strip = &grid->strips[grid->depth - 1];
	vertical = strip->vertical;

	dim.d[vertical] = *strip->current_cell;
	dim.d[!vertical] = strip->dim.d[!vertical];
	pos.x = grid->pos.x;
	pos.y = grid->pos.y - dim.y;

	if (x)
		*x = pos.x;
	if (y)
		*y = pos.y;
	if (w)
		*w = dim.x;
	if (h)
		*h = dim.y;
}

static
void gui__grid_advance_cell(gui_grid_t *grid)
{
	gui_grid_strip_t *strip;

	assert(grid->depth > 0);

	do {
		strip = &grid->strips[grid->depth - 1];
		if (strip->vertical)
			grid->pos.y -= *strip->current_cell;
		else
			grid->pos.x += *strip->current_cell;
		++strip->current_cell;
		if (strip->current_cell == strip->max_cell) {
			--grid->depth;
			if (strip->vertical)
				grid->pos.y += strip->dim.y;
			else
				grid->pos.x -= strip->dim.x;
		} else {
			break;
		}
	} while (grid->depth > 0);

	if (grid->depth == 0) {
		strip = &grid->strips[0];
		if (strip->vertical)
			grid->pos.x += strip->dim.x;
		else
			grid->pos.y -= strip->dim.y;
	}
}

static
s32 gui__grid_strip_parent_dimension(const gui_grid_t *grid, b32 vertical)
{
	if (grid->depth == 0) {
		return vertical
		     ? grid->pos.y - (grid->start.y - grid->dim.y)
		     : grid->start.x + grid->dim.x - grid->pos.x;
	} else {
		v2i cell;
		gui__grid_get_next_cell(grid, NULL, NULL, &cell.x, &cell.y);
		return cell.d[vertical];
	}
}

static
s32 *gui__grid_cells_end(gui_grid_t *grid)
{
	return grid->depth > 0 ? grid->strips[grid->depth-1].max_cell : grid->cells;
}

static
void gui__grid_add_strip(gui_grid_t *grid, b32 vertical, s32 minor_dim,
                         const s32 *cells, u32 num_cells, s32 scale)
{
	const s32 parent_dim = gui__grid_strip_parent_dimension(grid, vertical);
	s32 fixed_dim;
	s32 remaining_dim;
	s32 pcnt_total;
	s32 flex_cell_cnt;
	s32 flex_cell_dim;
	gui_grid_strip_t *strip;

	assert(num_cells > 0);
	assert(grid->depth < GUI_GRID_MAX_DEPTH);
	assert(num_cells < GUI_GRID_MAX_CELLS);
	assert((gui__grid_cells_end(grid) - grid->cells) + num_cells < GUI_GRID_MAX_CELLS);

	/* setup strip */
	strip = &grid->strips[grid->depth];
	strip->vertical     = vertical;
	strip->num_cells    = num_cells;
	strip->dim          = g_v2i_zero;
	strip->current_cell = gui__grid_cells_end(grid);
	strip->max_cell     = strip->current_cell + num_cells;

	fixed_dim = 0;
	pcnt_total = 0;
	flex_cell_cnt = 0;
	for (u32 i = 0; i < num_cells; ++i) {
		if (cells[i] > 0)
			fixed_dim += gui__scale_val(cells[i], scale);
		else if (cells[i] < 0)
			pcnt_total += -cells[i];
		else if (cells[i] == 0)
			++flex_cell_cnt;
	}

	/* compute strip dimensions */
	strip->dim.d[vertical]  = fixed_dim > parent_dim || pcnt_total + flex_cell_cnt == 0
	                        ? fixed_dim : parent_dim;
	strip->dim.d[!vertical] = (minor_dim == GUI_GRID_FLEX)
	                        ? gui__grid_strip_parent_dimension(grid, !vertical)
	                        : gui__scale_val(minor_dim, scale);

	/* compute flex/percent cell info */
	remaining_dim = strip->dim.d[vertical] - fixed_dim;
	flex_cell_dim = 0;
	if (pcnt_total < 100 && flex_cell_cnt > 0) {
		flex_cell_dim = (100 - pcnt_total) * remaining_dim / (100 * flex_cell_cnt);
		pcnt_total = 100;
	}

	/* compute cell dimensions */
	for (u32 i = 0; i < num_cells; ++i) {
		if (cells[i] > 0)
			strip->current_cell[i] = gui__scale_val(cells[i], scale);
		else if (cells[i] < 0)
			strip->current_cell[i] = -cells[i] * remaining_dim / pcnt_total;
		else if (cells[i] == 0)
			strip->current_cell[i] = flex_cell_dim;
	}

	/* distribute leftover dimension (due to pixel rounding) in a round-robin fashion */
	remaining_dim = strip->dim.d[vertical];
	for (u32 i = 0; i < num_cells; ++i)
		remaining_dim -= strip->current_cell[i];

	if (remaining_dim > 0 && (pcnt_total > 0 || flex_cell_cnt > 0)) {
		u32 i = 0;
		while (remaining_dim > 0) {
			if (cells[i] <= 0) {
				++strip->current_cell[i];
				--remaining_dim;
			}
			i = (i + 1) % num_cells;
		}
	}

	++grid->depth;
}

void pgui_grid_begin(gui_t *gui, gui_grid_t *grid, s32 x, s32 y, s32 w, s32 h)
{
	assert(grid != gui->grid);
	gui__grid_init(grid, x, y, w, h, gui->grid);
	gui_widget_bounds_push(gui, &grid->widget_bounds, true);
	gui->grid = grid;
}

void pgui_grid_end(gui_t *gui, gui_grid_t *grid)
{
	assert(grid);
	assert(gui->grid == grid);
	assert(gui->grid->depth == 0);
	gui->grid = gui->grid->prev;
	box2i_extend_box(&gui->widget_bounds->bbox, gui->widget_bounds->children);
	gui_widget_bounds_pop(gui, &grid->widget_bounds, true);
}

void pgui_row(gui_t *gui, s32 height, u32 num_cells)
{
	const s32 cells[GUI_GRID_MAX_CELLS] = { 0 };
	static_assert(GUI_GRID_FLEX == 0, "invalid initialization");
	assert(num_cells < GUI_GRID_MAX_CELLS);
	pgui_row_cells(gui, height, cells, num_cells);
}

void pgui_row_cells(gui_t *gui, s32 height, const s32 *cells, u32 num_cells)
{
	assert(gui->grid);
	gui__grid_add_strip(gui->grid, false, height, cells, num_cells, gui->scale);
}

void pgui_row_empty(gui_t *gui, s32 height)
{
	pgui_row(gui, height, 1);
	pgui_spacer_blank(gui);
}

void pgui_row_centered(gui_t *gui, s32 height, s32 width)
{
	const s32 cells[3] = { 0, width, 0 };
	static_assert(GUI_GRID_FLEX == 0, "invalid initialization");
	assert(countof(cells) < GUI_GRID_MAX_CELLS);
	pgui_row_cells(gui, height, B2PC(cells));
}

void pgui_col(gui_t *gui, s32 width, u32 num_cells)
{
	const s32 cells[GUI_GRID_MAX_CELLS] = { 0 };
	static_assert(GUI_GRID_FLEX == 0, "invalid initialization");
	assert(num_cells < GUI_GRID_MAX_CELLS);
	pgui_col_cells(gui, width, cells, num_cells);
}

void pgui_col_cells(gui_t *gui, s32 width, const s32 *cells, u32 num_cells)
{
	assert(gui->grid);
	gui__grid_add_strip(gui->grid, true, width, cells, num_cells, gui->scale);
}

void pgui_col_empty(gui_t *gui, s32 width)
{
	pgui_col(gui, width, 1);
	pgui_spacer_blank(gui);
}

void pgui_col_centered(gui_t *gui, s32 width, s32 height)
{
	const s32 cells[3] = { 0, height, 0 };
	static_assert(GUI_GRID_FLEX == 0, "invalid initialization");
	assert(countof(cells) < GUI_GRID_MAX_CELLS);
	pgui_col_cells(gui, width, B2PC(cells));
}

void pgui_cell(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	assert(gui->grid);
	gui__grid_get_next_cell(gui->grid, x, y, w, h);
}

void pgui_cell_consume(gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	assert(gui->grid);
	gui__grid_get_next_cell(gui->grid, x, y, w, h);
	gui__grid_advance_cell(gui->grid);
}

void pgui_cell_merge_next(gui_t *gui)
{
	assert(gui->grid);
	assert(gui->grid->depth > 0);
	gui_grid_strip_t *strip = &gui->grid->strips[gui->grid->depth - 1];
	if (strip->current_cell + 1 < strip->max_cell) {
		strip->current_cell[0] += strip->current_cell[1];
		for (s32 *cell = strip->current_cell + 2; cell != strip->max_cell; ++cell)
			cell[-1] = cell[0];
		--strip->max_cell;
	} else {
		assert(false);
	}
}

u64 pgui_next_widget_id(const gui_t *gui)
{
	/* TODO(rgriege): base panel widget id off relative position to panel
	 * without scroll, so scrolling doesn't reset focus id */
	s32 x, y;
	pgui_cell(gui, &x, &y, NULL, NULL);
	return gui_widget_id(gui, x, y);
}

void pgui_spacer(gui_t *gui)
{
	const gui_panel_style_t *style = &gui->style.panel;
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	gui_rect(gui, x, y, w, h, style->cell_bg_color, style->cell_border_color);
}

void pgui_spacer_blank(gui_t *gui)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	gui_widget_bounds_extend(gui, x, y, w, h);
}

void pgui_txt(gui_t *gui, const char *str)
{
	const gui_panel_style_t *style = &gui->style.panel;
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	gui_rect(gui, x, y, w, h, style->cell_bg_color, style->cell_border_color);
	gui_txt_styled(gui, x, y, w, h, str, &gui->style.txt);
}

void pgui_img(gui_t *gui, const gui_img_t *img, gui_img_scale_e scale)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	gui_img_boxed(gui, x, y, w, h, img, scale);
}

s32 pgui_btn_txt(gui_t *gui, const char *lbl)
{
	gui_btn_e result;
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_txt(gui, x, y, w, h, lbl);
	if (result == GUI_BTN_PRESS && gui->popup)
		gui->popup->close_at_end = true;
	return result;
}

s32 pgui_btn_img(gui_t *gui, const gui_img_t *img, gui_img_scale_e scale)
{
	gui_btn_e result;
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_img(gui, x, y, w, h, img, scale);
	if (result == GUI_BTN_PRESS && gui->popup)
		gui->popup->close_at_end = true;
	return result;
}

s32 pgui_btn_pen(gui_t *gui, gui_pen_t pen)
{
	gui_btn_e result;
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_pen(gui, x, y, w, h, pen);
	if (result == GUI_BTN_PRESS && gui->popup)
		gui->popup->close_at_end = true;
	return result;
}

s32 pgui_btn_color(gui_t *gui, color_t color)
{
	gui_btn_e result;
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_color(gui, x, y, w, h, color);
	if (result == GUI_BTN_PRESS && gui->popup)
		gui->popup->close_at_end = true;
	return result;
}

b32 pgui_chk(gui_t *gui, const char *lbl, b32 *val)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_chk(gui, x, y, w, h, lbl, val);
}

b32 pgui_chk_pen(gui_t *gui, gui_pen_t pen, b32 *val)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_chk_pen(gui, x, y, w, h, pen, val);
}

s32 pgui_npt_txt(gui_t *gui, char *lbl, u32 n, const char *hint,
                 gui_npt_flags_e flags)
{
	return pgui_npt_txt_ex(gui, lbl, n, hint, flags, &g_gui_npt_filter_print);
}

s32 pgui_npt_txt_ex(gui_t *gui, char *lbl, u32 n, const char *hint,
                    gui_npt_flags_e flags, gui_npt_filter_p filter)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_npt_txt_ex(gui, x, y, w, h, lbl, n, hint, flags, filter);
}

s32 pgui_npt_val(gui_t *gui, const char *txt, u32 n,
                 gui_npt_flags_e flags, gui_npt_filter_p filter)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_npt_val(gui, x, y, w, h, txt, n, flags, filter);
}

b32 pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_select(gui, x, y, w, h, lbl, val, opt);
}

b32 pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_mselect(gui, x, y, w, h, txt, val, opt);
}

b32 pgui_dropdown_begin(gui_t *gui, u32 *val, u32 num_items)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_dropdown_begin(gui, x, y, w, h, val, num_items);
}

b32 pgui_dropdown_item(gui_t *gui, u32 id, const char *txt)
{
	return gui_dropdown_item(gui, id, txt);
}

void pgui_dropdown_end(gui_t *gui)
{
	gui_dropdown_end(gui);
}

b32 pgui_slider_x(gui_t *gui, r32 *val)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_slider_x(gui, x, y, w, h, val);
}

b32 pgui_slider_y(gui_t *gui, r32 *val)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_slider_y(gui, x, y, w, h, val);
}

b32 pgui_range_x(gui_t *gui, r32 *val, r32 min, r32 max)
{
	r32 slider_val = (*val - min) / (max - min);
	b32 result = pgui_slider_x(gui, &slider_val);
	*val = (max - min) * slider_val + min;
	return result;
}

b32 pgui_range_y(gui_t *gui, r32 *val, r32 min, r32 max)
{
	r32 slider_val = (*val - min) / (max - min);
	b32 result = pgui_slider_y(gui, &slider_val);
	*val = (max - min) * slider_val + min;
	return result;
}

b32 pgui_menu_begin(gui_t *gui, const char *txt, s32 item_w, u32 num_items)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_menu_begin(gui, x, y, w, h, txt, item_w, num_items);
}

void pgui_menu_end(gui_t *gui)
{
	gui_menu_end(gui);
}

b32 pgui_color_picker_sv(gui_t *gui, colorf_t *color)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker_sv(gui, x, y, w, h, color);
}

b32 pgui_color_picker_h(gui_t *gui, colorf_t *color)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker_h(gui, x, y, w, h, color);
}

b32 pgui_color_picker(gui_t *gui, s32 pw, s32 ph, colorf_t *color)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker(gui, x, y, w, h, pw, ph, color);
}

b32 pgui_color_picker8(gui_t *gui, s32 pw, s32 ph, color_t *color)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker8(gui, x, y, w, h, pw, ph, color);
}

void pgui_scroll_area_begin(gui_t *gui, gui_scroll_area_t *scroll_area)
{
	s32 x, y, w, h;
	pgui_cell_consume(gui, &x, &y, &w, &h);
	gui_scroll_area_begin(gui, x, y, w, h, scroll_area);
}

void pgui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area)
{
	gui_scroll_area_end(gui, scroll_area);
}

void pgui_tree_begin(gui_t *gui, gui_tree_node_t *nodes, u32 max_nodes,
                     s32 row_height, s32 indent)
{
	assert(!gui->tree.nodes);
	gui->tree.nodes = nodes;
	gui->tree.max_nodes = max_nodes;
	gui->tree.row_height = row_height;
	gui->tree.indent = indent;
	gui->tree.node_iter = 0;
	gui->tree.depth = 0;
	gui->tree.insert_nodes_at_depth = ~0;
}

void pgui_tree_end(gui_t *gui)
{
	assert(gui->tree.depth == 0);
	gui->tree.nodes = NULL;
}

static
b32 pgui__tree_node_empty(const gui_tree_node_t *node)
{
	return node->depth == 0 && node->expanded == 0;
}

static
u32 pgui__tree_num_nodes_remaining(const gui_tree_t *tree)
{
	u32 i = tree->node_iter;
	while (i < tree->max_nodes && !pgui__tree_node_empty(&tree->nodes[i]))
		++i;
	return i - tree->node_iter;
}

static
gui_tree_node_t *pgui__tree_get_node(gui_tree_t *tree, gui_tree_node_t *empty_node)
{
	assert(tree->depth < sizeof(empty_node->depth) << 8);
	if (tree->depth == tree->insert_nodes_at_depth) {
		const u32 num_nodes = tree->node_iter + pgui__tree_num_nodes_remaining(tree);
		if (num_nodes > 0)
			for (u32 i = num_nodes - 1; i > tree->node_iter; --i)
				tree->nodes[i] = tree->nodes[i-1];
		tree->nodes[tree->node_iter].depth = tree->depth;
		tree->nodes[tree->node_iter].expanded = false;
		return &tree->nodes[tree->node_iter++];
	} else if (tree->node_iter == tree->max_nodes) {
		return empty_node;
	} else {
#ifdef DEBUG
		static b32 s_warned = false;
		if (   tree->nodes[tree->node_iter].depth != 0
		    && tree->nodes[tree->node_iter].depth != tree->depth
		    && !s_warned) {
			log_warn("too few gui tree nodes");
			s_warned = true;
		}
#endif
		return &tree->nodes[tree->node_iter++];
	}
}

b32 pgui_tree_node_begin(gui_t *gui)
{
	gui_tree_t *tree = &gui->tree;
	gui_tree_node_t empty_node = {0};
	gui_tree_node_t *node = pgui__tree_get_node(tree, &empty_node);
	b32 expanded = node->expanded;
	gui_pen_t pen = expanded ? gui_pen_tree_collapse : gui_pen_tree_restore;
	u32 lock;

	if (tree->depth > 0) {
		const s32 cols[3] = { tree->depth * tree->indent, tree->indent, 0 };
		pgui_row_cellsv(gui, tree->row_height, cols);
		pgui_spacer_blank(gui);
	} else {
		const s32 cols[2] = { tree->indent, 0 };
		pgui_row_cellsv(gui, tree->row_height, cols);
	}

	gui_style_push(gui, chk, gui_style(gui)->tree);
	gui_lock_if(gui, node == &empty_node, &lock);
	if (pgui_chk_pen(gui, pen, &expanded)) {
		if (expanded) {
			++tree->depth;
			tree->insert_nodes_at_depth = tree->depth;
		} else {
			const u32 num_nodes = tree->node_iter + pgui__tree_num_nodes_remaining(tree);
			u32 num_to_remove = 0;
			for (u32 i = tree->node_iter; i < tree->max_nodes && tree->nodes[i].depth > tree->depth; ++i)
				++num_to_remove;
			buf_remove_n(tree->nodes, tree->node_iter, num_to_remove, num_nodes);
			memset(&tree->nodes[num_nodes-num_to_remove], 0, num_to_remove * sizeof(gui_tree_node_t));
		}
		node->expanded = expanded;
	} else if (expanded) {
		++tree->depth;
	}
	gui_unlock_if(gui, lock);
	gui_style_pop(gui);

	pgui_col(gui, 0, 1);

	return expanded;
}

void pgui_tree_node_end(gui_t *gui)
{
	gui_tree_t *tree = &gui->tree;
	if (tree->insert_nodes_at_depth == tree->depth)
		tree->insert_nodes_at_depth = ~0;
	--tree->depth;
}

void pgui_tree_leaf(gui_t *gui)
{
	const gui_tree_t *tree = &gui->tree;
	const s32 cols[2] = { (tree->depth + 1) * tree->indent, 0 };
	pgui_row_cellsv(gui, tree->row_height, cols);
	pgui_spacer_blank(gui);
	pgui_col(gui, 0, 1);
}

void gui_pen_tree_collapse(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           const gui_element_style_t *style)
{
	gui_pen_panel_collapse(gui, x, y, w, h, style);
}

void gui_pen_tree_restore(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                          const gui_element_style_t *style)
{
	const v2f tri[3] = {
		{ x + 2 * w / 5, y +     h / 3 },
		{ x + 2 * w / 5, y + 2 * h / 3 },
		{ x + 3 * w / 5, y +     h / 2 },
	};
	gui_pen_rect(gui, x, y, w, h, style);
	gui_polyf(gui, tri, 3, style->line.color, g_nocolor);
}

void gui_set_splits(gui_t *gui, gui_split_t splits[], u32 num_splits)
{
	memset(splits, 0, num_splits * sizeof(splits[0]));
	gui->splits     = splits;
	gui->num_splits = num_splits;
	gui->root_split = NULL;
}

static
b32 gui__get_free_splits(gui_t *gui, u32 n, gui_split_t *splits[])
{
	u32 n_found = 0;
	for (u32 i = 0; i < gui->num_splits; ++i) {
		if (!gui->splits[i].in_use) {
			splits[n_found++] = &gui->splits[i];
			if (n_found == n)
				break;
		}
	}
	return n_found == n;
}

static
b32 gui__has_free_splits(gui_t *gui, u32 n)
{
	u32 n_found = 0;
	for (u32 i = 0; i < gui->num_splits; ++i)
		if (!gui->splits[i].in_use)
			++n_found;
	return n_found >= n;
}

static
void gui__split_init(gui_split_t *split, gui_split_flags_e flags,
                     gui_split_t *parent, r32 size)
{
	split->in_use            = true;
	split->flags             = flags;
	split->parent            = parent;
	split->siblings.next     = NULL;
	split->siblings.prev     = NULL;
	split->children.first    = NULL;
	split->children.last     = NULL;
	split->children.vertical = false;
	split->size              = size;
	split->box               = (box2i){0};
	split->panel             = NULL;

	if (parent) {
		if (parent->children.last) {
			parent->children.last->siblings.next = split;
			split->siblings.prev = parent->children.last;
		} else {
			parent->children.first = split;
		}
		parent->children.last  = split;
	}
}

static
void gui__split_clear(gui_split_t *split)
{
	split->in_use            = false;
	split->flags             = 0;
	split->parent            = NULL;
	split->siblings.next     = NULL;
	split->siblings.prev     = NULL;
	split->children.first    = NULL;
	split->children.last     = NULL;
	split->children.vertical = false;
	split->size              = 0;
	split->box               = (box2i){0};
	split->panel             = NULL;
}

b32 gui_create_root_split(gui_t *gui, gui_split_t **root)
{
	gui_split_t *split = NULL;

	if (gui->root_split) {
		assert(false);
		return false;
	}

	if (!gui__get_free_splits(gui, 1, &split))
		return false;

	gui__split_init(split, 0, NULL, 0);
	gui->root_split = split;
	if (root)
		*root = split;
	return true;
}

b32 gui_split2h(gui_t *gui, gui_split_t *parent, gui_split_t **left, r32 size,
                gui_split_t **right, gui_split_flags_e flags)
{
	const r32 size0 = max(size,  0.f);
	const r32 size1 = max(-size, 0.f);
	gui_split_t *children[2];
	b32 success = false;

	if (!gui__get_free_splits(gui, 2, children))
		goto out;

	parent->flags             |= GUI_SPLIT_DIVISIBLE;
	parent->children.vertical  = false;
	gui__split_init(children[0], flags, parent, size0);
	gui__split_init(children[1], flags, parent, size1);
	success = true;

out:
	if (left)
		*left = children[0];
	if (right)
		*right = children[1];
	return success;
}

b32 gui_split2v(gui_t *gui, gui_split_t *parent, gui_split_t **top, r32 size,
                gui_split_t **bottom, gui_split_flags_e flags)
{
	const r32 size0 = max(size,  0.f);
	const r32 size1 = max(-size, 0.f);
	gui_split_t *children[2];
	b32 success = false;

	if (!gui__get_free_splits(gui, 2, children))
		goto out;

	parent->flags             |= GUI_SPLIT_DIVISIBLE;
	parent->children.vertical  = true;
	gui__split_init(children[0], flags, parent, size0);
	gui__split_init(children[1], flags, parent, size1);
	success = true;

out:
	if (top)
		*top = children[0];
	if (bottom)
		*bottom = children[1];
	return success;
}

b32 gui_splitNh(gui_t *gui, gui_split_t *parent, gui_split_t **pchild,
                r32 size, gui_split_flags_e flags)
{
	gui_split_t *child = NULL;
	b32 success = false;

	if (!gui__get_free_splits(gui, 1, &child))
		goto out;

	parent->flags             |= GUI_SPLIT_DIVISIBLE;
	parent->children.vertical  = false;
	gui__split_init(child, flags, parent, size);
	success = true;

out:
	if (pchild)
		*pchild = child;
	return success;
}

b32 gui_splitNv(gui_t *gui, gui_split_t *parent, gui_split_t **pchild,
                r32 size, gui_split_flags_e flags)
{
	gui_split_t *child = NULL;
	b32 success = false;

	if (!gui__get_free_splits(gui, 1, &child))
		goto out;

	parent->flags             |= GUI_SPLIT_DIVISIBLE;
	parent->children.vertical  = true;
	gui__split_init(child, flags, parent, size);
	success = true;

out:
	if (pchild)
		*pchild = child;
	return success;
}

static
b32 gui__split_is_leaf(const gui_split_t *split)
{
	return !split->children.first;
}

static
b32 gui__split_has_one_child(const gui_split_t *split)
{
	return !gui__split_is_leaf(split)
	    && split->children.first == split->children.last;
}

static
b32 gui__split_has_two_children(const gui_split_t *split)
{
	return !gui__split_is_leaf(split)
	    && split->children.first->siblings.next == split->children.last;
}

static
b32 gui__split_collapsed(const gui_split_t *split)
{
	return split->panel && split->panel->collapsed;
}

static
b32 gui__find_split_from(gui_split_t *split, v2i point, gui_split_t **splitp)
{
	if (!gui__split_is_leaf(split)) {
		gui_split_t *child = split->children.first;
		while (child) {
			if (gui__find_split_from(child, point, splitp))
				return true;
			child = child->siblings.next;
		}
		return false;
	} else if (box2i_contains_point(split->box, point)) {
		*splitp = split;
		return true;
	} else {
		return false;
	}
}

static
b32 gui__find_split(gui_t *gui, v2i point, gui_split_t **split)
{
	return gui__find_split_from(gui->root_split, point, split);
}

typedef enum gui__split_division_type
{
	GUI__SPLIT_DIVISION_NONE,
	GUI__SPLIT_DIVISION_CENTER,
	GUI__SPLIT_DIVISION_LEFT,
	GUI__SPLIT_DIVISION_RIGHT,
	GUI__SPLIT_DIVISION_TOP,
	GUI__SPLIT_DIVISION_BOTTOM,
} gui__split_division_type_e;

static
s32 gui__split_division(const gui_split_t *split, v2i point)
{
	s32 x, y, w, h, xm, ym, dx, dy, s;

	box2i_to_xywh(split->box, &x, &y, &w, &h);

	xm = x+w/2;
	ym = y+h/2;
	dx = abs(point.x - xm);
	dy = abs(point.y - ym);
	s = min(w/4, h/4)/2;
	if (dx < s && dy < s) {
		return GUI__SPLIT_DIVISION_CENTER;
	} else if (dx > dy) {
		if (dx > 2*s)
			return GUI__SPLIT_DIVISION_NONE;
		else if (point.x <= xm)
			return GUI__SPLIT_DIVISION_LEFT;
		else
			return GUI__SPLIT_DIVISION_RIGHT;
	} else {
		if (dy > 2*s)
			return GUI__SPLIT_DIVISION_NONE;
		else if (point.y > ym)
			return GUI__SPLIT_DIVISION_TOP;
		else
			return GUI__SPLIT_DIVISION_BOTTOM;
	}
	return GUI__SPLIT_DIVISION_NONE;
}

static
void gui__split_render(gui_t *gui, gui_split_t *split, b32 *changed);

static
void gui__split_render_branch(gui_t *gui, gui_split_t *split, b32 *changed)
{
	s32 x, y, w, h;
	s32 cx, cy, cw, ch;
	gui_split_t *child;

	assert(split->children.first);

	box2i_to_xywh(split->box, &x, &y, &w, &h);

	if (   gui__split_has_two_children(split)
	    && (split->children.first->flags & GUI_SPLIT_RESIZABLE)
	    && (split->children.last->flags & GUI_SPLIT_RESIZABLE)
	    && !*changed
	    && !mouse_covered(gui)) {
		child = split->children.first;
		box2i_to_xywh(child->box, &cx, &cy, &cw, &ch);
		if (!split->children.vertical) {
			const s32 orig_drag_x = cx + cw - GUI_SPLIT_RESIZE_BORDER / 2;
			s32 drag_x = orig_drag_x;
			if (gui__resize_horiz(gui, &drag_x, cy, GUI_SPLIT_RESIZE_BORDER, ch)) {
				if (fabsf(child->size) > 1.f)
					child->size += drag_x - orig_drag_x;
				else
					child->size = (r32)(drag_x + GUI_SPLIT_RESIZE_BORDER / 2 - cx) / w;
				*changed = true;
			}
		} else {
			const s32 orig_drag_y = cy - GUI_SPLIT_RESIZE_BORDER / 2;
			s32 drag_y = orig_drag_y;
			if (gui__resize_vert(gui, cx, &drag_y, cw, GUI_SPLIT_RESIZE_BORDER)) {
				if (fabsf(child->size) > 1.f)
					child->size += orig_drag_y - drag_y;
				else
					child->size = (r32)(drag_y + GUI_SPLIT_RESIZE_BORDER / 2) / h;
				*changed = true;
			}
		}
	}

	if (split->children.vertical) {
		child = split->children.first->siblings.next;
		while (child) {
			const s32 ly = child->box.max.y;
			gui_line_styled(gui, x, ly, x + w, ly, &gui->style.split);
			child = child->siblings.next;
		}
	} else {
		child = split->children.first->siblings.next;
		while (child) {
			const s32 lx = child->box.min.x;
			gui_line_styled(gui, lx, y, lx, y + h, &gui->style.split);
			child = child->siblings.next;
		}
	}

	child = split->children.first;
	while (child) {
		gui__split_render(gui, child, changed);
		child = child->siblings.next;
	}
}

static
void gui__split_render_leaf(gui_t *gui, const gui_split_t *split)
{
	s32 x, y, w, h, xm, ym, s;
	v2i mouse;

	if (!gui->is_dragging_dockable_panel || !gui__has_free_splits(gui, 2))
		return;

	mouse_pos(gui, &mouse.x, &mouse.y);
	if (!box2i_contains_point(split->box, mouse))
		return;

	box2i_to_xywh(split->box, &x, &y, &w, &h);
	xm = x+w/2;
	ym = y+h/2;
	s = min(w/4, h/4)/2;

	switch (gui__split_division(split, mouse)) {
	case GUI__SPLIT_DIVISION_NONE:
	break;
	case GUI__SPLIT_DIVISION_CENTER:
		gui_rect(gui, xm-s, ym-s, 2*s, 2*s, g_nocolor, g_white);
	break;
	case GUI__SPLIT_DIVISION_LEFT:
	case GUI__SPLIT_DIVISION_RIGHT:
		if (split->flags & GUI_SPLIT_DIVISIBLE)
			gui_line(gui, xm, y, xm, y + h, 1, g_white);
	break;
	case GUI__SPLIT_DIVISION_TOP:
	case GUI__SPLIT_DIVISION_BOTTOM:
		if (split->flags & GUI_SPLIT_DIVISIBLE)
			gui_line(gui, x, ym, x + w, ym, 1, g_white);
	break;
	}
}

static
void gui__split_render(gui_t *gui, gui_split_t *split, b32 *changed)
{
	assert(split);

	if (!gui__split_is_leaf(split))
		gui__split_render_branch(gui, split, changed);
	else
		gui__split_render_leaf(gui, split);
}

static
void gui__splits_render(gui_t *gui, b32 *changed)
{
	gui->splits_rendered_this_frame = true;

	gui_style_push(gui, drag.inactive.bg_color, g_nocolor);
	gui_style_push(gui, drag.hot.bg_color, g_nocolor);
	gui_style_push(gui, drag.active.bg_color, g_nocolor);

	*changed = false;
	gui__split_render(gui, gui->root_split, changed);

	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);
}

void gui_splits_render(gui_t *gui)
{
	b32 changed;
	gui__splits_render(gui, &changed);
}

static
s32 gui__split_dim(r32 size, r32 parent_dimension, s32 scale,
                   r32 total_sized_child_dimension,
                   u32 num_siblings_without_size)
{
	if (size > 1.f) {
		return min(gui__scale_val(size, scale), parent_dimension);
	} else if (size > 0.f) {
		return parent_dimension * size;
	} else {
		assert(parent_dimension >= total_sized_child_dimension);
		assert(num_siblings_without_size > 0);
		return (parent_dimension - total_sized_child_dimension) / num_siblings_without_size;
	}
}

static
void gui__splits_compute_from(gui_split_t *split, s32 scale)
{
	const b32 vertical = split->children.vertical;
	const s32 collapsed_dimension = gui__scale_val(GUI_PANEL_TITLEBAR_HEIGHT, scale);
	const v2i start = { split->box.min.x, split->box.max.y };
#if GUI_SPLIT_ALIGN_LAST_TO_END
	const v2i end = { split->box.max.x, split->box.min.y };
#endif
	const v2i dir = { 1, -1 };
	const v2i dim = box2i_get_extent(split->box);
	v2i cpos;
	gui_split_t *child;
	u32 num_children;
	u32 num_children_collapsed;
	u32 num_children_without_size;
	s32 total_sized_child_dimension;
	u32 num_children_remaining;
	u32 num_children_collapsed_remaining;

	assert(!gui__split_is_leaf(split));

	num_children = 0;
	num_children_collapsed = 0;
	num_children_without_size = 0;
	total_sized_child_dimension = 0;
	child = split->children.first;
	while (child) {
		if (gui__split_collapsed(child)) {
			++num_children_collapsed;
			total_sized_child_dimension += collapsed_dimension;
		} else if (child->size > 0.f) {
			total_sized_child_dimension += gui__split_dim(child->size, dim.d[vertical],
			                                              scale, 0.f, 0);
		} else {
			++num_children_without_size;
		}
		++num_children;
		child = child->siblings.next;
	}
	total_sized_child_dimension = min(total_sized_child_dimension, dim.d[vertical]);

	cpos = start;
	num_children_remaining = num_children;
	num_children_collapsed_remaining = num_children_collapsed;
	child = split->children.first;
	while (child) {
		v2i cdim = dim;
		v2i cend;

		if (gui__split_collapsed(child)) {
			cdim.d[vertical] = collapsed_dimension;
			--num_children_collapsed_remaining;
#if GUI_SPLIT_ALIGN_LAST_TO_END
		} else if (num_children_remaining == num_children_collapsed_remaining + 1) {
			/* expand to fill available space */
			const s32 remaining = v2i_mul(v2i_sub(end, cpos), dir).d[vertical]
			                    - num_children_collapsed_remaining * collapsed_dimension;
			cdim.d[vertical] = gui__split_dim(child->size, dim.d[vertical], scale,
			                                  total_sized_child_dimension,
			                                  num_children_without_size);
			cdim.d[vertical] = max(cdim.d[vertical], remaining);
#endif
		} else {
			cdim.d[vertical] = gui__split_dim(child->size, dim.d[vertical], scale,
			                                  total_sized_child_dimension,
			                                  num_children_without_size);
		}

		cend = box2i_clamp_point(split->box, v2i_add(cpos, v2i_mul(cdim, dir)));
		box2i_from_point(&child->box, cpos);
		box2i_extend_point(&child->box, cend);

		cdim.d[!vertical] = 0;
		cpos = box2i_clamp_point(split->box, v2i_add(cpos, v2i_mul(cdim, dir)));

		--num_children_remaining;
		child = child->siblings.next;
	}

	child = split->children.first;
	while (child) {
		if (!gui__split_is_leaf(child))
			gui__splits_compute_from(child, scale);
		child = child->siblings.next;
	}
}

void gui_splits_compute(gui_t *gui)
{
	assert(gui->root_split);
	box2i_from_xywh(&gui->root_split->box, 0, 0, gui->window_dim.x, gui->window_dim.y);
	if (!gui__split_is_leaf(gui->root_split))
		gui__splits_compute_from(gui->root_split, gui->scale);
}

static
void pgui__panel_compute_scaled_dimensions(const gui_t *gui, gui_panel_t *panel)
{
	const s32  x = panel->unscaled.x;
	const s32  y = panel->unscaled.y;
	const s32 uw = panel->unscaled.w;
	const s32 uh = panel->unscaled.h;
	const s32 sw = gui_scale_val(gui, uw);
	const s32 sh = gui_scale_val(gui, uh);
	v2i uanchor, sanchor;

	gui_align_anchor(x, y, uw, uh, panel->scale_anchor, &uanchor.x, &uanchor.y);
	gui_align_anchor(x, y, sw, sh, panel->scale_anchor, &sanchor.x, &sanchor.y);

	panel->x = x + uanchor.x - sanchor.x;
	panel->y = y + uanchor.y - sanchor.y;
	panel->w = sw;
	panel->h = sh;

	if (panel->flags & GUI_PANEL_TITLEBAR) {
		/* Ensure we don't scale the drag handle to a position off the screen. */
		if (panel->x < 0)
			panel->x = 0;
		if (panel->y + panel->h > gui->window_dim.y)
			panel->y = gui->window_dim.y - panel->h;
	}
}

static
void pgui__panel_compute_unscaled_dimensions(const gui_t *gui, gui_panel_t *panel)
{
	const s32  x = panel->x;
	const s32  y = panel->y;
	const s32 sw = panel->w;
	const s32 sh = panel->h;
	const s32 uw = gui_scale_val_inverse(gui, sw);
	const s32 uh = gui_scale_val_inverse(gui, sh);
	v2i uanchor, sanchor;

	gui_align_anchor(x, y, uw, uh, panel->scale_anchor, &uanchor.x, &uanchor.y);
	gui_align_anchor(x, y, sw, sh, panel->scale_anchor, &sanchor.x, &sanchor.y);

	panel->unscaled.x = x + sanchor.x - uanchor.x;
	panel->unscaled.y = y + sanchor.y - uanchor.y;
	panel->unscaled.w = uw;
	panel->unscaled.h = uh;
}

void pgui__panel_init(gui_t *gui, gui_panel_t *panel, u32 id,
                      s32 x, s32 y, s32 w, s32 h,
                      const char *title, gui_panel_flags_e flags)
{
	assert(id <= GUI__MAX_PANEL_ID);
	assert(!(flags & GUI_PANEL_TITLEBAR) || title);

	panel->unscaled.x = x;
	panel->unscaled.y = y;
	panel->unscaled.w = w;
	panel->unscaled.h = h;
	pgui__panel_compute_scaled_dimensions(gui, panel);
	panel->id = id;

	panel->title = title;
	panel->flags = flags;

	memclr(panel->scroll_area);

	panel->prev   = NULL;
	panel->next   = NULL;
	panel->split  = NULL;

	panel->closed     = false;
	panel->collapsed  = false;
	panel->tabbed_out = false;
	panel->scale_anchor = GUI_ALIGN_MIDCENTER;
}

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, u32 id,
                     s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_e flags)
{
	pgui__panel_init(gui, panel, id, x, y, w, h, title, flags);
	pgui_panel_to_front(gui, panel);
}

void pgui_panel_init_centered(gui_t *gui, gui_panel_t *panel, u32 id,
                              s32 w, s32 h,
                              const char *title, gui_panel_flags_e flags)
{
	const s32 x = (gui->window_dim.x - w)/2;
	const s32 y = (gui->window_dim.y - h)/2;
	pgui__panel_init(gui, panel, id, x, y, w, h, title, flags);
}

void pgui_panel_init_in_split(gui_t *gui, gui_panel_t *panel, u32 id,
                              gui_split_t *split,
                              const char *title, gui_panel_flags_e flags)
{
	s32 x, y, w, h;

	assert(split);
	assert(flags & GUI_PANEL_DOCKABLE);

	box2i_to_xywh(split->box, &x, &y, &w, &h);
	pgui__panel_init(gui, panel, id, x, y, w, h, title, flags);
	panel->split = split;
	split->panel = panel;
}

static
void pgui__validate_tab_flags(gui_panel_flags_e flags0, gui_panel_flags_e flags1)
{
	assert(flags0 & GUI_PANEL_TITLEBAR);
	assert(flags1 & GUI_PANEL_TITLEBAR);
	assert((flags0 & GUI_PANEL_DRAGGABLE)   == (flags1 & GUI_PANEL_DRAGGABLE));
	assert((flags0 & GUI_PANEL_RESIZABLE)   == (flags1 & GUI_PANEL_RESIZABLE));
	assert((flags0 & GUI_PANEL_CLOSABLE)    == (flags1 & GUI_PANEL_CLOSABLE));
	assert((flags0 & GUI_PANEL_COLLAPSABLE) == (flags1 & GUI_PANEL_COLLAPSABLE));
}

void pgui_panel_add_tab(gui_panel_t *panel, gui_panel_t *tab)
{
	gui_panel_t *last;

	pgui__validate_tab_flags(panel->flags, tab->flags);

	last = panel;
	while (last->next)
		last = last->next;

	last->next = tab;
	tab->prev = last;
	tab->tabbed_out = true;
}

void pgui_panel_remove_tab(gui_t *gui, gui_panel_t *panel)
{
	gui_panel_t *next_tab = NULL;

	/* remove from neighbor linked list */
	if (panel->prev) {
		panel->prev->next = panel->next;
		next_tab = panel->prev;
	}
	if (panel->next) {
		panel->next->prev = panel->prev;
		next_tab = panel->next;
	}

	if (!panel->tabbed_out && next_tab)
		pgui_panel_select_tab(next_tab);

	/* remove from container split */
	if (panel->split) {
		gui_split_t *split = panel->split;
		split->panel = next_tab;
		panel->split = NULL;
		if (   !next_tab
		    && split != gui->root_split
		    && (split->flags & GUI_SPLIT_TEMPORARY)
		    && split->parent) {
			gui_split_t *parent = split->parent;
			if (gui__split_has_one_child(parent)) {
				gui__split_clear(parent->children.first);
				parent->children.first = NULL;
				parent->children.last  = NULL;
			} else if (gui__split_has_two_children(parent)) {
				if (parent->children.first == split) {
					if (parent->children.last->panel) {
						parent->panel = parent->children.last->panel;
						parent->children.last->panel->split = parent;
					}
				} else {
					if (parent->children.first->panel) {
						parent->panel = parent->children.first->panel;
						parent->children.first->panel->split = parent;
					}
				}
				gui__split_clear(parent->children.first);
				gui__split_clear(parent->children.last);
				parent->children.first = NULL;
				parent->children.last  = NULL;
			} else {
				if (split->siblings.prev)
					split->siblings.prev->siblings.next = split->siblings.next;
				if (split->siblings.next)
					split->siblings.next->siblings.prev = split->siblings.prev;
				if (parent->children.first == split)
					parent->children.first = split->siblings.next;
				if (parent->children.last == split)
					parent->children.last = split->siblings.prev;
			}
		}
	}

	/* reset data */
	panel->prev = NULL;
	panel->next = NULL;
	panel->tabbed_out = false;
}

void pgui_panel_select_tab(gui_panel_t *panel)
{
	gui_panel_t *first = panel;
	while (first->prev)
		first = first->prev;
	for (gui_panel_t *p = first; p; p = p->next)
		p->tabbed_out = true;
	panel->tabbed_out = false;
}

static
b32 gui__current_panel_contains_mouse(const gui_t *gui)
{
	const gui_panel_t *panel = gui->panel;
	box2i box;
	s32 y, height;

	if (panel->closed)
		return false;

	if (panel->collapsed) {
		const s32 titlebar_height = gui_scale_val(gui, GUI_PANEL_TITLEBAR_HEIGHT);
		y = panel->y + panel->h - titlebar_height;
		height = titlebar_height;
	} else {
		y = panel->y;
		height = panel->h;
	}

	box2i_from_xywh(&box, panel->x, y, panel->w, height);
	gui__mask_box(gui, &box);
	return box2i_contains_point(box, gui->mouse_pos);
}

static
void pgui__panel_resize(gui_t *gui, gui_panel_t *panel)
{
	const s32 min_dim = gui_scale_val(gui, GUI_PANEL_MIN_DIM);
	const s32 resize_border = gui_scale_val(gui, GUI_PANEL_RESIZE_BORDER);
	v2i resize;
	s32 resize_delta;

	if (   (panel->flags & GUI_PANEL_RESIZABLE)
	    && !panel->split
	    && !panel->collapsed
	    && gui->cursor == GUI_CURSOR_DEFAULT) {
		gui_style_push(gui, drag, g_gui_style_invis.drag);

		resize.x = panel->x - resize_border;
		if (gui__resize_horiz(gui, &resize.x, panel->y, resize_border, panel->h)) {
			resize_delta = panel->x - resize_border - resize.x;
			if (panel->w + resize_delta < min_dim) {
				const r32 dx = panel->w - min_dim;
				panel->w = min_dim;
				panel->x += dx;
				gui->active_id = gui_widget_id(gui, panel->x - resize_border, panel->y);
			} else {
				panel->w += resize_delta;
				panel->x -= resize_delta;
			}
			pgui__panel_compute_unscaled_dimensions(gui, panel);
		}

		resize.x = panel->x + panel->w;
		if (gui__resize_horiz(gui, &resize.x, panel->y, resize_border, panel->h)) {
			panel->w += resize.x - (panel->x + panel->w);
			if (panel->w < min_dim) {
				panel->w = min_dim;
				gui->active_id = gui_widget_id(gui, panel->x + panel->w, panel->y);
			}
			pgui__panel_compute_unscaled_dimensions(gui, panel);
		}

		resize.y = panel->y - resize_border;
		if (gui__resize_vert(gui, panel->x, &resize.y, panel->w, resize_border)) {
			resize_delta = panel->y - resize_border - resize.y;
			if (panel->h + resize_delta < min_dim) {
				const r32 dy = panel->h - min_dim;
				panel->h = min_dim;
				panel->y += dy;
				gui->active_id = gui_widget_id(gui, panel->x, panel->y - resize_border);
			} else {
				panel->h += resize_delta;
				panel->y -= resize_delta;
			}
			pgui__panel_compute_unscaled_dimensions(gui, panel);
		}

		resize.y = panel->y + panel->h;
		if (gui__resize_vert(gui, panel->x, &resize.y, panel->w, resize_border)) {
			panel->h += resize.y - (panel->y + panel->h);
			if (panel->h < min_dim) {
				panel->h = min_dim;
				gui->active_id = gui_widget_id(gui, panel->x, panel->y + panel->h);
			}
			pgui__panel_compute_unscaled_dimensions(gui, panel);
		}

		gui_style_pop(gui);
	}
}

static
s32 box2i_to_point_dist_sq(box2i box, v2i point)
{
	s32 dx, dy;

	if (box2i_contains_point(box, point))
		return 0;

	if (point.x <= box.min.x)
		dx = box.min.x - point.x;
	else if (point.x >= box.max.x)
		dx = point.x - box.max.x;
	else
		dx = 0;

	if (point.y <= box.min.y)
		dy = box.min.y - point.y;
	else if (point.y >= box.max.y)
		dy = point.y - box.max.y;
	else
		dy = 0;

	return dx*dx + dy*dy;
}

static
void pgui__panel_titlebar(gui_t *gui, gui_panel_t *panel, b32 *dragging)
{
	const s32 dim = gui_scale_val(gui, GUI_PANEL_TITLEBAR_HEIGHT);
	s32 y;
	s32 rw, rx;
	s32 tab_count;
	gui_panel_t *panel_to_remove_from_tabs = NULL;

	y = panel->y + panel->h - dim;

	if (panel->flags & GUI_PANEL_DRAGGABLE) {
		const b32 contains_mouse = gui__widget_contains_mouse(gui, panel->x, y, dim, dim);
		*dragging = gui_drag(gui, &panel->x, &y, contains_mouse, MB_LEFT);
		if (*dragging) {
			panel->y = y - panel->h + dim;
			pgui__panel_compute_unscaled_dimensions(gui, panel);
			if (panel->split) {
				pgui_panel_remove_tab(gui, panel);
				pgui_panel_to_front(gui, panel);
				pgui__panel_titlebar(gui, panel, dragging);
				return;
			}
		}
	} else {
		*dragging = false;
	}

	gui_rect(gui, panel->x, y, panel->w,
	         dim, gui->style.panel.titlebar.bg_color,
	         gui->style.panel.titlebar.outline_color);

	gui_style_push(gui, drag, gui->style.panel.drag);
	gui__drag_rect_render(gui, panel->x, y, dim, dim, *dragging);
	gui_style_pop(gui);

	rw = 0;
	if (panel->flags & GUI_PANEL_COLLAPSABLE)
		rw += dim;
	if (panel->flags & GUI_PANEL_CLOSABLE)
		rw += dim;

	tab_count = 0;
	for (gui_panel_t *p = panel; p; p = p->next)
		++tab_count;
	if (tab_count > 1) {
		const s32 max_tab_dim = gui_scale_val(gui, GUI_PANEL_MAX_TAB_WIDTH);
		s32 tab_dim, tab_idx;
		u32 selected_tab_idx;
		tab_dim = (panel->w - dim - rw) / tab_count;
		tab_dim = min(tab_dim, max_tab_dim);
		tab_idx = 0;
		selected_tab_idx = 0;
		for (gui_panel_t *p = panel; p; p = p->next) {
			if (!p->tabbed_out)
				selected_tab_idx = (u32)tab_idx;
			++tab_idx;
		}
		tab_idx = 0;
		gui_style_push(gui, select, gui->style.panel.tab);
		for (gui_panel_t *p = panel; p; p = p->next) {
			const s32 x = panel->x + dim + tab_idx * tab_dim;
			const u64 select_id = gui_widget_id(gui, x, y);
			if (gui_select(gui, x, y, tab_dim, dim, p->title,
			               &selected_tab_idx, (u32)tab_idx)) {
				pgui_panel_select_tab(p);
			} else if (   (p->flags & GUI_PANEL_DRAGGABLE)
			           && select_id == gui->active_id) {
				box2i box;
				box2i_from_xywh(&box, x, y, tab_dim, dim);
				if (box2i_to_point_dist_sq(box, gui->mouse_pos) > dim*dim) {
					s32 drag_y;
					gui->drag_offset.x = -dim/2;
					gui->drag_offset.y = -dim/2;
					p->x   = gui->mouse_pos.x - dim/2;
					drag_y = gui->mouse_pos.y - dim/2;
					p->y   = drag_y - p->h + dim;
					gui->active_id = gui__widget_id(gui, p->id, p->x, drag_y);
					pgui__panel_compute_unscaled_dimensions(gui, panel);
					panel_to_remove_from_tabs = p;
				}
			}
			++tab_idx;
		}
		gui_style_pop(gui);
	} else {
		gui_txt_styled(gui, panel->x + dim, y, panel->w, dim,
		               panel->title, &gui->style.panel.titlebar.text);
	}

	rx = panel->x + panel->w - rw;
	if (panel->flags & GUI_PANEL_COLLAPSABLE) {
		if (panel->collapsed)
			gui_style_push(gui, btn, gui->style.panel.restore);
		else
			gui_style_push(gui, btn, gui->style.panel.collapse);
		if (gui_btn_pen(gui, rx, y, dim, dim, gui->style.btn.pen) == GUI_BTN_PRESS)
			panel->collapsed = !panel->collapsed;
		gui_style_pop(gui);
		rx += dim;
	}

	if (panel->flags & GUI_PANEL_CLOSABLE) {
		gui_style_push(gui, btn, gui->style.panel.close);
		if (gui_btn_pen(gui, rx, y, dim, dim, gui->style.btn.pen) == GUI_BTN_PRESS)
			pgui_panel_close(gui, panel);
		gui_style_pop(gui);
		rx += dim;
	}

	if (panel_to_remove_from_tabs) {
		pgui_panel_remove_tab(gui, panel_to_remove_from_tabs);
		pgui_panel_to_front(gui, panel_to_remove_from_tabs);
		if (panel_to_remove_from_tabs == panel)
			pgui__panel_titlebar(gui, panel, dragging);
	}
}

static
void pgui__panel_stop_dragging(gui_t *gui, gui_panel_t *panel)
{
	gui_split_t *split, *child1, *child2;

	if (   !(panel->flags & GUI_PANEL_DOCKABLE)
	    || !gui->root_split
	    || !gui__has_free_splits(gui, 2)
	    || !gui__find_split(gui, gui->mouse_pos, &split))
		return;

	switch (gui__split_division(split, gui->mouse_pos)) {
	case GUI__SPLIT_DIVISION_NONE:
	break;
	case GUI__SPLIT_DIVISION_CENTER:
		if (split->panel) {
			pgui_panel_add_tab(split->panel, panel);
			pgui_panel_select_tab(panel);
		} else {
			panel->split = split;
			split->panel = panel;
		}
	break;
	case GUI__SPLIT_DIVISION_LEFT:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2h(gui, split, &child1, 0.5f, &child2, split->flags | GUI_SPLIT_TEMPORARY);
			child1->panel = panel;
			panel->split = child1;
			child2->panel = split->panel;
			if (split->panel)
				split->panel->split = child2;
			split->panel = NULL;
			gui__splits_compute_from(split, gui->scale);
		}
	break;
	case GUI__SPLIT_DIVISION_RIGHT:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2h(gui, split, &child1, 0.5f, &child2, split->flags | GUI_SPLIT_TEMPORARY);
			child1->panel = split->panel;
			if (split->panel)
				split->panel->split = child1;
			child2->panel = panel;
			panel->split = child2;
			split->panel = NULL;
			gui__splits_compute_from(split, gui->scale);
		}
	break;
	case GUI__SPLIT_DIVISION_TOP:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2v(gui, split, &child1, 0.5f, &child2, split->flags | GUI_SPLIT_TEMPORARY);
			child1->panel = panel;
			panel->split = child1;
			child2->panel = split->panel;
			if (split->panel)
				split->panel->split = child2;
			split->panel = NULL;
			gui__splits_compute_from(split, gui->scale);
		}
	break;
	case GUI__SPLIT_DIVISION_BOTTOM:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2v(gui, split, &child1, 0.5f, &child2, split->flags | GUI_SPLIT_TEMPORARY);
			child1->panel = split->panel;
			if (split->panel)
				split->panel->split = child1;
			child2->panel = panel;
			panel->split = child2;
			split->panel = NULL;
			gui__splits_compute_from(split, gui->scale);
		}
	break;
	}
}

s32 pgui_panel_body_height(const gui_t *gui, const gui_panel_t *panel)
{
	return (panel->flags & GUI_PANEL_TITLEBAR)
	     ? panel->h - gui__scale_val(GUI_PANEL_TITLEBAR_HEIGHT, gui->scale)
	     : panel->h;
}

static
void pgui__style_push_panel_scroll_area(gui_t *gui)
{
	const s32 scrollbar_track_width = min(gui->style.panel.padding.right/2,
	                                      gui->style.panel.padding.bottom/2);
	gui_style_push(gui, scroll_area.bg_color, gui->style.panel.bg_color);
	gui_style_push(gui, scroll_area.padding, gui->style.panel.padding);
	gui_style_push_s32(gui, scroll_area.scrollbar_track_width, scrollbar_track_width);
	gui_style_push(gui, scroll_area.scrollbar, gui->style.panel.scrollbar);
}

static
void pgui__style_pop_panel_scroll_area(gui_t *gui)
{
	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);
	gui_style_pop(gui);
}

b32 pgui_panel(gui_t *gui, gui_panel_t *panel)
{
	const gui_panel_style_t *style = &gui->style.panel;
	const b32 content_visible = pgui_panel_content_visible(panel);

	assert(!gui->panel);

	if (panel->split && !gui->splits_rendered_this_frame) {
		b32 changed = false;
		gui__splits_render(gui, &changed);
		if (changed)
			gui_splits_compute(gui);
	}

	if (panel->prev) {
		gui_panel_t *first = panel->prev;
		while (first->prev)
			first = first->prev;

		panel->unscaled.x  = first->unscaled.x;
		panel->unscaled.y  = first->unscaled.y;
		panel->unscaled.w  = first->unscaled.w;
		panel->unscaled.h  = first->unscaled.h;
		panel->x           = first->x;
		panel->y           = first->y;
		panel->w           = first->w;
		panel->h           = first->h;
		panel->split       = first->split;
		panel->closed      = first->closed;
		panel->collapsed   = first->collapsed;

		gui->panel = panel;
	} else if (panel->closed) {
		return false;
	} else {
		b32 was_dragging = false;
		b32 dragging = false;

		if (panel->split) {
			box2i_to_xywh(panel->split->box, &panel->x, &panel->y, &panel->w, &panel->h);
			pgui__panel_compute_unscaled_dimensions(gui, panel);
		} else if (gui->active_id == 0) {
			/* Don't update dimensions if the user is dragging or resizing the panel -
			 * it will fuck up the widget id's due to integer division rounding. */
			pgui__panel_compute_scaled_dimensions(gui, panel);
		}

		gui->panel = panel;

		/* must run after gui->panel is assigned so drag id is correct */
		if (panel->flags & GUI_PANEL_TITLEBAR) {
			const s32 titlebar_height = gui_scale_val(gui, GUI_PANEL_TITLEBAR_HEIGHT);
			const s32 drag_y = panel->y + panel->h - titlebar_height;
			const u64 drag_id = gui_widget_id(gui, panel->x, drag_y);
			was_dragging = gui_widget_active(gui, drag_id);
		}

		pgui__panel_resize(gui, panel);
		if (panel->flags & GUI_PANEL_TITLEBAR)
			pgui__panel_titlebar(gui, panel, &dragging);

		if (dragging)
			gui->is_dragging_dockable_panel = (panel->flags & GUI_PANEL_DOCKABLE);
		else if (was_dragging)
			pgui__panel_stop_dragging(gui, panel);

		if (panel->collapsed && !panel->split) {
			/* background outline display */
			const s32 h = gui_scale_val(gui, GUI_PANEL_TITLEBAR_HEIGHT);
			gui_rect(gui, panel->x, panel->y + panel->h - h, panel->w, h,
			         g_nocolor, style->border_color);
		}

		if (   dragging
		    || (   gui__allow_new_panel_interaction(gui)
		        && mouse_pressed(gui, MB_LEFT)
		        && gui__current_panel_contains_mouse(gui))) {
			pgui_panel_to_front(gui, panel);
		} else {
			panel->pri = ++gui->next_panel_pri;
			for (gui_panel_t *p = panel->next; p; p = p->next)
				p->pri = ++gui->next_panel_pri;
		}
	}

	if (   !content_visible
	    || !pgui_panel_content_visible(panel)
	    || panel->w <= 0
	    || panel->h <= 0) {
		gui->panel = NULL;
		return false;
	}

	/* background outline display */
	if (!panel->split)
		gui_rect(gui, panel->x, panel->y, panel->w, panel->h,
		         g_nocolor, style->border_color);

	if (panel->flags & GUI_PANEL_SCROLLABLE) {
		const s32 body_height = pgui_panel_body_height(gui, panel);
		pgui__style_push_panel_scroll_area(gui);
		gui_scroll_area_begin(gui, panel->x, panel->y, panel->w, body_height,
		                      &panel->scroll_area);
		pgui__style_pop_panel_scroll_area(gui);
	} else {
		const s32 body_height = pgui_panel_body_height(gui, panel);
		const gui_padding_style_t padding = gui__scale_padding(style->padding, gui->scale);
		box2i mask;

		mask.min.x = panel->x + padding.left;
		mask.min.y = panel->y + padding.bottom;
		mask.max.x = panel->x + panel->w - padding.right;
		mask.max.y = panel->y + body_height - padding.top;

		gui__mask_box(gui, &mask);
		gui_mask_push_box(gui, mask);
	}
	return true;
}

void pgui_panel_collapse(gui_panel_t *panel)
{
	assert(panel->flags & GUI_PANEL_COLLAPSABLE);
	if (!panel->collapsed)
		panel->collapsed = !panel->collapsed;
}

void pgui_panel_restore(gui_panel_t *panel)
{
	assert(panel->flags & GUI_PANEL_COLLAPSABLE);
	if (panel->collapsed)
		panel->collapsed = !panel->collapsed;
}

void pgui_panel_open(gui_t *gui, gui_panel_t *panel)
{
	assert(panel->closed);
	panel->closed = false;
	pgui_panel_to_front(gui, panel);
}

void pgui_panel_close(gui_t *gui, gui_panel_t *panel)
{
	assert(!panel->closed);
	panel->closed = true;
	pgui_panel_remove_tab(gui, panel);
}

void pgui_panel_toggle(gui_t *gui, gui_panel_t *panel)
{
	if (panel->closed)
		pgui_panel_open(gui, panel);
	else
		pgui_panel_close(gui, panel);
}

void pgui_panel_finish(gui_t *gui, gui_panel_t *panel)
{
	b32 contains_mouse;

	assert(gui->panel == panel);

	if (panel->flags & GUI_PANEL_SCROLLABLE) {
		pgui__style_push_panel_scroll_area(gui);
		gui_scroll_area_end(gui, &panel->scroll_area);
		pgui__style_pop_panel_scroll_area(gui);
	} else {
		const s32 body_height = pgui_panel_body_height(gui, panel);
		const gui_panel_style_t *style = &gui->style.panel;
		gui_mask_pop(gui);
		gui_rect(gui, panel->x, panel->y, panel->w, body_height, style->bg_color, g_nocolor);
	}

	contains_mouse = gui__current_panel_contains_mouse(gui);

	/* NOTE(rgriege): would be great to avoid the additional layer here,
	 * but otherwise the next widgets are on top of the panel bg & scrollbars */
	gui__layer_new(gui);

	if (contains_mouse && !mouse_covered(gui))
		mouse_cover(gui, gui_widget_id(gui, ~0, ~0));

	gui->panel = NULL;
}

b32 pgui_panel_content_visible(const gui_panel_t *panel)
{
	return !(panel->closed || panel->collapsed || panel->tabbed_out);
}

void pgui_panel_to_front(gui_t *gui, gui_panel_t *panel)
{
	gui_panel_t *last = panel;
	while (last->next)
		last = last->next;
	for (gui_panel_t *p = last; p; p = p->prev)
		p->pri = --gui->min_panel_pri;
}

int pgui_panel_sort(const void *lhs_, const void *rhs_)
{
	const gui_panel_t *lhs = lhs_, *rhs = rhs_;
	if ((lhs->split == rhs->split) || (lhs->split && rhs->split))
		return lhs->pri - rhs->pri;
	else if (lhs->split)
		return 1;
	else
		return -1;
}

int pgui_panel_sortp(const void *lhs_, const void *rhs_)
{
	const gui_panel_t *const *lhs = lhs_;
	const gui_panel_t *const *rhs = rhs_;
	return pgui_panel_sort(*lhs, *rhs);
}

void pgui_panel_grid_begin(gui_t *gui, gui_grid_flex_e flex)
{
	assert(gui->panel);

	if (gui->scroll_area) {
		pgui__style_push_panel_scroll_area(gui);
		pgui_scroll_area_grid_begin(gui, &gui->grid_panel, flex);
		pgui__style_pop_panel_scroll_area(gui);
	} else {
		const gui_panel_style_t *style = &gui->style.panel;
		const gui_padding_style_t padding = gui__scale_padding(style->padding, gui->scale);
		const gui_panel_t *panel = gui->panel;
		const s32 body_height = pgui_panel_body_height(gui, panel);
		v2i pos, dim;

		pos.x = panel->x + padding.left;
		pos.y = panel->y + padding.bottom;
		dim.x = panel->w - padding.left - padding.right;
		dim.y = body_height - padding.bottom - padding.top;

		if (flex & GUI_GRID_FLEX_HORIZONTAL)
			dim.x = 0;

		if (flex & GUI_GRID_FLEX_VERTICAL) {
			pos.y += dim.y;
			dim.y = 0;
		}

		pgui_grid_begin(gui, &gui->grid_panel, pos.x, pos.y, dim.x, dim.y);
	}
}

void pgui_panel_grid_end(gui_t *gui)
{
	assert(gui->panel);
	assert(gui->grid);
	assert(gui->grid == &gui->grid_panel);
	pgui_grid_end(gui, gui->grid);
}

u32 pgui_grid_depth(const gui_t *gui)
{
	assert(gui->grid);
	return gui->grid->depth;
}

void pgui_grid_dimensions(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	assert(gui->grid);
	if (x)
		*x = gui->grid->pos.x;
	if (y)
		*y = gui->grid->pos.y - gui->grid->dim.y;
	if (w)
		*w = gui->grid->dim.x;
	if (h)
		*h = gui->grid->dim.y;
}

void gui_pen_window_minimize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             const gui_element_style_t *style)
{
	gui_line_styled(gui, x + w / 4, y + 2 * h / 5, x + 3 * w / 4, y + 2 * h / 5,
	                &style->line);
}

void gui_pen_window_maximize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             const gui_element_style_t *style)
{
	gui_rect(gui, x + w / 4, y + h / 4, w / 2, h / 2, g_nocolor, style->line.color);
}

void gui_pen_window_close(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                          const gui_element_style_t *style)
{
	const s32 ix = x + w / 4;
	const s32 iy = y + h / 4;
	const s32 iw = w / 2;
	const s32 ih = h / 2;
	gui_line_styled(gui, ix, iy,      ix + iw, iy + ih, &style->line);
	gui_line_styled(gui, ix, iy + ih, ix + iw, iy,      &style->line);
}

void gui_pen_panel_drag(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        const gui_element_style_t *style)
{
	const s32 ix = x + w / 4;
	const s32 iy = y + h / 2;
	const s32 iw = w / 2;
	gui_pen_rect(gui, x, y, w, h, style);
	gui_line_styled(gui, ix, iy - 2, ix + iw, iy - 2, &style->line);
	gui_line_styled(gui, ix, iy,     ix + iw, iy,     &style->line);
	gui_line_styled(gui, ix, iy + 2, ix + iw, iy + 2, &style->line);
}

void gui_pen_panel_collapse(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            const gui_element_style_t *style)
{
	const v2f tri[3] = {
		{ x +     w / 3, y + 3 * h / 5 },
		{ x + 2 * w / 3, y + 3 * h / 5 },
		{ x +     w / 2, y + 2 * h / 5 },
	};
	gui_pen_rect(gui, x, y, w, h, style);
	gui_polyf(gui, tri, 3, style->line.color, g_nocolor);
}

void gui_pen_panel_restore(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           const gui_element_style_t *style)
{
	const v2f tri[3] = {
		{ x + 3 * w / 5, y +     h / 3 },
		{ x + 3 * w / 5, y + 2 * h / 3 },
		{ x + 2 * w / 5, y +     h / 2 },
	};
	gui_pen_rect(gui, x, y, w, h, style);
	gui_polyf(gui, tri, 3, style->line.color, g_nocolor);
}

void gui_pen_panel_close(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         const gui_element_style_t *style)
{
	const s32 ix = x + w / 4;
	const s32 iy = y + h / 4;
	const s32 iw = w / 2;
	const s32 ih = h / 2;
	gui_pen_rect(gui, x, y, w, h, style);
	gui_line_styled(gui, ix, iy,      ix + iw, iy + ih, &style->line);
	gui_line_styled(gui, ix, iy + ih, ix + iw, iy,      &style->line);
}

/* Style */

gui_style_t *gui_style(gui_t *gui)
{
	return &gui->style;
}

const gui_style_t *gui_style_c(const gui_t *gui)
{
	return &gui->style;
}

void gui_style_set(gui_t *gui, const gui_style_t *style)
{
	gui->style = *style;
}

typedef struct gui__style_stack_item
{
	size_t offset;
	size_t size;
} gui__style_stack_item_t;

static
void *gui__style_stack_item_location(gui_t *gui, gui__style_stack_item_t item)
{
	return ((u8*)&gui->style) + item.offset;
}

static
b32 gui__style_stack_can_push_item(gui_t *gui, gui__style_stack_item_t item)
{
	return gui->style_stack_sz + sizeof(item) + item.size < GUI_STYLE_STACK_LIMIT;
}

static
void gui__style_stack_push_item(gui_t *gui, gui__style_stack_item_t item)
{
	const void *loc = gui__style_stack_item_location(gui, item);
	memcpy(&gui->style_stack[gui->style_stack_sz], loc, item.size);
	gui->style_stack_sz += (u32)item.size;
	memcpy(&gui->style_stack[gui->style_stack_sz], &item, sizeof(item));
	gui->style_stack_sz += sizeof(item);
}

void gui_style_push_(gui_t *gui, const void *value, size_t offset, size_t size)
{
	const gui__style_stack_item_t item = { .offset = offset, .size = size };
	if (gui__style_stack_can_push_item(gui, item)) {
		void *loc = gui__style_stack_item_location(gui, item);
		assert(loc != value);
		gui__style_stack_push_item(gui, item);
		memcpy(loc, value, item.size);
	} else {
		assert(false);
	}
}

void gui_style_push_current_(gui_t *gui, size_t offset, size_t size)
{
	const gui__style_stack_item_t item = { .offset = offset, .size = size };
	if (gui__style_stack_can_push_item(gui, item))
		gui__style_stack_push_item(gui, item);
	else
		assert(false);
}

void gui_style_pop(gui_t *gui)
{
	gui__style_stack_item_t item;
	void *loc;
	if (gui->style_stack_sz > sizeof(item)) {
		gui->style_stack_sz -= sizeof(item);
		memcpy(&item, &gui->style_stack[gui->style_stack_sz], sizeof(item));
		gui->style_stack_sz -= (u32)item.size;
		loc = gui__style_stack_item_location(gui, item);
		memcpy(loc, &gui->style_stack[gui->style_stack_sz], item.size);
	} else {
		assert(false);
		gui->style_stack_sz = 0;
	}
}

#define GUI_STYLE_STACK_PUSH_LITERAL(name, type) \
void gui_style_push_##name##_(gui_t *gui, size_t offset, type val) \
{ \
	gui_style_push_(gui, &val, offset, sizeof(val)); \
}

GUI_STYLE_STACK_PUSH_LITERAL(color, color_t)
GUI_STYLE_STACK_PUSH_LITERAL(b32, b32)
GUI_STYLE_STACK_PUSH_LITERAL(s32, s32)
GUI_STYLE_STACK_PUSH_LITERAL(r32, r32)
GUI_STYLE_STACK_PUSH_LITERAL(ptr, const void*)

void gui_style_push_pen_(gui_t *gui, size_t offset, gui_pen_t pen)
{
	const size_t size =   offsetof(gui_style_t, btn.inactive)
	                    - offsetof(gui_style_t, btn.pen);
	void(*fn)(void) = (void(*)(void))pen;
	assert(pen);
	gui_style_push_(gui, &fn, offset, size);
}

#undef GUI_IMPLEMENTATION
#endif // GUI_IMPLEMENTATION
