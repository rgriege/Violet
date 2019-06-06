#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core.h"
#include "violet/color.h"
#include "violet/key.h"

/* Mesh */

typedef struct mesh_t
{
	u32 vbo;
	u32 sz;
} mesh_t;

void mesh_init(mesh_t *m, const v2f *poly, u32 n);
void mesh_destroy(mesh_t *m);
void mesh_poly(const mesh_t *m, v2f *poly);
void mesh_bind(const mesh_t *m);
void mesh_unbind();
void mesh_set_vertices(mesh_t *m, const v2f *v, u32 n);


/* Texture */

typedef enum texture_blend
{
	TEXTURE_BLEND_NRM,
	TEXTURE_BLEND_ADD,
	TEXTURE_BLEND_MUL,
	TEXTURE_BLEND_MAX = TEXTURE_BLEND_MUL,
} texture_blend_e;

typedef struct texture_t
{
	u32 handle;
	u32 width;
	u32 height;
	u32 blend;
} texture_t;

b32  texture_load(texture_t *tex, const char *filename);
void texture_init(texture_t *tex, u32 w, u32 h, u32 fmt, const void *data);
m3f  texture_get_transform(const texture_t *texture, s32 x, s32 y,
                           r32 sx, r32 sy, r32 rotation);
void texture_destroy(texture_t *tex);
void texture_coords_from_poly(mesh_t *tex_coords, const v2f *v, u32 n);
void texture_bind(const texture_t *tex);
void texture_unbind();


/* Shader */

typedef enum shader_type_t
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
} shader_type_t;

typedef struct shader_t
{
	const char *filename;
	u32 handle;
} shader_t;


b32  shader_init_from_string(shader_t *shader, const char *str,
                             shader_type_t type, const char *id);
b32  shader_init_from_file(shader_t *shader, const char *fname,
                           shader_type_t type);
void shader_destroy(shader_t *shader);

typedef struct shader_prog_t
{
	u32 handle;
	shader_t vertex_shader;
	shader_t frag_shader;
} shader_prog_t;

b32  shader_program_load_from_files(shader_prog_t *prog,
                                    const char *vert_fname,
                                    const char *frag_fname);
b32  shader_program_load_from_strings(shader_prog_t *prog,
                                      const char *vert_str,
                                      const char *frag_str);
b32  shader_program_create(shader_prog_t *prog, shader_t vertex_shader,
                           shader_t frag_shader);
void shader_program_destroy(shader_prog_t *p);
void shader_program_bind(const shader_prog_t *p);
void shader_program_unbind();
s32  shader_program_attrib(const shader_prog_t *p, const char *name);
s32  shader_program_uniform(const shader_prog_t *p, const char *name);


/* Image */

typedef struct img_t
{
	texture_t texture;
} img_t;

b32  img_load(img_t *img, const char *filename);
void img_init(img_t *img, u32 w, u32 h, u32 fmt, void *data);
void img_destroy(img_t *img);


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
} gui_align_t;

#define GUI_ALIGN_VERTICAL   (GUI_ALIGN_TOP | GUI_ALIGN_MIDDLE | GUI_ALIGN_BOTTOM)
#define GUI_ALIGN_HORIZONTAL (GUI_ALIGN_LEFT | GUI_ALIGN_CENTER | GUI_ALIGN_RIGHT)

void gui_align_anchor(s32 x, s32 y, s32 w, s32 h, gui_align_t align, s32 *px, s32 *py);

typedef struct font_t
{
	const char *filename;
	u32 sz;
	s32 num_glyphs;
	s32 ascent, descent, line_gap, newline_dist;
	void *char_info;
	texture_t texture;
} font_t;

b32  font_load(font_t *f, const char *filename, u32 sz);
void font_destroy(font_t *f);


/* General Gui */

typedef struct gui gui_t;

typedef enum gui_flags_t
{
	WINDOW_BORDERLESS = 1 << 0,
	WINDOW_RESIZABLE  = 1 << 1,
	WINDOW_MAXIMIZED  = 1 << 2,
	WINDOW_FULLSCREEN = 1 << 3,
	WINDOW_CENTERED   = 1 << 4,
} gui_flags_t;

gui_t *gui_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                  gui_flags_t flags);
gui_t *gui_create_ex(s32 x, s32 y, s32 w, s32 h, const char *title,
                     gui_flags_t flags, const char *font_file_path);
void   gui_destroy(gui_t *gui);
void   gui_move(const gui_t *gui, s32 dx, s32 dy);
void   gui_dim(const gui_t *gui, s32 *x, s32 *y);
b32    gui_is_maximized(const gui_t *gui);
void   gui_minimize(gui_t *gui);
void   gui_maximize(gui_t *gui);
void   gui_restore(gui_t *gui);
void   gui_fullscreen(gui_t *gui);
b32    gui_begin_frame(gui_t *gui);
void   gui_end_frame(gui_t *gui);
void   gui_end_frame_ex(gui_t *gui, u32 target_frame_milli,
                        u32 idle_frame_milli, u32 idle_start_milli);
void   gui_run(gui_t *gui, u32 fps, b32(*ufunc)(gui_t*, void*), void *udata);

timepoint_t gui_frame_start(const gui_t *gui);
u32         gui_frame_time_milli(const gui_t *gui);
timepoint_t gui_last_input_time(const gui_t *gui);


typedef enum mouse_button_t
{
	MB_LEFT      = 1 << 0,
	MB_MIDDLE    = 1 << 1,
	MB_RIGHT     = 1 << 2,
	MB_X1        = 1 << 3,
	MB_X2        = 1 << 4,
	MB_WHEELUP   = 1 << 5,
	MB_WHEELDOWN = 1 << 6,
	MB_WHEEL     = MB_WHEELUP | MB_WHEELDOWN,
} mouse_button_t;

/* Input */

void mouse_pos(const gui_t *gui, s32 *x, s32 *y);
void mouse_pos_last(const gui_t *gui, s32 *x, s32 *y);
void mouse_pos_press(const gui_t *gui, s32 *x, s32 *y);
void mouse_pos_global(const gui_t *gui, s32 *x, s32 *y);
b32  mouse_pos_changed(const gui_t *gui);
b32  mouse_pressed(const gui_t *gui, u32 mask);
b32  mouse_pressed_bg(const gui_t *gui, u32 mask);
b32  mouse_down(const gui_t *gui, u32 mask);
b32  mouse_down_bg(const gui_t *gui, u32 mask);
b32  mouse_released(const gui_t *gui, u32 mask);
b32  mouse_released_bg(const gui_t *gui, u32 mask);
b32  mouse_over_bg(const gui_t *gui);
void mouse_scroll(const gui_t *gui, s32 *dir);
void mouse_scroll_bg(const gui_t *gui, s32 *dir);
void mouse_press_debug(gui_t *gui, b32 enabled);

b32 key_down(const gui_t *gui, gui_key_t key);
b32 key_pressed(const gui_t *gui, gui_key_t key);
b32 key_released(const gui_t *gui, gui_key_t key);
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
                   u32 tex, texture_blend_e blend);
void gui_vertf(gui_t *gui, r32 x, r32 y, color_t c, r32 u, r32 v);
void gui_end(gui_t *gui);

/* Primitives */

typedef enum img_scale
{
	IMG_SCALED,
	IMG_CENTERED,
} img_scale_t;

#ifndef GUI_FONT_FILE_PATH
#define GUI_FONT_FILE_PATH "Roboto.ttf"
#endif

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c);
void gui_linef(gui_t *gui, r32 x0, r32 y0, r32 x1, r32 y1, r32 w, color_t c);
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
void gui_img(gui_t *gui, s32 x, s32 y, const char *img);
void gui_img_ex(gui_t *gui, s32 x, s32 y, const img_t *img, r32 sx, r32 sy,
                r32 rotation, r32 opacity);
void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                   img_scale_t scale);
img_t *gui_init_cached_img(gui_t *gui, const char *name);
void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt, color_t c,
             gui_align_t align);
void gui_txt_dim(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt,
                 gui_align_t align, s32 *px, s32 *py, s32 *pw, s32 *ph);
s32  gui_txt_width(gui_t *gui, const char *txt, u32 sz);

void gui_mask_push(gui_t *gui, s32 x, s32 y, s32 w, s32 h);
void gui_mask_pop(gui_t *gui);


typedef struct gui_line_style gui_line_style_t;
typedef struct gui_text_style gui_text_style_t;
typedef struct gui_element_style gui_element_style_t;

void gui_line_styled(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                     const gui_line_style_t *style);
void gui_txt_styled(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, const gui_text_style_t *style);

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

v2f *gui_verts_mut(gui_t *gui);
u32  gui_verts_cnt(const gui_t *gui);



/* Widgets */

/* Limit for input boxes & dropdowns */
#ifndef GUI_TXT_MAX_LENGTH
#define GUI_TXT_MAX_LENGTH 128
#endif

#ifndef GUI_FOCUS_STACK_SIZE
#define GUI_FOCUS_STACK_SIZE 2
#endif

#ifndef GUI_HINT_TIMER
#define GUI_HINT_TIMER 1000
#endif

typedef enum npt_flags_t
{
	NPT_PASSWORD              = 1 << 0,
	NPT_CLEAR_ON_FOCUS        = 1 << 1,
	NPT_COMPLETE_ON_ENTER     = 1 << 2, /* always enabled */
	NPT_COMPLETE_ON_TAB       = 1 << 3,
	NPT_COMPLETE_ON_CLICK_OUT = 1 << 4,
	NPT_COMPLETE_ON_ESCAPE    = 1 << 5,
	NPT_COMPLETE_ON_UNCHANGED = 1 << 6,
	NPT_COMPLETE_ON_DEFOCUS   = NPT_COMPLETE_ON_TAB
	                          | NPT_COMPLETE_ON_CLICK_OUT
	                          | NPT_COMPLETE_ON_ESCAPE
	                          | NPT_COMPLETE_ON_UNCHANGED,
} npt_flags_t;

typedef struct npt_filter
{
	b8 ascii[127];
	b8 non_ascii;
} npt_filter_t;
typedef const npt_filter_t* npt_filter_p;

typedef enum btn_val_t
{
	BTN_NONE,
	BTN_PRESS,
	BTN_HOLD,
} btn_val_t;

const npt_filter_t g_gui_npt_filter_print;
const npt_filter_t g_gui_npt_filter_numeric;
const npt_filter_t g_gui_npt_filter_hex;

b32 gui_npt_filter(npt_filter_p filter, s32 codepoint);
const char *gui_npt_val_buf(const gui_t *gui);

typedef struct gui_widget_bounds
{
	box2i bbox;
	struct gui_widget_bounds *prev;
} gui_widget_bounds_t;

typedef struct gui_scroll_area
{
	v2i pos;
	v2i dim;
	v2i scroll;
	gui_widget_bounds_t widget_bounds;
	v2i last_max_dim;
	struct gui_scroll_area *prev;
} gui_scroll_area_t;

/* returns NPT_COMPLETE_ON_XXX if completed using an enabled completion method
 * or 0 otherwise */
s32  gui_npt_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                 const char *hint, npt_flags_t flags);
s32  gui_npt_txt_ex(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                    const char *hint, npt_flags_t flags, npt_filter_p filter);
s32  gui_npt_val(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, u32 n,
                 npt_flags_t flags, npt_filter_p filter);
s32  gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt);
s32  gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                 img_scale_t scale);
s32  gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen);
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
void gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                 const char *txt, u32 *val, u32 opt);
b32  gui_dropdown_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        u32 *val, u32 num_items);
b32  gui_dropdown_item(gui_t *gui, const char *txt);
void gui_dropdown_end(gui_t *gui);
typedef void(*gui_drag_callback_t)(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                                   s32 offset_x, s32 offset_y, void *udata);
void gui_drag_callback_default(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                               s32 offset_x, s32 offset_y, void *udata);
b32  gui_drag(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, mouse_button_t mb);
b32  gui_drag_horiz(gui_t *gui, s32 *x, s32 y, s32 w, s32 h, mouse_button_t mb);
b32  gui_drag_vert(gui_t *gui, s32 x, s32 *y, s32 w, s32 h, mouse_button_t mb);
b32  gui_dragx(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, mouse_button_t mb,
               gui_drag_callback_t cb, void *udata);
b32  gui_cdrag(gui_t *gui, s32 *x, s32 *y, s32 r, mouse_button_t mb);
b32  gui_cdragx(gui_t *gui, s32 *x, s32 *y, s32 r, mouse_button_t mb,
                gui_drag_callback_t cb, void *udata);
b32  gui_menu_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, s32 item_w, u32 num_items);
void gui_menu_end(gui_t *gui);
b32  gui_color_picker_sv(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c);
b32  gui_color_picker_h(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c);
b32  gui_color_picker(gui_t *gui, s32 s, s32 y, s32 w, s32 h,
                      s32 pw, s32 ph, colorf_t *c);
b32  gui_color_picker8(gui_t *gui, s32 s, s32 y, s32 w, s32 h,
                       s32 pw, s32 ph, color_t *c);
void gui_scroll_area_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                           gui_scroll_area_t *scroll_area);
void gui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area);

u64  gui_widget_id(const gui_t *gui, s32 x, s32 y);
void gui_widget_focus_next(gui_t *gui);
b32  gui_widget_hot(const gui_t *gui, u64 id);
b32  gui_widget_active(const gui_t *gui, u64 id);
b32  gui_widget_focused(const gui_t *gui, u64 id);
b32  gui_any_widget_hot(const gui_t *gui);
b32  gui_any_widget_active(const gui_t *gui);
b32  gui_any_widget_has_focus(const gui_t *gui);
b32  gui_locked(const gui_t *gui);
void gui_lock(gui_t *gui);
void gui_unlock(gui_t *gui);
/* NOTE: I usually hate 'conditional' methods, but this cleans up usage code */
void gui_lock_if(gui_t *gui, b32 cond, u32 *restore_val);
void gui_lock_restore(gui_t *gui, u32 val);


/* Window */

void gui_window_drag(gui_t *gui, s32 x, s32 y, s32 w, s32 h);


/* Grid layout */

#define GUI_GRID_FLEX 0

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
	v2i start, dim, pos;
	gui_widget_bounds_t widget_bounds;
	struct gui_grid *prev;
} gui_grid_t;

void pgui_grid_begin(gui_t *gui, gui_grid_t *grid, s32 x, s32 y, s32 w, s32 h);
void pgui_grid_end(gui_t *gui, gui_grid_t *grid);
void pgui_row(gui_t *gui, s32 height, u32 num_cells);
void pgui_row_cells(gui_t *gui, s32 height, const r32 *cells, u32 num_cells);
#define pgui_row_cellsv(gui, height, cells) \
	pgui_row_cells(gui, height, cells, countof(cells))
void pgui_row_empty(gui_t *gui, s32 height);
void pgui_row_centered(gui_t *gui, s32 height, r32 width);

void pgui_col(gui_t *gui, s32 width, u32 num_cells);
void pgui_col_cells(gui_t *gui, s32 width, const r32 *cells, u32 num_cells);
#define pgui_col_cellsv(gui, width, cells) \
	pgui_col_cells(gui, width, cells, countof(cells))
void pgui_col_empty(gui_t *gui, s32 width);
void pgui_col_centered(gui_t *gui, s32 width, r32 height);

void pgui_cell(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h);
u64  pgui_next_widget_id(const gui_t *gui);

void pgui_spacer(gui_t *gui);
void pgui_spacer_blank(gui_t *gui);
void pgui_txt(gui_t *gui, const char *str);
void pgui_img(gui_t *gui, const char *fname, img_scale_t scale);
s32  pgui_btn_txt(gui_t *gui, const char *lbl);
s32  pgui_btn_img(gui_t *gui, const char *fname, img_scale_t scale);
s32  pgui_btn_pen(gui_t *gui, gui_pen_t pen);
b32  pgui_chk(gui_t *gui, const char *lbl, b32 *val);
s32  pgui_npt_txt(gui_t *gui, char *lbl, u32 n, const char *hint,
                  npt_flags_t flags);
s32  pgui_npt_txt_ex(gui_t *gui, char *lbl, u32 n, const char *hint,
                     npt_flags_t flags, npt_filter_p filter);
s32  pgui_npt_val(gui_t *gui, const char *txt, u32 n,
                  npt_flags_t flags, npt_filter_p filter);
b32  pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt);
void pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt);
b32  pgui_dropdown_begin(gui_t *gui, u32 *val, u32 num_items);
b32  pgui_dropdown_item(gui_t *gui, const char *txt);
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


/* Splits */

#ifndef GUI_SPLIT_RESIZE_BORDER
#define GUI_SPLIT_RESIZE_BORDER 4
#endif

typedef enum gui_split_flags
{
	GUI_SPLIT_RESIZABLE = 0x1, /* applies to branches */
	GUI_SPLIT_TEMPORARY = 0x2, /* applies to branches */
	GUI_SPLIT_DIVISIBLE = 0x4, /* applies to leaves */
	GUI_SPLIT_FULL      = 0x7,
} gui_split_flags_t;

typedef struct gui_split
{
	b16 in_use;
	b16 vertical;
	s32 flags;
	struct gui_split *parent, *sp1, *sp2;
	r32 sz, default_sz;
	box2i box;
	struct gui_panel *panel;
} gui_split_t;

void gui_set_splits(gui_t *gui, gui_split_t splits[], u32 num_splits);
b32  gui_create_root_split(gui_t *gui, gui_split_t **root);
b32  gui_split2h(gui_t *gui, gui_split_t *split, gui_split_t **sp1, r32 sz,
                 gui_split_t **sp2, gui_split_flags_t flags);
b32  gui_split2v(gui_t *gui, gui_split_t *split, gui_split_t **sp1, r32 sz,
                 gui_split_t **sp2, gui_split_flags_t flags);
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
	GUI_PANEL_SCROLLBARS  = 0x20,
	GUI_PANEL_DOCKABLE    = 0x40,
	GUI_PANEL_FULL        = 0x4f,
} gui_panel_flags_t;

typedef struct gui_panel
{
	s32 x, y, width, height;
	const char *title;
	gui_panel_flags_t flags;
	u32 id;
	intptr userdata;
	gui_scroll_area_t scroll_area;
	s32 pri;
	struct gui_panel *prev, *next;
	gui_split_t *split;
	b32 closed;
	b32 collapsed;
	b32 tabbed_out;
} gui_panel_t;

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, u32 id,
                     s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_t flags);
void pgui_panel_init_centered(gui_t *gui, gui_panel_t *panel, u32 id,
                              s32 w, s32 h,
                              const char *title, gui_panel_flags_t flags);
void pgui_panel_init_in_split(gui_t *gui, gui_panel_t *panel, u32 id,
                              gui_split_t *split,
                              const char *title, gui_panel_flags_t flags);
void pgui_panel_add_tab(gui_panel_t *panel, gui_panel_t *tab);
void pgui_panel_select_tab(gui_panel_t *panel);
b32  pgui_panel(gui_t *gui, gui_panel_t *panel);
void pgui_panel_collapse(gui_panel_t *panel);
void pgui_panel_restore(gui_panel_t *panel);
void pgui_panel_open(gui_t *gui, gui_panel_t *panel);
void pgui_panel_close(gui_t *gui, gui_panel_t *panel);
void pgui_panel_finish(gui_t *gui, gui_panel_t *panel);
b32  pgui_panel_content_visible(const gui_t *gui, const gui_panel_t *panel);
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
	u32 size;
	color_t color;
	gui_align_t align;
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

typedef struct gui_scroll_area_style
{
	color_t bg_color;
	s32 padding;
	s32 scrollbar_track_width;
	gui_slider_style_t scrollbar;
} gui_scroll_area_style_t;

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
	s32 padding;
} gui_panel_style_t;

typedef struct gui_style
{
	color_t                 bg_color;
	gui_line_style_t        line;
	gui_text_style_t        txt;
	gui_widget_style_t      npt;
	gui_widget_style_t      btn;
	gui_widget_style_t      chk;
	gui_slider_style_t      slider;
	gui_widget_style_t      select;
	gui_widget_style_t      dropdown;
	gui_widget_style_t      drag;
	gui_element_style_t     hint;
	gui_scroll_area_style_t scroll_area;
	gui_panel_style_t       panel;
	gui_line_style_t        split;
} gui_style_t;

const gui_style_t g_gui_style_default;
const gui_style_t g_gui_style_invis;

gui_style_t *gui_style(gui_t *gui);
void         gui_style_set(gui_t *gui, const gui_style_t *style);

void gui_style_push_(gui_t *gui, const void *value, size_t offset, size_t size);
void gui_style_push_current_(gui_t *gui, size_t offset, size_t size);
void gui_style_push_color_(gui_t *gui, size_t offset, color_t val);
void gui_style_push_b32_(gui_t *gui, size_t offset, u32 val);
void gui_style_push_u32_(gui_t *gui, size_t offset, u32 val);
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
#define gui_style_push_u32(gui, loc, val) \
	gui_style_push_u32_(gui, offsetof(gui_style_t, loc), val)
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
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_STATIC
#include "stbi.h"
#define STB_RECT_PACK_IMPLEMENTATION
// #define STBRP_STATIC
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
// #define STBTT_STATIC
#define stbtt_uint8 u8
#define stbtt_int8 s8
#define stbtt_uint16 u16
#define stbtt_int16 s16
#define stbtt_uint32 u32
#define stbtt_int32 s32
#define STBTT_malloc(x, u) ((allocator_t*)(u))->malloc_(x, u  MEMCALL_LOCATION)
#define STBTT_free(x, u)   ((allocator_t*)(u))->free_(x, u  MEMCALL_LOCATION)
#include "stb_truetype.h"

#include "violet/array.h"
#include "violet/fmath.h"
#include "violet/geom.h"
#include "violet/graphics.h"
#include "violet/imath.h"
#include "violet/string.h"
#include "violet/utf8.h"
#include "violet/os.h"

#if defined(_MSC_VER) && !defined(__clang__)
#ifdef _M_X64
#include <intrin.h>
#define __builtin_popcount __popcnt
#else
int __builtin_popcount(u32 x)
{
	x =  x               - ((x >> 1)  & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2)  & 0x33333333);
	x = (x               +  (x >> 4)) & 0x0f0f0f0f;
	x = (x * 0x01010101) >> 24;
	return (int)x;
}
#endif // _M_X64
#endif // _MSC_VER

static const char *g_vertex_shader;
static const char *g_fragment_shader;


/* Mesh */

void mesh_init(mesh_t *m, const v2f *poly, u32 n)
{
	GL_CHECK(glGenBuffers, 1, &m->vbo);
	mesh_set_vertices(m, poly, n);
}

void mesh_destroy(mesh_t *m)
{
	if (m->vbo != 0)
		GL_CHECK(glDeleteBuffers, 1, &m->vbo);
}

void mesh_poly(const mesh_t *m, v2f *poly)
{
	array_clear(poly);
	array_reserve(poly, m->sz);
	array_sz(poly) = m->sz;
	mesh_bind(m);
	GL_CHECK(glGetBufferSubData, GL_ARRAY_BUFFER, 0, m->sz * 2 * sizeof(GL_FLOAT), poly);
}

void mesh_bind(const mesh_t *m)
{
	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, m->vbo);
}

void mesh_unbind()
{
	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

void mesh_set_vertices(mesh_t *m, const v2f *v, u32 n)
{
	mesh_bind(m);
	m->sz = n;
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, m->sz * 2 * sizeof(GL_FLOAT), v, GL_STATIC_DRAW);
}

/* Texture */

b32 texture_load(texture_t *tex, const char *filename)
{
	b32 ret = false;
	int w, h;
	u8 *image = stbi_load(filename, &w, &h, NULL, 4);
	if (image) {
		texture_init(tex, w, h, GL_RGBA, image);
		stbi_image_free(image);
		ret = true;
	}
	return ret;
}

void texture_init(texture_t *tex, u32 w, u32 h, u32 fmt, const void *data)
{
	tex->width = w;
	tex->height = h;
	tex->blend = TEXTURE_BLEND_NRM;

	GL_CHECK(glGenTextures, 1, &tex->handle);
	texture_bind(tex);

	GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt,
			GL_UNSIGNED_BYTE, data);

	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void texture_destroy(texture_t *tex)
{
	if (tex->handle != 0) {
		GL_CHECK(glDeleteTextures, 1, &tex->handle);
		tex->handle = 0;
	}
}

void texture_coords_from_poly(mesh_t *tex_coords, const v2f *v, u32 n)
{
	box2f extent;
	v2f dimension;
	v2f *coords;

	polyf_bounding_box(v, n, &extent);
	dimension = v2f_sub(extent.max, extent.min);

	array_init_ex(coords, n, g_temp_allocator);
	for (const v2f *vi = v, *vn = v + n; vi != vn; ++vi)
		array_append(coords, v2f_div(v2f_sub(*vi, extent.min), dimension));
	mesh_init(tex_coords, coords, n);
	array_destroy(coords);
}

void texture_bind(const texture_t *tex)
{
	GL_CHECK(glBindTexture, GL_TEXTURE_2D, tex->handle);
}

void texture_unbind()
{
	GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
}


/* Shader */

b32 shader_init_from_string(shader_t *shader, const char *str,
                            shader_type_t type, const char *id)
{
	b32 retval = false;
	char *log_buf;
	GLint compiled, log_len;

	shader->handle = glCreateShader(  type == VERTEX_SHADER
	                                ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	GL_ERR_CHECK("glCreateShader");
	GL_CHECK(glShaderSource, shader->handle, 1, (const GLchar **)&str, 0);

	GL_CHECK(glCompileShader, shader->handle);
	GL_CHECK(glGetShaderiv, shader->handle, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GL_CHECK(glGetShaderiv, shader->handle, GL_INFO_LOG_LENGTH, &log_len);
		log_buf = malloc(log_len);
		GL_CHECK(glGetShaderInfoLog, shader->handle, log_len, NULL, log_buf);
		log_error("Compilation error in shader '%s': %s", id, log_buf);
		free(log_buf);
		shader->handle = 0;
		goto err;
	}

	retval = true;
err:
	return retval;
}

b32 shader_init_from_file(shader_t *shader, const char *fname,
                          shader_type_t type)
{
	b32 retval = false;
	FILE *file;
	char *file_buf;
	size_t fsize;

	file = fopen(fname, "r");
	if (!file) {
		log_error("Could not open shader file '%s'", fname);
		return retval;
	}

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	rewind(file);
	file_buf = malloc(fsize + 1);
	if (fread(file_buf, 1, fsize, file) != fsize) {
		log_error("Failed to read shader file '%s'", fname);
		goto err;
	}
	file_buf[fsize] = 0;

	retval = shader_init_from_string(shader, file_buf, type, fname);

err:
	free(file_buf);
	fclose(file);
	return retval;
}

void shader_destroy(shader_t *shader)
{
	GL_CHECK(glDeleteShader, shader->handle);
	shader->handle = 0;
}

b32 shader_program_load_from_strings(shader_prog_t *prog,
                                     const char *vert_str,
                                     const char *frag_str)
{
	b32 retval = false;

	if (!shader_init_from_string(&prog->vertex_shader, vert_str,
	                             VERTEX_SHADER, "vertex ubershader"))
		goto out;

	if (!shader_init_from_string(&prog->frag_shader, frag_str,
	                             FRAGMENT_SHADER, "fragment ubershader"))
		goto err_frag;
	
	if (shader_program_create(prog, prog->vertex_shader, prog->frag_shader)) {
		retval = true;
		goto out;
	}

	shader_destroy(&prog->frag_shader);
err_frag:
	shader_destroy(&prog->vertex_shader);
out:
	return retval;
}

b32 shader_program_load_from_files(shader_prog_t *prog,
                                   const char *vert_fname,
                                   const char *frag_fname)
{
	b32 retval = false;

	if (!shader_init_from_file(&prog->vertex_shader, vert_fname, VERTEX_SHADER))
		goto out;

	if (!shader_init_from_file(&prog->frag_shader, frag_fname, FRAGMENT_SHADER))
		goto err_frag;
	
	if (shader_program_create(prog, prog->vertex_shader, prog->frag_shader)) {
		retval = true;
		goto out;
	}

	shader_destroy(&prog->frag_shader);
err_frag:
	shader_destroy(&prog->vertex_shader);
out:
	return retval;
}

b32 shader_program_create(shader_prog_t *p, shader_t vertex_shader,
                          shader_t frag_shader)
{
	GLint status, length;
	char *log_buf;

	p->handle = glCreateProgram();
	GL_ERR_CHECK("glCreateProgram");

	GL_CHECK(glAttachShader, p->handle, vertex_shader.handle);
	p->vertex_shader = vertex_shader;
	GL_CHECK(glAttachShader, p->handle, frag_shader.handle);
	p->frag_shader = frag_shader;

	GL_CHECK(glLinkProgram, p->handle);
	GL_CHECK(glGetProgramiv, p->handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GL_CHECK(glGetProgramiv, p->handle, GL_INFO_LOG_LENGTH, &length);
		log_buf = malloc(length);
		GL_CHECK(glGetProgramInfoLog, p->handle, length, NULL, log_buf);
		log_error("Shader link error using '%s' & '%s': %s",
		          vertex_shader.filename, frag_shader.filename, log_buf);
		free(log_buf);
		return false;
	}

#ifdef GUI_VALIDATE_SHADER
	GL_CHECK(glValidateProgram, p->handle);
	GL_CHECK(glGetProgramiv, p->handle, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		GL_CHECK(glGetProgramiv, p->handle, GL_INFO_LOG_LENGTH, &length);
		log_buf = malloc(length);
		GL_CHECK(glGetProgramInfoLog, p->handle, length, NULL, log_buf);
		log_error("Shader validation error using '%s' & '%s': %s",
		          vertex_shader.filename, frag_shader.filename, log_buf);
		free(log_buf);
		return false;
	}
#endif

	return true;
}

void shader_program_bind(const shader_prog_t *p)
{
	GL_CHECK(glUseProgram, p->handle);
}

void shader_program_unbind()
{
	GL_CHECK(glUseProgram, 0);
}

void shader_program_destroy(shader_prog_t *p)
{
	GL_CHECK(glDetachShader, p->handle, p->vertex_shader.handle);
	shader_destroy(&p->vertex_shader);
	GL_CHECK(glDetachShader, p->handle, p->frag_shader.handle);
	shader_destroy(&p->frag_shader);
	GL_CHECK(glDeleteProgram, p->handle);
	p->handle = 0;
}

s32 shader_program_attrib(const shader_prog_t *p, const char *name)
{
	s32 attrib = glGetAttribLocation(p->handle, name);
	GL_ERR_CHECK("glGetAttribLocation");
	return attrib;
}

s32 shader_program_uniform(const shader_prog_t *p, const char *name)
{
	s32 uniform = glGetAttribLocation(p->handle, name);
	GL_ERR_CHECK("glGetUniformLocation");
	return uniform;
}


/* Image */

b32 img_load(img_t *img, const char *filename)
{
	if (!texture_load(&img->texture, filename)) {
		log_error("img_load(%s) error", filename);
		return false;
	}
	return true;
}

void img_init(img_t *img, u32 w, u32 h, u32 fmt, void *data)
{
	texture_init(&img->texture, w, h, fmt, data);
}

void img_destroy(img_t *img)
{
	texture_destroy(&img->texture);
}



/* Font */

void gui_align_anchor(s32 x, s32 y, s32 w, s32 h, gui_align_t align, s32 *px, s32 *py)
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
int rgtt_PackFontRanges(stbtt_pack_context *spc, stbtt_fontinfo *info,
                        stbtt_pack_range *ranges, int num_ranges)
{
	int i, j, n, return_value = 1;
	stbrp_rect *rects;

	// flag all characters as NOT packed
	for (i=0; i < num_ranges; ++i)
		for (j=0; j < ranges[i].num_chars; ++j)
			ranges[i].chardata_for_range[j].x0 =
				ranges[i].chardata_for_range[j].y0 =
				ranges[i].chardata_for_range[j].x1 =
				ranges[i].chardata_for_range[j].y1 = 0;

	n = 0;
	for (i = 0; i < num_ranges; ++i)
		n += ranges[i].num_chars;

	rects = STBTT_malloc(sizeof(*rects) * n, spc->user_allocator_context);
	if (!rects)
		return 0;

	n = stbtt_PackFontRangesGatherRects(spc, info, ranges, num_ranges, rects);

	stbtt_PackFontRangesPackRects(spc, rects, n);

	return_value = stbtt_PackFontRangesRenderIntoRects(spc, info, ranges,
	                                                   num_ranges, rects);

	STBTT_free(rects, spc->user_allocator_context);
	return return_value;
}

static
int rgtt_PackFontRange(stbtt_pack_context *spc, stbtt_fontinfo *info,
                       float font_size, int first_unicode_codepoint_in_range,
                       int num_chars_in_range,
                       stbtt_packedchar *chardata_for_range)
{
	stbtt_pack_range range;
	range.first_unicode_codepoint_in_range = first_unicode_codepoint_in_range;
	range.array_of_unicode_codepoints = NULL;
	range.num_chars                   = num_chars_in_range;
	range.chardata_for_range          = chardata_for_range;
	range.font_size                   = font_size;
	return rgtt_PackFontRanges(spc, info, &range, 1);
}

static
int rgtt_Pack(stbtt_fontinfo *info, int font_size, void *char_info, texture_t *tex)
{
	temp_memory_mark_t mark = temp_memory_save(g_temp_allocator);
	unsigned char *bitmap = NULL;
	s32 w = 512, h = 512;
	stbtt_pack_context context;
	b32 packed = false, failed = false;

	/* TODO(rgriege): oversample with smaller fonts */
	// stbtt_PackSetOversampling(&context, 2, 2);

	while (!packed && !failed) {
		temp_memory_restore(mark);
		bitmap = amalloc(w*h, g_temp_allocator);
		/* NOTE(rgriege): otherwise bitmap has noise at the bottom */
		memset(bitmap, 0, w * h);

		if (!stbtt_PackBegin(&context, bitmap, w, h, w, 1, g_temp_allocator)) {
			failed = true;
		} else if (rgtt_PackFontRange(&context, info, font_size, 0,
		                              info->numGlyphs, char_info)) {
			stbtt_PackEnd(&context); // not really necessary, handled by memory mark
			packed = true;
		} else if (w < 1024) {
			w *= 2;
		} else if (h < 1024) {
			h *= 2;
		} else {
			failed = true; // fail on fonts needing too large a texture
		}
	}

	if (packed) {
		texture_init(tex, w, h, GL_RED, bitmap);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
		GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
	}

	temp_memory_restore(mark);
	return packed;
}

b32 font_load(font_t *f, const char *filename, u32 sz)
{
	b32 retval = false;
	stbtt_fontinfo info = { .userdata = g_temp_allocator };
	int ascent, descent, line_gap;
	r32 scale;
	void *ttf;

	f->char_info = NULL;

	if (!(ttf = file_read_all(filename, "rb", g_temp_allocator)))
		goto out;

	if (!stbtt_InitFont(&info, ttf, stbtt_GetFontOffsetForIndex(ttf, 0)))
		goto err_ttf;

	f->num_glyphs = info.numGlyphs;
	log_debug("packing %d glyphs for %s:%u", f->num_glyphs, filename, sz);
	f->char_info = malloc(f->num_glyphs * sizeof(stbtt_packedchar));

	if (!rgtt_Pack(&info, sz, f->char_info, &f->texture))
		goto err_pack;

	f->filename = filename;
	f->sz = sz;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	scale = stbtt_ScaleForPixelHeight(&info, sz);
	f->ascent = scale * ascent;
	f->descent = scale * descent;
	f->line_gap = scale * line_gap;
	f->newline_dist = scale * (ascent - descent + line_gap);
	retval = true;

err_pack:
	if (!retval)
		free(f->char_info);
err_ttf:
	afree(ttf, g_temp_allocator);
out:
	return retval;
}

void font_destroy(font_t *f)
{
	free(f->char_info);
	texture_destroy(&f->texture);
}

static
s32 font__line_width(font_t *f, const char *txt)
{
	const char *p = txt;
	char *pnext;
	s32 cp;
	stbtt_aligned_quad q;
	r32 width = 0, y = 0;

	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		p = pnext;
		if (cp == '\n')
			goto out;
		else if (cp < f->num_glyphs)
			stbtt_GetPackedQuad(f->char_info, f->texture.width, f->texture.height,
			                    cp, &width, &y, &q, 1);
	}
out:
	return width;
}

static
s32 font__line_offset_x(font_t *f, const char *txt,
                        const gui_text_style_t *style)
{
	const s32 width = font__line_width(f, txt);
	if (style->align & GUI_ALIGN_CENTER)
		return -width / 2;
	else if (style->align & GUI_ALIGN_RIGHT)
		return -(width + style->padding);
	else /* default to GUI_ALIGN_LEFT */
		return style->padding;
}

static
s32 font__offset_y(font_t *f, const char *txt, const gui_text_style_t *style)
{
	s32 height;
	if (style->align & GUI_ALIGN_MIDDLE) {
		height = 1;
		for (const char *c = txt; *c != '\0'; ++c)
			if (*c == '\n')
				++height;
		return   height % 2 == 0
		       ? -f->descent + f->line_gap + f->newline_dist * (height / 2 - 1)
		       :   -f->descent - (f->ascent - f->descent) / 2
		         + f->newline_dist * (height / 2);
	} else if (style->align & GUI_ALIGN_TOP) {
		return -f->ascent - f->line_gap / 2 - style->padding;
	} else /* default to GUI_ALIGN_BOTTOM */ {
		height = -f->descent + f->line_gap / 2 + style->padding;
		for (const char *c = txt; *c != '\0'; ++c)
			if (*c == '\n')
				height += f->newline_dist;
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

#define gi__gui_scroll_area_style_default { \
	.bg_color = gi_nocolor, \
	.padding = 0, \
	.scrollbar_track_width = 5, \
	.scrollbar = gi__gui_slider_style_default, \
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
	.dropdown = gi__gui_btn_style_default,
	.drag = gi__gui_chk_style_default,
	.hint = gi__gui_hint_style_default,
	.scroll_area = gi__gui_scroll_area_style_default,
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
		.padding = 10,
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

const gui_style_t g_gui_style_invis = {
	.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
	.line     = gi__gui_line_style_invis,
	.txt      = gi__gui_text_style_invis,
	.npt      = gi__gui_widget_style_invis,
	.btn      = gi__gui_widget_style_invis,
	.chk      = gi__gui_widget_style_invis,
	.slider   = gi__gui_slider_style_invis,
	.select   = gi__gui_widget_style_invis,
	.dropdown = gi__gui_widget_style_invis,
	.drag     = gi__gui_widget_style_invis,
	.hint     = gi__gui_element_style_invis,
	.panel    = {
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
		.padding           = 10,
	},
	.split    = gi__gui_line_style_invis,
};


typedef enum gui_vbo_type
{
	VBO_VERT,
	VBO_COLOR,
	VBO_TEX,
	VBO_COUNT
} gui_vbo_type_t;


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

typedef struct draw_call
{
	GLint idx;
	GLsizei cnt;
	u32 type;
	GLuint tex;
	texture_blend_e blend;
} draw_call_t;

#define GUI__LAYER_PRIORITY_HINT  2
#define GUI__LAYER_PRIORITY_POPUP 1

typedef struct gui__layer
{
	u32 draw_call_idx, draw_call_cnt;
	s32 x, y, w, h;
	s32 pri;
} gui__layer_t;

typedef struct cached_img
{
	img_t img;
	u32 id;
} cached_img_t;

typedef enum gui_cursor
{
	GUI__CURSOR_DEFAULT,
	GUI__CURSOR_RESIZE_NS,
	GUI__CURSOR_RESIZE_EW,
	GUI__CURSOR_COUNT
} gui__cursor_t;

typedef enum gui__key_toggle_state
{
	GUI__KBT_OFF,
	GUI__KBT_PRESSED_ON,
	GUI__KBT_RELEASED_ON,
} gui__key_toggle_state_t;

typedef struct gui__repeat
{
	u32 val;
	u32 delay;
	u32 interval;
	u32 timer;
	b32 triggered;
} gui__repeat_t;

typedef struct gui
{
	timepoint_t creation_time;
	timepoint_t frame_start_time;
	timepoint_t last_input_time;
	u32 frame_time_milli;
	SDL_Window *window;
	SDL_GLContext gl_context;
	SDL_Window *parent_window;
	SDL_GLContext parent_gl_context;

	/* rendering */
	u32 vao, vbo[VBO_COUNT];
	shader_prog_t shader;
#ifdef __EMSCRIPTEN__
	s32 shader_attrib_loc[VBO_COUNT];
#endif
	texture_t texture_white;
	texture_t texture_white_dotted;
	v2f verts[GUI_MAX_VERTS];
	color_t vert_colors[GUI_MAX_VERTS];
	v2f vert_tex_coords[GUI_MAX_VERTS];
	u32 vert_cnt;
	draw_call_t draw_calls[GUI_MAX_DRAW_CALLS];
	u32 draw_call_cnt;
	u32 draw_call_vert_idx;
	gui__layer_t layers[GUI_MAX_LAYERS];
	gui__layer_t *layer;
	box2i masks[GUI_MASK_STACK_LIMIT];
	box2i *mask;

	v2i window_dim;
	v2i window_restore_pos;
	v2i window_restore_dim;

	/* mouse */
	v2i mouse_pos;
	v2i mouse_pos_last;
	v2i mouse_pos_press;
	u32 mouse_btn;
	u32 mouse_btn_diff;
	u64 mouse_covered_by_widget_id;
	b32 mouse_debug;
	gui__repeat_t mouse_repeat;

	/* keyboard */
	u8 prev_keys[KB_COUNT];
	const u8 *keys;
	gui__key_toggle_state_t key_toggles[KBT_COUNT];
	gui__repeat_t key_repeat;
	char text_npt[32];

	/* style */
	SDL_Cursor *cursors[GUI__CURSOR_COUNT];
	b32 use_default_cursor;
	char font_file_path[256];
	array(font_t) fonts;
	cached_img_t *imgs;
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
		u32 cursor_pos; /* byte, NOT GLYPH, offset */
		b32 performed_action;
		u32 initial_txt_hash;
		char val_buf[GUI_TXT_MAX_LENGTH];
		char pw_buf[GUI_TXT_MAX_LENGTH];
	} npt;
	v2i drag_offset;
	v2f *vert_buf;
	struct
	{
		u64 id;
		s32 x, y, w, h;
		u32 *val;
		u32 num_items;
		u32 item_idx;
		b32 triggered_by_key;
		s32 render_state;
		char selected_item_txt[GUI_TXT_MAX_LENGTH];
	} dropdown;
	struct
	{
		u64 id;
		struct {
			s32 x, y, w, h;
		} mask;
		b32 inside;
		b32 close_at_end;
	} popup; /* TODO(rgriege): support nested popups */
	b32 dragging_window;
	colorf_t color_picker8_color;
	gui_scroll_area_t *scroll_area;
	s32 hint_timer;

	/* grid */
	gui_grid_t grid_panel;
	gui_grid_t grid_popup;
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

static u32 g_gui_cnt = 0;

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

static b32 gui__get_display_usable_bounds(s32 display_idx, SDL_Rect *rect);
static void gui__store_window_rect(gui_t *gui);
static void gui__layer_init(gui_t *gui, gui__layer_t *layer, s32 x, s32 y, s32 w, s32 h);
static void gui__layer_new(gui_t *gui);

gui_t *gui_create_ex(s32 x, s32 y, s32 w, s32 h, const char *title,
                     gui_flags_t flags, const char *font_file_path)
{
	gui_t *gui = calloc(1, sizeof(gui_t));
	if (g_gui_cnt == 0) {
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			log_error("SDL_Init(VIDEO) failed: %s", SDL_GetError());
			goto err_sdl;
		}
		gui->parent_window = NULL;
	} else {
		gui->parent_window = SDL_GL_GetCurrentWindow();
		gui->parent_gl_context = SDL_GL_GetCurrentContext();
	}

#ifndef __EMSCRIPTEN__
	// Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_GetNumVideoDisplays() < 1) {
		log_error("could not create window: no video displays found");
		goto err_win;
	}

	SDL_Rect usable_bounds;
	if (!gui__get_display_usable_bounds(0, &usable_bounds))
		goto err_ctx;

	u32 sdl_flags = SDL_WINDOW_OPENGL;
	if (flags & WINDOW_BORDERLESS)
		sdl_flags |= SDL_WINDOW_BORDERLESS;
	if (flags & WINDOW_RESIZABLE)
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	if (flags & WINDOW_MAXIMIZED) {
		x = usable_bounds.x;
		y = usable_bounds.y;
		w = usable_bounds.w;
		h = usable_bounds.h;
	} else if (flags & WINDOW_CENTERED) {
		w = min(w, usable_bounds.w);
		h = min(h, usable_bounds.h);
		x = (usable_bounds.w - w) / 2 + usable_bounds.x;
		y = (usable_bounds.h - h) / 2 + usable_bounds.y;
	} else {
		x = max(x, usable_bounds.x);
		y = max(y, usable_bounds.y);
		w = min(w, usable_bounds.w - x);
		h = min(h, usable_bounds.h - y);
	}
	if (flags & WINDOW_FULLSCREEN)
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	gui->window = SDL_CreateWindow(title, x, y, w, h, sdl_flags);
	if (gui->window == NULL) {
		log_error("SDL_CreateWindow failed: %s", SDL_GetError());
		goto err_win;
	}

	gui->gl_context = SDL_GL_CreateContext(gui->window);
	if (gui->gl_context == NULL) {
		log_error("SDL_CreateContext failed: %s", SDL_GetError());
		goto err_ctx;
	}

	if (SDL_GL_SetSwapInterval(0) != 0)
		log_warn("SDL_GL_SetSwapInterval failed: %s", SDL_GetError());

	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		log_error("glewInit error: %s", glewGetErrorString(glew_err));
		goto err_glew;
	}
	GL_ERR_CHECK("glewInit");

	log_info("GL version: %s", glGetString(GL_VERSION));
	GL_ERR_CHECK("glGetString");

	GL_CHECK(glEnable, GL_MULTISAMPLE);
	GL_CHECK(glEnable, GL_BLEND);
	GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_CHECK(glEnable, GL_SCISSOR_TEST);

	GL_CHECK(glGenVertexArrays, 1, &gui->vao);
	GL_CHECK(glGenBuffers, VBO_COUNT, gui->vbo);

	static const color_t texture_white_data[1] = { gi_white };
	texture_init(&gui->texture_white, 1, 1, GL_RGBA, texture_white_data);

	static const u32 texture_white_dotted_data[] = { 0x00ffffff, 0xffffffff };
	texture_init(&gui->texture_white_dotted, 2, 1, GL_RGBA, texture_white_dotted_data);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (!shader_program_load_from_strings(&gui->shader, g_vertex_shader,
	                                      g_fragment_shader))
		goto err_white;

#ifdef __EMSCRIPTEN__
	gui->shader_attrib_loc[VBO_VERT]  = shader_program_attrib(&gui->shader, "position");
	gui->shader_attrib_loc[VBO_COLOR] = shader_program_attrib(&gui->shader, "color");
	gui->shader_attrib_loc[VBO_TEX]   = shader_program_attrib(&gui->shader, "tex_coord");
#endif

	gui->cursors[GUI__CURSOR_DEFAULT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	gui->cursors[GUI__CURSOR_RESIZE_NS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	gui->cursors[GUI__CURSOR_RESIZE_EW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	for (u32 i = 0; i < GUI__CURSOR_COUNT; ++i)
		if (!gui->cursors[i])
			goto err_cursor;
	strncpy(gui->font_file_path, font_file_path, sizeof(gui->font_file_path)-1);
	gui->fonts = array_create();
	gui->imgs = array_create();

	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt) == 1); /* must be run before SDL_GetWindowSize */
	}
	SDL_GetWindowSize(gui->window, &gui->window_dim.x, &gui->window_dim.y);
	gui__store_window_rect(gui);

	gui->creation_time = time_current();
	gui->frame_start_time = gui->creation_time;
	gui->last_input_time = gui->creation_time;
	gui->frame_time_milli = 0;

	memset(gui->prev_keys, 0, KB_COUNT);
	memset(gui->key_toggles, 0, KBT_COUNT * sizeof(gui__key_toggle_state_t));
	gui__repeat_init(&gui->key_repeat);

	gui_style_set(gui, &g_gui_style_default);

	gui->style_stack_sz = 0;

	gui->mouse_btn = SDL_GetMouseState(&gui->mouse_pos.x, &gui->mouse_pos.y);
	gui->mouse_pos_last = gui->mouse_pos;
	gui->mouse_pos_press = gui->mouse_pos;
	gui->mouse_debug = false;
	gui__repeat_init(&gui->mouse_repeat);

	gui->lock = 0;
	gui->hot_id = 0;
	gui->active_id = 0;
	arrclr(gui->focus_ids);
	gui->active_id_at_frame_start = 0;
	gui->focus_next_widget = false;
	gui->focus_prev_widget_id = 0;
	gui->prev_widget_id = 0;
	gui->hot_id_last_frame = 0;
	gui->widget_bounds = &gui->default_widget_bounds;
	memclr(gui->default_widget_bounds);


	gui->npt.cursor_pos = 0;
	gui->npt.performed_action = false;
	gui->npt.initial_txt_hash = 0;
	gui->npt.pw_buf[0] = 0;
	gui->npt.val_buf[0] = 0;
	gui->drag_offset = g_v2i_zero;
	gui->vert_buf = array_create();
	gui->dropdown.id = 0;
	gui->popup.id = 0;
	gui->dragging_window = false;
	gui->color_picker8_color = (colorf_t){0};
	gui->scroll_area = NULL;
	gui->hint_timer = GUI_HINT_TIMER;

	gui->grid = NULL;

	gui->splits = NULL;
	gui->num_splits = 0;
	gui->root_split = NULL;
	gui->splits_rendered_this_frame = false;

	gui->panel = NULL;
	gui->is_dragging_dockable_panel = false;
	gui->next_panel_pri = 0;
	gui->min_panel_pri = 0;

	++g_gui_cnt;

	goto out;

err_cursor:
	for (u32 i = 0; i < GUI__CURSOR_COUNT; ++i)
		if (gui->cursors[i])
			SDL_FreeCursor(gui->cursors[i]);
err_white:
	texture_destroy(&gui->texture_white);
	texture_destroy(&gui->texture_white_dotted);
	GL_CHECK(glDeleteBuffers, 3, gui->vbo);
	GL_CHECK(glDeleteVertexArrays, 1, &gui->vao);
err_glew:
	SDL_GL_DeleteContext(gui->gl_context);
	if (gui->parent_window)
		SDL_GL_MakeCurrent(gui->parent_window, gui->parent_gl_context);
err_ctx:
	SDL_DestroyWindow(gui->window);
err_win:
	SDL_Quit();
err_sdl:
	free(gui);
	gui = NULL;
out:
	return gui;
}

gui_t *gui_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                  gui_flags_t flags)
{
	return gui_create_ex(x, y, w, h, title, flags, GUI_FONT_FILE_PATH);
}

void gui_destroy(gui_t *gui)
{
	array_destroy(gui->vert_buf);
	for (u32 i = 0; i < GUI__CURSOR_COUNT; ++i)
		SDL_FreeCursor(gui->cursors[i]);
	array_foreach(gui->fonts, font_t, f)
		font_destroy(f);
	array_destroy(gui->fonts);
	array_foreach(gui->imgs, cached_img_t, ci)
		img_destroy(&ci->img);
	array_destroy(gui->imgs);
	shader_program_destroy(&gui->shader);
	texture_destroy(&gui->texture_white);
	texture_destroy(&gui->texture_white_dotted);
	GL_CHECK(glDeleteBuffers, 3, gui->vbo);
	GL_CHECK(glDeleteVertexArrays, 1, &gui->vao);
	SDL_GL_DeleteContext(gui->gl_context);
	SDL_DestroyWindow(gui->window);
	if (--g_gui_cnt == 0)
		SDL_Quit();
	else if (gui->parent_window)
		SDL_GL_MakeCurrent(gui->parent_window, gui->parent_gl_context);
	free(gui);
}

void gui_move(const gui_t *gui, s32 dx, s32 dy)
{
	s32 x, y;
	SDL_GetWindowPosition(gui->window, &x, &y);
	SDL_SetWindowPosition(gui->window, x + dx, y + dy);
}

void gui_dim(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->window_dim.x;
	*y = gui->window_dim.y;
}

static
b32 gui__get_display_usable_bounds(s32 display_idx, SDL_Rect *rect)
{
	if (SDL_GetDisplayUsableBounds(0, rect) != 0) {
		log_error("SDL_GetDisplayUsableBounds failed: %s", SDL_GetError());
		return false;
	}

#ifdef __APPLE__
	static SDL_Rect usable_bounds = {0};
	if (usable_bounds.w != 0) {
		/* SDL_GetDisplayUsableBounds returns SDL_GetDisplayBounds in later calls...
		 * I don't like caching this, because the usable region could change while
		 * the app is running, but it's the only option on Mac. */
		*rect = usable_bounds;
	} else {
		/* window origin is supposed to be in the top-left corner,
		 * but for some reason SDL_GetDisplayUsableBounds
		 * returns it in the bottom-left corner on Mac */
		SDL_Rect bounds;
		if (SDL_GetDisplayBounds(0, &bounds) != 0) {
			log_error("SDL_GetDisplayBounds failed: %s", SDL_GetError());
			return false;
		}
		rect->y = bounds.h - rect->h - rect->y;
		usable_bounds = *rect;
	}
#endif
	return true;
}

static
b32 gui__maximum_window_rect(const gui_t *gui, SDL_Rect *rect)
{
	int display_idx;

	display_idx = SDL_GetWindowDisplayIndex(gui->window);
	if (display_idx < 0) {
		log_error("SDL_GetWindowDisplayIndex failed: %s", SDL_GetError());
		return false;
	}

	if (!gui__get_display_usable_bounds(display_idx, rect))
		return false;

	return true;
}

b32 gui_is_maximized(const gui_t *gui)
{
	SDL_Rect rect;
	return gui__maximum_window_rect(gui, &rect)
	    && gui->window_dim.x == rect.w
	    && gui->window_dim.y == rect.h;
}

void gui_minimize(gui_t *gui)
{
	SDL_MinimizeWindow(gui->window);
}

static
void gui__store_window_rect(gui_t *gui)
{
	v2i pos;
	SDL_GetWindowPosition(gui->window, &pos.x, &pos.y);
	gui->window_restore_pos.x = pos.x;
	gui->window_restore_pos.y = pos.y;
	gui->window_restore_dim.x = gui->window_dim.x;
	gui->window_restore_dim.y = gui->window_dim.y;
}

void gui_maximize(gui_t *gui)
{
	/* NOTE(rgriege): needs a workaround because
	 * SDL_MaximizeWindow consumes the menu bar on windoge */
	SDL_Rect rect;
	gui__store_window_rect(gui);
	if (gui__maximum_window_rect(gui, &rect)) {
		SDL_SetWindowPosition(gui->window, rect.x, rect.y);
		SDL_SetWindowSize(gui->window, rect.w, rect.h);
	} else {
		/* fallback */
		SDL_MaximizeWindow(gui->window);
	}
}

void gui_restore(gui_t *gui)
{
	const v2i pos = gui->window_restore_pos;
	const v2i dim = gui->window_restore_dim;
	/* NOTE(rgriege): order is important here */
	SDL_SetWindowSize(gui->window, dim.x, dim.y);
	SDL_SetWindowPosition(gui->window, pos.x, pos.y);
}

void gui_fullscreen(gui_t *gui)
{
	SDL_Rect rect;
		gui__store_window_rect(gui);
	if (gui__maximum_window_rect(gui, &rect))
		SDL_MaximizeWindow(gui->window);
}

static
b32 gui__key_is_mod(gui_key_t key)
{
	switch (key) {
	case KB_CAPSLOCK:
	case KB_LCTRL:
	case KB_LSHIFT:
	case KB_LALT:
	case KB_RCTRL:
	case KB_RSHIFT:
	case KB_RALT:
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
void gui__defocus_widget(gui_t *gui, u64 id)
{
	u32 slot;
	if (gui__widget_focus_slot(gui, id, &slot)) {
		for (u32 i = slot; i < countof(gui->focus_ids); ++i)
			gui->focus_ids[i] = 0;
	} else {
		assert(0);
	}
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

static
void gui__defocus_popup(gui_t *gui)
{
	gui->popup.id = 0;
}

b32 gui_begin_frame(gui_t *gui)
{
	s32 key_cnt;
	b32 quit = false, left_window = false;
	const colorf_t bg_color = color_to_colorf(gui->style.bg_color);
	SDL_Event evt;
	const u32 last_mouse_btn = gui->mouse_btn;
	timepoint_t now = time_current();

	gui->frame_time_milli = time_diff_milli(gui->frame_start_time, now);
	gui->frame_start_time = now;

	SDL_GL_MakeCurrent(gui->window, gui->gl_context);

	gui->mouse_btn = 0;
	gui->text_npt[0] = '\0';
	while (SDL_PollEvent(&evt) == 1) {
		switch (evt.type) {
		case SDL_QUIT:
			quit = true;
		break;
		case SDL_MOUSEWHEEL:
			if (evt.wheel.y != 0) {
				gui->mouse_btn |= (evt.wheel.y > 0 ? MB_WHEELUP : MB_WHEELDOWN);
				gui->last_input_time = now;
			}
		break;
		case SDL_WINDOWEVENT:
			switch (evt.window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				quit = true;
			break;
			case SDL_WINDOWEVENT_LEAVE:
				left_window = true;
			break;
			default:
				gui->last_input_time = now;
			break;
			}
		break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			gui->last_input_time = now;
		break;
		case SDL_TEXTINPUT:
			if (   strlen(gui->text_npt) + strlen(evt.text.text) + 1
			    <= countof(gui->text_npt))
				strcat(gui->text_npt, evt.text.text);
		break;
		}
	}

	SDL_GetWindowSize(gui->window, &gui->window_dim.x, &gui->window_dim.y);

	gui->mouse_pos_last = gui->mouse_pos;
	if (gui->dragging_window) {
		v2i w, gm;
		SDL_GetWindowPosition(gui->window, &w.x, &w.y);
		gui->mouse_btn |= SDL_GetGlobalMouseState(&gm.x, &gm.y);
		gui->mouse_btn_diff = gui->mouse_btn ^ last_mouse_btn;
		gui->mouse_pos = v2i_sub(gm, w);
	} else if (left_window) {
		gui->mouse_btn = 0;
		gui->mouse_btn_diff = 0;
		gui->hot_id = 0;
		gui->active_id = 0;
	} else {
		gui->mouse_btn |= SDL_GetMouseState(&gui->mouse_pos.x, &gui->mouse_pos.y);
		gui->mouse_btn_diff = gui->mouse_btn ^ last_mouse_btn;
	}
	gui->mouse_pos.y = gui->window_dim.y - gui->mouse_pos.y;

	if (mouse_pressed(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT))
		gui->mouse_pos_press = gui->mouse_pos;
	gui__repeat_update(&gui->mouse_repeat, gui->mouse_btn,
	                   __builtin_popcount(gui->mouse_btn),
	                   gui->frame_time_milli);
	if (mouse_down(gui, ~0))
		gui->last_input_time = now;

	gui->mouse_covered_by_widget_id = ~0;

	gui->keys = SDL_GetKeyboardState(&key_cnt);
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
	assert(key_cnt > KB_COUNT);
	gui__toggle_key(gui, KBT_CAPS, KB_CAPSLOCK);
	gui__toggle_key(gui, KBT_SCROLL, KB_SCROLLLOCK);
	gui__toggle_key(gui, KBT_NUM, KB_NUMLOCK_OR_CLEAR);

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
		gui->popup.id = 0;
		if (SDL_IsTextInputActive())
			SDL_StopTextInput();
	}
	gui->focus_id_found_this_frame = false;

	if (gui__key_triggered(gui, KB_TAB) && gui->focus_ids[0] == 0)
		gui__tab_focus_adjacent_widget(gui);

	gui->active_id_at_frame_start = gui->active_id;

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

	gui->vert_cnt = 0;
	gui->draw_call_cnt = 0;
	gui->draw_call_vert_idx = 0;
	memclr(gui->layers);
	gui->layer = &gui->layers[0];
	gui__layer_init(gui, gui->layer, 0, 0, gui->window_dim.x, gui->window_dim.y);
	memclr(gui->masks);
	gui->mask = &gui->masks[0];
	box2i_from_xywh(gui->mask, 0, 0, gui->window_dim.x, gui->window_dim.y);

	GL_CHECK(glViewport, 0, 0, gui->window_dim.x, gui->window_dim.y);

	/* NOTE(rgriege): reset the scissor for glClear */
	GL_CHECK(glScissor, 0, 0, gui->window_dim.x, gui->window_dim.y);

	GL_CHECK(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	GL_CHECK(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* popup */
	gui->popup.inside = false;
	if (gui->popup.id != 0) {
		const s32 x = gui->popup.mask.x;
		const s32 y = gui->popup.mask.y;
		const s32 w = gui->popup.mask.w;
		const s32 h = gui->popup.mask.h;
		box2i box;
		box2i_from_xywh(&box, x, y, w, h);

		assert(gui->focus_ids[0] == gui->popup.id);

		/* catch mouse_covered_by_panel */
		if (box2i_contains_point(box, gui->mouse_pos))
			gui->mouse_covered_by_widget_id = gui->popup.id;
	}

	/* ensure this is set every frame by a gui_window_drag() call (perf) */
	gui->dragging_window = false;

	gui->use_default_cursor = true;
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

	return !quit;
}

static const GLenum g_draw_call_types[GUI_DRAW_COUNT] = {
	GL_POINTS,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_LINES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_TRIANGLES,
	GL_QUAD_STRIP,
	GL_QUADS,
	GL_POLYGON,
};

static
box2i gui__current_mask(const gui_t *gui)
{
	const gui__layer_t *layer = gui->layer;
	box2i box;
	box2i_from_xywh(&box, layer->x, layer->y, layer->w, layer->h);
	return box;
}

static
void gui__mask_box(const gui_t *gui, box2i *box)
{
	const box2i mask = gui__current_mask(gui);
	box2i_clamp_point(mask, &box->min);
	box2i_clamp_point(mask, &box->max);
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

static
void gui__widget_bounds_push(gui_t *gui, gui_widget_bounds_t *widget_bounds)
{
	box2i_extend_box(&gui->widget_bounds->bbox, widget_bounds->bbox);
	widget_bounds->prev = gui->widget_bounds;
	gui->widget_bounds = widget_bounds;
}

static
void gui__widget_bounds_pop(gui_t *gui, b32 apply_to_prev)
{
	if (gui->widget_bounds->prev) {
		if (apply_to_prev)
			box2i_extend_box(&gui->widget_bounds->prev->bbox, gui->widget_bounds->bbox);
		gui->widget_bounds = gui->widget_bounds->prev;
	} else {
		assert(false);
		gui->widget_bounds = &gui->default_widget_bounds;
	}
}

static
void gui__widget_bounds_extend(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, x, y, w, h);
	box2i_extend_box(&gui->widget_bounds->bbox, box);
}

b32 gui_begin(gui_t *gui, u32 num_verts, gui_draw_call_type_e type)
{
	return gui_begin_tex(gui, num_verts, type, 0, TEXTURE_BLEND_NRM);
}

b32 gui_begin_tex(gui_t *gui, u32 num_verts, gui_draw_call_type_e type,
                  u32 tex, texture_blend_e blend)
{
	assert(num_verts > 0);
	if (   gui->vert_cnt + num_verts <= GUI_MAX_VERTS
	    && gui->draw_call_cnt < GUI_MAX_DRAW_CALLS) {
		draw_call_t *draw_call = &gui->draw_calls[gui->draw_call_cnt];
		draw_call->idx   = gui->vert_cnt;
		draw_call->cnt   = num_verts;
		draw_call->type  = type;
		draw_call->tex   = tex != 0 ? tex : gui->texture_white.handle;
		draw_call->blend = blend;
		gui->draw_call_vert_idx = 0;
		return true;
	} else {
		assert(false);
		return false;
	}
}

static
void gui__vertf(gui_t *gui, r32 x, r32 y, color_t c, r32 u, r32 v)
{
	const u32 idx_local = gui->draw_call_vert_idx++;
	const u32 idx       = gui->vert_cnt + idx_local;
	assert(idx_local < (u32)gui->draw_calls[gui->draw_call_cnt].cnt);
	gui->verts[idx].x           = x;
	gui->verts[idx].y           = y;
	gui->vert_colors[idx]       = c;
	gui->vert_tex_coords[idx].x = u;
	gui->vert_tex_coords[idx].y = v;
}

void gui_vertf(gui_t *gui, r32 x, r32 y, color_t c, r32 u, r32 v)
{
	box2i_extend_point(&gui->widget_bounds->bbox, (v2i){ (s32)x, (s32)y });
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

	box2i_extend_box(&gui->widget_bounds->bbox, bbox);

	if (!gui__box_visible(gui, bbox))
		return;

	if (color_equal(fill, g_nocolor))
		return;

	assert(n % 3 == 0);

	if (gui_begin(gui, n, GUI_DRAW_TRIANGLES)) {
		for (u32 i = 0; i < n; ++i)
			gui__vertf(gui, v[i].x, v[i].y, fill, 0.f, 0.f);
		gui_end(gui);
	}
}

static
void gui__poly(gui_t *gui, const v2f *v, u32 n, gui_draw_call_type_e type,
               color_t fill, color_t stroke, b32 closed)
{
	const box2i bbox = gui__vert_bounds(v, n);

	box2i_extend_box(&gui->widget_bounds->bbox, bbox);

	if (!gui__box_visible(gui, bbox))
		return;

	if (!color_equal(fill, g_nocolor) && gui_begin(gui, n, type)) {
		for (u32 i = 0; i < n; ++i)
			gui__vertf(gui, v[i].x, v[i].y, fill, 0.f, 0.f);
		gui_end(gui);
	}

	if (!color_equal(stroke, g_nocolor)) {
		const r32 dash_len = gui->style.line.dash_len;
		if (dash_len != 0.f) {
			if (gui_begin_tex(gui, closed ? n + 1 : n, GUI_DRAW_LINE_STRIP,
			                  gui->texture_white_dotted.handle, TEXTURE_BLEND_NRM)) {
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

m3f texture_get_transform(const texture_t *texture, s32 x, s32 y,
                          r32 sx, r32 sy, r32 rotation)
{
	const v2f scale = {
		((r32)texture->width) * sx,
		((r32)texture->height) * sy 
	};

	m3f transform = m3f_init_translation((v2f){ x, y });

	if (rotation != 0.f) {

		transform = m3f_mul_m3(transform,
		                       m3f_init_translation((v2f){scale.x / 2.f, scale.y / 2.f}));

		transform = m3f_mul_m3(transform,
		                       m3f_init_rotation(rotation));

		transform = m3f_mul_m3(transform,
		                       m3f_init_translation((v2f){-scale.x / 2.f, -scale.y / 2.f}));
	}

	return m3f_mul_m3(transform, m3f_init_scale(scale));
}

static
void texture__render(gui_t *gui, const texture_t *texture, s32 x, s32 y,
                     r32 sx, r32 sy, r32 rotation, color_t color)
{
	static const v2f corners[] = {
		{ 0.f, 0.f },
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f }
	};
	const m3f transform = texture_get_transform(texture, x, y, sx, sy, rotation);
	v2f points[4];
	box2i bbox;

	for (u32 i = 0; i < countof(points); ++i)
		points[i] = m3f_mul_v2(transform, corners[i]);

	bbox = gui__vert_bounds(B2PC(points));

	box2i_extend_box(&gui->widget_bounds->bbox, bbox);

	if (!gui__box_visible(gui, bbox))
		return;

	if (texture->blend == TEXTURE_BLEND_MUL) {
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
void text__render(gui_t *gui, const texture_t *texture, r32 x0, r32 y0,
                  r32 x1, r32 y1, r32 s0, r32 t0, r32 s1, r32 t1, color_t color)
{
	const box2i bbox = { .min = { (s32)x0, (s32)y0 }, .max = { (s32)x1, (s32)y1 } };

	box2i_extend_box(&gui->widget_bounds->bbox, bbox);

	if (!gui__box_visible(gui, bbox))
		return;

	if (gui_begin_tex(gui, 4, GUI_DRAW_TRIANGLE_FAN, texture->handle, texture->blend)) {
		gui__vertf(gui, x0, y1, color, s0, 1.f - t0);
		gui__vertf(gui, x0, y0, color, s0, 1.f - t1);
		gui__vertf(gui, x1, y0, color, s1, 1.f - t1);
		gui__vertf(gui, x1, y1, color, s1, 1.f - t0);
		gui_end(gui);
	}
}

static void gui__layer_complete_current(gui_t *gui);
static int gui__layer_sort(const void *lhs, const void *rhs);

void gui_end_frame(gui_t *gui)
{
#ifdef __EMSCRIPTEN__
	const s32 *loc = gui->shader_attrib_loc;
#else
	const s32 loc[VBO_COUNT] = { VBO_VERT, VBO_COLOR, VBO_TEX };
#endif
	GLuint current_texture = 0;

	texture_blend_e current_blend = TEXTURE_BLEND_NRM;

	assert(gui->grid == NULL);

	if (gui->root_split && !gui->splits_rendered_this_frame)
		gui_splits_render(gui);

	gui__layer_complete_current(gui);

	const u32 n_layers = gui->layer - &gui->layers[0] + 1;
	/* move hints/popups to the back of the array
	 * can't use qsort - not guaranteed to be stable */
	isort(gui->layers, n_layers, sizeof(gui->layers[0]), gui__layer_sort);
	/* front-to-back -> back-to-front */
	reverse(gui->layers, sizeof(gui->layers[0]), n_layers);


	GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GL_CHECK(glDisable, GL_DEPTH_TEST);
	GL_CHECK(glBindVertexArray, gui->vao);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, gui->vbo[VBO_VERT]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, gui->vert_cnt * sizeof(v2f),
	         gui->verts, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, loc[VBO_VERT], 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, loc[VBO_VERT]);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, gui->vbo[VBO_COLOR]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, gui->vert_cnt * sizeof(color_t),
	         gui->vert_colors, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, loc[VBO_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, loc[VBO_COLOR]);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, gui->vbo[VBO_TEX]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, gui->vert_cnt * sizeof(v2f),
	         gui->vert_tex_coords, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, loc[VBO_TEX], 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, loc[VBO_TEX]);

	GL_CHECK(glUseProgram, gui->shader.handle);
	GL_CHECK(glUniform2f, glGetUniformLocation(gui->shader.handle, "window_halfdim"),
	         gui->window_dim.x/2, gui->window_dim.y/2);

	/* NOTE(rgriege): This method of ordering creates an inconsistency:
	 * panels/layers must be called from top-to-bottom, but widgets/primitives
	 * within a layer must be called from bottom-to-top.  Without introducing a
	 * frame of delay, top-to-bottom panels are unavoidable to ensure that
	 * the top panels receive the input events.  Bottom-to-top widget rendering
	 * is nice for overlaying text on top of a movable widget. Will be a problem
	 * if overlapping widges are in the same panel/layer, but that doesn't seem
	 * like a use case to design for other than dragging icons on a desktop,
	 * which could be 'solved' by placing the dragged icon on a separate layer. */
	for (u32 i = 0; i < n_layers; ++i) {
		const gui__layer_t *layer = &gui->layers[i];
		GL_CHECK(glScissor, layer->x, layer->y, layer->w, layer->h);
		for (u32 j = 0; j < layer->draw_call_cnt; ++j) {
			draw_call_t *draw_call = &gui->draw_calls[layer->draw_call_idx+j];
			if (draw_call->tex != current_texture) {
				GL_CHECK(glBindTexture, GL_TEXTURE_2D, draw_call->tex);
				current_texture = draw_call->tex;
			}
			if (draw_call->blend != current_blend) {
				switch (draw_call->blend) {
					case TEXTURE_BLEND_NRM:
						GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						break;
					case TEXTURE_BLEND_ADD:
						GL_CHECK(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
						break;
					case TEXTURE_BLEND_MUL:
						GL_CHECK(glBlendFunc, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
						break;
				}
				current_blend = draw_call->blend;
			}

			GL_CHECK(glDrawArrays, g_draw_call_types[draw_call->type],
			         draw_call->idx, draw_call->cnt);
		}
	}

	if (gui->use_default_cursor)
		SDL_SetCursor(gui->cursors[GUI__CURSOR_DEFAULT]);

	GL_CHECK(glFlush);
	SDL_GL_SwapWindow(gui->window);

	memcpy(gui->prev_keys, gui->keys, KB_COUNT);
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

void gui_run(gui_t *gui, u32 fps, b32(*ufunc)(gui_t *gui, void *udata),
             void *udata)
{
	const u32 target_frame_milli = 1000/fps;
	u32 frame_milli;
	b32 quit = false;
	while(gui_begin_frame(gui) && !quit) {
		vlt_mem_advance_gen();
		quit = ufunc(gui, udata);
		gui_end_frame(gui);
		frame_milli = time_diff_milli(gui_frame_start(gui), time_current());
		if (frame_milli < target_frame_milli)
			time_sleep_milli(target_frame_milli - frame_milli);
		else
			log_warn("long frame: %ums", frame_milli);
	}
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

void mouse_pos_global(const gui_t *gui, s32 *x, s32 *y)
{
	SDL_GetGlobalMouseState(x, y);
}

b32 mouse_pos_changed(const gui_t *gui)
{
	return !v2i_equal(gui->mouse_pos, gui->mouse_pos_last);
}

b32 mouse_pressed(const gui_t *gui, u32 mask)
{
	return (gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
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
	return !(gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b32 mouse_released_bg(const gui_t *gui, u32 mask)
{
	return mouse_released(gui, mask) && mouse_over_bg(gui);
}

static
b32 gui__mouse_covered(const gui_t *gui)
{
	return gui->mouse_covered_by_widget_id != ~0
	    && !(gui->mouse_covered_by_widget_id == gui->popup.id && gui->popup.inside);
}

b32 mouse_over_bg(const gui_t *gui)
{
	return gui->active_id == 0
	    && gui->active_id_at_frame_start == 0
	    && !gui__mouse_covered(gui);
}

void mouse_scroll(const gui_t *gui, s32 *dir)
{
	if (gui->mouse_btn & MB_WHEELUP)
		*dir = 1;
	else if (gui->mouse_btn & MB_WHEELDOWN)
		*dir = -1;
	else
		*dir = 0;
}

void mouse_scroll_bg(const gui_t *gui, s32 *dir)
{
	if (mouse_over_bg(gui))
		mouse_scroll(gui, dir);
}

void mouse_press_debug(gui_t *gui, b32 enabled)
{
	gui->mouse_debug = enabled;
}

b32 key_down(const gui_t *gui, gui_key_t key)
{
	return gui->keys[key];
}

b32 key_pressed(const gui_t *gui, gui_key_t key)
{
	return gui->keys[key] && !gui->prev_keys[key];
}

b32 key_released(const gui_t *gui, gui_key_t key)
{
	return !gui->keys[key] && gui->prev_keys[key];
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
	gui_rect(gui, x, y, w, h, g_white, g_nocolor);
	gui->vert_colors[gui->vert_cnt-4] = bl;
	gui->vert_colors[gui->vert_cnt-3] = br;
	gui->vert_colors[gui->vert_cnt-2] = tr;
	gui->vert_colors[gui->vert_cnt-1] = tl;
}

void gui_arc(gui_t *gui, s32 x, s32 y, s32 r, r32 angle_start, r32 angle_end,
             color_t fill, color_t stroke)
{
	array_set_sz(gui->vert_buf, arc_poly_sz(r, angle_start, angle_end));
	arc_to_poly(x, y, r, angle_start, angle_end, A2PN(gui->vert_buf), false);
	gui__poly(gui, A2PN(gui->vert_buf), GUI_DRAW_TRIANGLE_FAN, fill, stroke, false);
	array_clear(gui->vert_buf);
}

void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t stroke)
{
	array_set_sz(gui->vert_buf, arc_poly_sz(r, 0, fPI * 2.f));
	arc_to_poly(x, y, r, 0, fPI * 2.f, A2PN(gui->vert_buf), true);
	gui__poly(gui, A2PN(gui->vert_buf), GUI_DRAW_TRIANGLE_FAN, fill, stroke, true);
	array_clear(gui->vert_buf);
}

void gui_trisf(gui_t *gui, const v2f *v, u32 n, color_t fill)
{
	gui__triangles(gui, v, n, fill);
}

void gui_poly(gui_t *gui, const v2i *v, u32 n, color_t fill, color_t stroke)
{
	array_set_sz(gui->vert_buf, n);
	for (u32 i = 0; i < n; ++i) {
		gui->vert_buf[i].x = v[i].x;
		gui->vert_buf[i].y = v[i].y;
	}
	gui_polyf(gui, gui->vert_buf, n, fill, stroke);
	array_clear(gui->vert_buf);
}

void gui_polyf(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t stroke)
{
	if (n == 3 || fill.a == 0 || polyf_is_convex(v, n)) {
		gui__poly(gui, v, n, GUI_DRAW_TRIANGLE_FAN, fill, stroke, true);
	} else {
		if (triangulate(v, n, &gui->vert_buf)) {
			gui__triangles(gui, A2PN(gui->vert_buf), fill);
			array_clear(gui->vert_buf);
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
}

void gui_polyline(gui_t *gui, const v2i *v, u32 n, color_t stroke)
{
	array_set_sz(gui->vert_buf, n);
	for (u32 i = 0; i < n; ++i) {
		gui->vert_buf[i].x = v[i].x;
		gui->vert_buf[i].y = v[i].y;
	}
	gui__poly(gui, A2PN(gui->vert_buf), GUI_DRAW_TRIANGLE_FAN, g_nocolor, stroke, false);
	array_clear(gui->vert_buf);
}

void gui_polylinef(gui_t *gui, const v2f *v, u32 n, r32 w, color_t stroke)
{
	assert(n >= 2 && w >= 1.f);
	if (w == 1.f) {
		gui__poly(gui, v, n, GUI_DRAW_TRIANGLE_FAN, g_nocolor, stroke, false);
	} else {
		const r32 w2    = w / 2.f;
		const v2f dir0  = v2f_scale(v2f_dir(v[0], v[1]), w2);
		const v2f perp0 = v2f_lperp(dir0);
		const v2f dirn  = v2f_scale(v2f_dir(v[n-2], v[n-1]), w2);
		const v2f perpn = v2f_lperp(dirn);

		array_set_sz(gui->vert_buf, 2*n);

		gui->vert_buf[0] = v2f_sub(v2f_sub(v[0], dir0), perp0);
		gui->vert_buf[1] = v2f_add(v2f_sub(v[0], dir0), perp0);

		for (u32 i = 1; i < n-1; ++i) {
			gui__polyline_corner_offset(v[i-1], v[i], v[i+1], w2, &gui->vert_buf[2*i]);
			gui->vert_buf[2*i+1] = v2f_add(v[i], v2f_sub(v[i], gui->vert_buf[2*i]));
		}

		gui->vert_buf[2*n-2] = v2f_sub(v2f_add(v[n-1], dirn), perpn);
		gui->vert_buf[2*n-1] = v2f_add(v2f_add(v[n-1], dirn), perpn);

		gui__poly(gui, A2PN(gui->vert_buf), GUI_DRAW_TRIANGLE_STRIP, stroke, g_nocolor, true);
		array_clear(gui->vert_buf);
	}
}

static
cached_img_t *gui__find_img(gui_t *gui, u32 id)
{
	array_foreach(gui->imgs, cached_img_t, ci)
		if (ci->id == id)
			return ci;
	return NULL;
}

img_t *gui_init_cached_img(gui_t *gui, const char *name)
{
	const u32 id = hash(name);
	cached_img_t *cached_img;

	cached_img = array_append_null(gui->imgs);
	cached_img->id = id;
	return &cached_img->img;
}

static
const img_t *gui__find_or_load_img(gui_t *gui, const char *fname)
{
	const u32 id = hash(fname);
	cached_img_t *cached_img = gui__find_img(gui, id);
	if (cached_img)
		return &cached_img->img;

	cached_img = array_append_null(gui->imgs);
	cached_img->id = id;
	if (img_load(&cached_img->img, fname))
		return &cached_img->img;

	array_pop(gui->imgs);
	return NULL;
}

void gui_img(gui_t *gui, s32 x, s32 y, const char *fname)
{
	const img_t *img = gui__find_or_load_img(gui, fname);
	if (img)
		gui_img_ex(gui, x, y, img, 1.f, 1.f, 0.f, 1.f);
}

void gui_img_ex(gui_t *gui, s32 x, s32 y, const img_t *img, r32 sx, r32 sy,
                r32 rotation, r32 opacity)
{
	color_t color = g_white;
	color.a = opacity * 255;
	texture__render(gui, &img->texture, x, y, sx, sy, rotation, color);
}

void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                   img_scale_t scale)
{
	const img_t *img = gui__find_or_load_img(gui, fname);
	if (!img)
		return;

	switch (scale) {
	case IMG_CENTERED:;
		const s32 x_off = (w - img->texture.width) / 2;
		const s32 y_off = (h - img->texture.height) / 2;
		if (x_off >= 0 && y_off >= 0) {
			gui_img_ex(gui, x + x_off, y + y_off, img, 1.f, 1.f, 0.f, 1.f);
			break;
		}
	case IMG_SCALED:;
		const r32 sx = (r32)w / img->texture.width;
		const r32 sy = (r32)h / img->texture.height;
		gui_img_ex(gui, x, y, img, sx, sy, 0.f, 1.f);
	break;
	}
}

static inline
font_t *gui__get_font(gui_t *gui, u32 sz)
{
	font_t *font = gui->fonts, *font_end = array_end(gui->fonts);
	while (font != font_end && font->sz != sz)
		++font;
	if (font != font_end)
		return font;

	font = array_append_null(gui->fonts);
	if (font_load(font, gui->font_file_path, sz)) {
		return font;
	} else {
		array_pop(gui->fonts);
		assert(false);
		return NULL;
	}
}

static
void gui__fixup_stbtt_aligned_quad(stbtt_aligned_quad *q, r32 yb)
{
	/* NOTE(rgriege): stbtt assumes y=0 at top, but for violet y=0 is at bottom */
	const r32 dy = q->y1 - q->y0;
	q->y0 = yb + (yb - q->y1);
	q->y1 = q->y0 + dy;
}

static
void gui__add_codepoint_to_line_width(font_t *font, s32 cp, r32 *line_width)
{
	const texture_t *texture = &font->texture;
	r32 y = 0;
	stbtt_aligned_quad q;

	if (cp < font->num_glyphs)
		stbtt_GetPackedQuad(font->char_info, texture->width, texture->height,
		                    cp, line_width, &y, &q, 1);
}

static
void gui__wrap_txt(gui_t *gui, char *txt, const gui_text_style_t *style, r32 max_width)
{
	char *p = txt;
	char *pnext;
	s32 cp = utf8_next_codepoint(p, &pnext);
	r32 line_width = style->padding;

	font_t *font = gui__get_font(gui, style->size);
	if (!font)
		return;

	while (cp != 0) {
		char *p_space_before_word = NULL;
		r32 line_width_before_word;

		if (cp == ' ') {
			p_space_before_word = p;
			gui__add_codepoint_to_line_width(font, cp, &line_width);
			p = pnext;
			cp = utf8_next_codepoint(p, &pnext);
		} else if (cp == '\n') {
			p = pnext;
			cp = utf8_next_codepoint(p, &pnext);
			line_width = style->padding;
		}
		line_width_before_word = line_width;

		while (cp != ' ' && cp != '\n' && cp != 0) {
			gui__add_codepoint_to_line_width(font, cp, &line_width);
			p = pnext;
			cp = utf8_next_codepoint(p, &pnext);
		}

		if (line_width >= max_width) {
			if (p_space_before_word)
				*p_space_before_word = '\n';
			line_width = style->padding + line_width - line_width_before_word;
		}
	}
}

static
void gui__txt_char_pos(gui_t *gui, s32 *ix, s32 *iy, s32 w, s32 h,
                       const char *txt_, u32 pos, const gui_text_style_t *style)
{
	font_t *font;
	s32 ix_ = *ix, iy_ = *iy;
	r32 x, y;
	stbtt_aligned_quad q;
	array(char) buf = NULL;
	const char *txt = txt_;
	const char *p = txt;
	char *pnext;
	s32 cp;
	u32 i;

	font = gui__get_font(gui, style->size);
	if (!font)
		return;

	if (style->wrap) {
		const u32 len = (u32)strlen(txt);
		array_init_ex(buf, len + 1, g_temp_allocator);
		memcpy(buf, txt, len + 1);
		gui__wrap_txt(gui, buf, style, w);
		txt = buf;
		p = buf;
	}

	gui_align_anchor(ix_, iy_, w, h, style->align, &ix_, &iy_);
	x = ix_ + font__line_offset_x(font, txt, style);
	y = iy_ + font__offset_y(font, txt, style);

	if (pos == 0)
		goto out;

	i = 0;
	while (i < pos && (cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (cp == '\n') {
			y -= font->newline_dist;
			x = ix_ + font__line_offset_x(font, pnext, style);
		} else if (cp < font->num_glyphs) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, cp, &x, &y, &q, 1);
		}
		i += (pnext - p);
		p = pnext;
	}

out:
	*ix = x;
	*iy = y;
	if (buf)
		array_destroy(buf);
}

static
void gui__txt(gui_t *gui, s32 *ix, s32 *iy, const char *txt,
              const gui_text_style_t *style)
{
	font_t *font;
	r32 x = *ix, y = *iy;
	stbtt_aligned_quad q;
	const char *p = txt;
	char *pnext;
	s32 cp;

	font = gui__get_font(gui, style->size);
	if (!font)
		return;

	x += font__line_offset_x(font, txt, style);
	y += font__offset_y(font, txt, style);

	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (cp == '\n') {
			y -= font->newline_dist;
			x = *ix + font__line_offset_x(font, pnext, style);
		} else if (cp < font->num_glyphs) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, cp, &x, &y, &q, 1);
			gui__fixup_stbtt_aligned_quad(&q, y);
			text__render(gui, &font->texture, q.x0, q.y0, q.x1, q.y1, q.s0, q.t0,
			             q.s1, q.t1, style->color);
		}
		p = pnext;
	}
	*ix = x;
	*iy = y;
}

static
u32 gui__txt_mouse_pos(gui_t *gui, s32 xi_, s32 yi_, s32 w, s32 h,
                       const char *txt_, v2i mouse, const gui_text_style_t *style)
{
	font_t *font;
	s32 xi = xi_, yi = yi_;
	r32 x, y;
	stbtt_aligned_quad q;
	u32 closest_pos;
	s32 closest_dist, dist;
	v2i p;
	array(char) buf = NULL;
	const char *txt  = txt_;
	const char *ptxt = txt_;
	char *pnext;
	s32 cp;

	if (style->wrap) {
		const u32 len = (u32)strlen(txt);
		array_init_ex(buf, len + 1, g_temp_allocator);
		memcpy(buf, txt, len + 1);
		gui__wrap_txt(gui, buf, style, w);
		txt  = buf;
		ptxt = buf;
	}

	font = gui__get_font(gui, style->size);
	if (!font)
		return 0;

	gui_align_anchor(xi, yi, w, h, style->align, &xi, &yi);
	x = xi + font__line_offset_x(font, txt, style);
	y = yi + font__offset_y(font, txt, style);

	v2i_set(&p, x, y + font->ascent / 2);
	closest_pos = 0;
	closest_dist = v2i_dist_sq(p, mouse);

	while ((cp = utf8_next_codepoint(ptxt, &pnext)) != 0) {
		ptxt = pnext;
		if (cp == '\n') {
			y -= font->newline_dist;
			x = xi + font__line_offset_x(font, ptxt, style);
		} else if (cp < font->num_glyphs) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, cp, &x, &y, &q, 1);
		} else {
			continue;
		}
		v2i_set(&p, roundf(x), roundf(y + font->ascent / 2));
		dist = v2i_dist_sq(p, mouse);
		if (dist < closest_dist) {
			closest_dist = dist;
			closest_pos = ptxt - txt;
		}
	}
	if (buf)
		array_destroy(buf);
	return closest_pos;
}

void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt,
             color_t c, gui_align_t align)
{
	const gui_text_style_t style = {
		.size = sz,
		.color = c,
		.align = align,
		.padding = 0,
		.wrap = false,
	};
	gui__txt(gui, &x, &y, txt, &style);
}

void gui_txt_dim(gui_t *gui, s32 x_, s32 y_, s32 sz, const char *txt,
                 gui_align_t align, s32 *px, s32 *py, s32 *pw, s32 *ph)
{
	const char *p = txt;
	char *pnext;
	s32 cp;
	font_t *font;
	r32 x = x_, y = y_;
	ivalf x_range, y_range;
	stbtt_aligned_quad q;
	const gui_text_style_t style = {
		.size = sz,
		.color = g_nocolor,
		.align = align,
		.padding = 0,
		.wrap = false,
	};

	font = gui__get_font(gui, style.size);
	if (!font) {
		*px = 0;
		*py = 0;
		*pw = 0;
		*ph = 0;
		return;
	}

	x += font__line_offset_x(font, txt, &style);
	y += font__offset_y(font, txt, &style);

	x_range.l = x_range.r = x;
	y_range.l = y_range.r = y;

	while ((cp = utf8_next_codepoint(p, &pnext)) != 0) {
		if (cp == '\n') {
			y -= font->newline_dist;
			x = x_ + font__line_offset_x(font, pnext, &style);
		} else if (cp < font->num_glyphs) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, cp, &x, &y, &q, 1);
			gui__fixup_stbtt_aligned_quad(&q, y);

			x_range.l = min(x_range.l, q.x0);
			x_range.r = max(x_range.r, q.x1);
			y_range.l = min(y_range.l, q.y0);
			y_range.r = max(y_range.r, q.y1);
		}
		p = pnext;
	}
	*px = x_range.l;
	*py = y_range.l;
	*pw = ivalf_length(x_range);
	*ph = ivalf_length(y_range);
}

s32 gui_txt_width(gui_t *gui, const char *txt, u32 sz)
{
	s32 width = 0;
	const char *line = txt;

	font_t *font = gui__get_font(gui, sz);
	if (!font)
		return 0;

	while (*line != '\0') {
		const s32 line_width = font__line_width(font, line);
		width = max(width, line_width);
		while (*line != '\0' && *line != '\n')
			++line;
		if (*line == '\n')
			++line;
	}
	return width;
}

static
void gui__layer_init(gui_t *gui, gui__layer_t *layer, s32 x, s32 y, s32 w, s32 h)
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
	const gui__layer_t *lhs = lhs_, *rhs = rhs_;
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
		gui__wrap_txt(gui, buf, style, w);
		gui__txt(gui, &x, &y, buf, style);
		array_destroy(buf);
	} else {
		gui__txt(gui, &x, &y, txt, style);
	}
}

/* Widgets */

static
b32 gui__allow_new_panel_interaction(const gui_t *gui)
{
	return (gui->hot_id == 0 || !gui->hot_id_found_this_frame)
	    && gui->active_id == 0
	    && !gui__mouse_covered(gui);
}

static
b32 gui__allow_new_interaction(const gui_t *gui)
{
	return (gui->hot_id == 0 || !gui->hot_id_found_this_frame)
	    && gui->active_id == 0
	    && !gui__mouse_covered(gui)
	    && !mouse_down(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT);
}

typedef enum gui__widget_render_state
{
	GUI__WIDGET_RENDER_INACTIVE,
	GUI__WIDGET_RENDER_HOT,
	GUI__WIDGET_RENDER_ACTIVE,
} gui__widget_render_state_t;

static
gui__widget_render_state_t gui__widget_render_state(const gui_t *gui, u64 id,
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
gui__widget_render_state_t gui__btn_render_state(const gui_t *gui, u64 id,
                                                 btn_val_t val, b32 contains_mouse)
{
	return gui__widget_render_state(gui, id, val != BTN_NONE, false, contains_mouse);
}

static
gui_element_style_t gui__element_style(const gui_t *gui,
                                       gui__widget_render_state_t render_state,
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

v2f *gui_verts_mut(gui_t *gui)
{
	return gui->verts;
}

u32 gui_verts_cnt(const gui_t *gui)
{
	return gui->vert_cnt;
}

const npt_filter_t g_gui_npt_filter_print = {
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

const npt_filter_t g_gui_npt_filter_numeric = {
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

const npt_filter_t g_gui_npt_filter_hex = {
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
void gui__npt_move_cursor_vertical(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                                   const char *txt, s32 diff, u32 fallback)
{
	const u32 orig_pos = gui->npt.cursor_pos;
	const gui_text_style_t *style = &gui->style.npt.active.text;
	v2i cursor = { .x = x, .y = y };

	gui__txt_char_pos(gui, &cursor.x, &cursor.y, w, h, txt, orig_pos, style);
	cursor.y += diff;
	gui->npt.cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, txt, cursor, style);
	if (gui->npt.cursor_pos == orig_pos && orig_pos != fallback)
		gui->npt.cursor_pos = fallback;
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
	    && !gui->lock;
}

static
b32 gui__popup_contains_mouse(const gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	return box2i_contains_point(box, gui->mouse_pos) && !gui->lock;
}

static
void gui__widget_handle_focus(gui_t *gui, u64 id, b32 contains_mouse)
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
			if (mouse_pressed(gui, ~0) && !contains_mouse && id != gui->popup.id)
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
void gui__npt_prep_action(gui_t *gui, npt_flags_t flags, char *txt, u32 *len)
{
	if (!gui->npt.performed_action && (flags & NPT_CLEAR_ON_FOCUS)) {
		txt[0] = '\0';
		*len = 0;
		gui->npt.cursor_pos = 0;
	}
	gui->npt.performed_action = true;
}

static
void gui__hint_render(gui_t *gui, u64 id, const char *hint)
{
	if (id == gui->hot_id && gui->hint_timer <= 0 && hint) {
		const gui_element_style_t *style = &gui->style.hint;
		const s32 x = gui->mouse_pos.x;
		const s32 y = gui->mouse_pos.y;
		s32 rx, ry, rw, rh;
		gui_txt_dim(gui, x, y, style->text.size, hint,
		            GUI_ALIGN_BOTLEFT, &rx, &ry, &rw, &rh);
		rx = (x + rw < gui->window_dim.x) ? x : x - rw;
		ry = (y + rh < gui->window_dim.y) ? y : y - rh;
		rw += 2 * style->text.padding;
		rh += 2 * style->text.padding;
		gui_mask_push(gui, rx, ry, rw, rh);
		gui->layer->pri = GUI__LAYER_PRIORITY_HINT;
		gui_rect(gui, rx, ry, rw, rh, style->bg_color, style->outline_color);
		gui_txt_styled(gui, rx, ry, rw, rh, hint, &style->text);
		gui_mask_pop(gui);
	}
}

b32 gui_npt_filter(npt_filter_p filter, s32 codepoint)
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
                const char *hint, npt_flags_t flags)
{
	return gui_npt_txt_ex(gui, x, y, w, h, txt, n, hint, flags,
	                      &g_gui_npt_filter_print);
}

static
btn_val_t gui__btn_logic(gui_t *gui, u64 id, b32 contains_mouse, mouse_button_t mb);

s32 gui_npt_txt_ex(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                   const char *hint, npt_flags_t flags, npt_filter_p filter)
{
	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_focused = gui_widget_focused(gui, id);
	const gui_widget_style_t *style = &gui->style.npt;
	b32 contains_mouse;
	b32 complete = 0;
	gui__widget_render_state_t render_state;
	gui_element_style_t elem_style;
	const char *displayed_txt;
	const char *txt_to_display;

	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	if (gui__btn_logic(gui, id, contains_mouse, MB_LEFT) == BTN_PRESS && !was_focused)
		gui__focus_widget(gui, id);

	if (gui->active_id == id && contains_mouse) {
		const gui_text_style_t *txt_style = &style->active.text;
		gui->npt.cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, txt,
		                                         gui->mouse_pos, txt_style);
	}
	if (gui_widget_focused(gui, id)) {
		u32 len = (u32)strlen(txt);
		gui->npt.cursor_pos = clamp(0, gui->npt.cursor_pos, len);
		if (strlen(gui->text_npt) > 0 && !key_mod(gui, KBM_CTRL)) {
			gui__npt_prep_action(gui, flags, txt, &len);
			const char *p = gui->text_npt;
			char *pnext;
			s32 cp;
			u32 cp_sz;
			while (   (cp = utf8_next_codepoint(p, &pnext)) != 0
			       && (cp_sz = pnext - p)
			       && len + cp_sz < n) {
				if (gui_npt_filter(filter, cp)) {
					/* move all bytes after the cursor (inc NUL) forward by cp_sz */
					for (u32 i = len + cp_sz; i > gui->npt.cursor_pos + cp_sz - 1; --i)
						txt[i] = txt[i-cp_sz];
					memcpy(&txt[gui->npt.cursor_pos], p, cp_sz);
					len += cp_sz;
					gui->npt.cursor_pos += cp_sz;
				}
				p = pnext;
			}
		} else if (gui->key_repeat.triggered) {
			const u32 key_idx = gui->key_repeat.val;
			if (key_idx == KB_BACKSPACE) {
				gui__npt_prep_action(gui, flags, txt, &len);
				if (gui->npt.cursor_pos > 0) {
					char *cursor = &txt[gui->npt.cursor_pos], *prev;
					utf8_prev_codepoint(cursor, &prev);
					gui->npt.cursor_pos -= (cursor - prev);
					buf_remove_n(txt, gui->npt.cursor_pos, cursor - prev, len+1);
				}
			} else if (key_idx == KB_DELETE) {
				gui__npt_prep_action(gui, flags, txt, &len);
				char *cursor = &txt[gui->npt.cursor_pos], *next;
				if (utf8_next_codepoint(cursor, &next) != 0)
					buf_remove_n(txt, gui->npt.cursor_pos, next - cursor, len+1);
			} else if (key_idx == KB_RETURN || key_idx == KB_KP_ENTER) {
				gui__npt_prep_action(gui, flags, txt, &len);
				gui__defocus_widget(gui, id);
				complete = NPT_COMPLETE_ON_ENTER;
			} else if (key_idx == KB_V && key_mod(gui, KBM_CTRL)) {
				char *clipboard;
				u32 sz;
				gui__npt_prep_action(gui, flags, txt, &len);
				if (   SDL_HasClipboardText()
				    && (clipboard = SDL_GetClipboardText())
				    && (sz = (u32)strlen(clipboard)) > 0
				    && len + sz < n) {
					memmove(&txt[gui->npt.cursor_pos + sz],
					        &txt[gui->npt.cursor_pos],
					        len - gui->npt.cursor_pos + 1);
					memcpy(&txt[gui->npt.cursor_pos], clipboard, sz);
					gui->npt.cursor_pos += sz;
					SDL_free(clipboard);
				}
			} else if (gui__key_up(gui)) {
				const gui_text_style_t *txt_style = &style->active.text;
				const font_t *font = gui__get_font(gui, txt_style->size);
				if (font) {
					const s32 dy = font->newline_dist;
					gui__npt_move_cursor_vertical(gui, x, y, w, h, txt, dy, 0);
				}
			} else if (gui__key_down(gui)) {
				const gui_text_style_t *txt_style = &style->active.text;
				const font_t *font = gui__get_font(gui, txt_style->size);
				if (font) {
					const s32 dy = -font->newline_dist;
					gui__npt_move_cursor_vertical(gui, x, y, w, h, txt, dy, len);
				}
			} else if (gui__key_left(gui)) {
				if (gui->npt.cursor_pos > 0) {
					char *cursor = &txt[gui->npt.cursor_pos], *prev;
					utf8_prev_codepoint(cursor, &prev);
					gui->npt.cursor_pos -= (cursor - prev);
				}
			} else if (gui__key_right(gui)) {
				char *cursor = &txt[gui->npt.cursor_pos], *next;
				if (utf8_next_codepoint(cursor, &next) != 0)
					gui->npt.cursor_pos += (next - cursor);
			} else if (gui__key_home(gui)) {
				gui->npt.cursor_pos = 0;
			} else if (gui__key_end(gui)) {
				gui->npt.cursor_pos = len;
			}
		}
	}

	if (was_focused != gui_widget_focused(gui, id)) {
		if (was_focused) {
			SDL_StopTextInput();
			if (complete || gui->lock) {
			} else if (   (flags & NPT_COMPLETE_ON_TAB)
			           && gui__key_triggered(gui, KB_TAB)) {
				complete = NPT_COMPLETE_ON_TAB;
			} else if (   (flags & NPT_COMPLETE_ON_CLICK_OUT)
			           && mouse_pressed(gui, ~0)
			           && !contains_mouse) {
				complete = NPT_COMPLETE_ON_CLICK_OUT;
			} else if (   (flags & NPT_COMPLETE_ON_ESCAPE)
			           && gui__key_triggered(gui, KB_ESCAPE)) {
				complete = NPT_COMPLETE_ON_ESCAPE;
			} else if (   (flags & NPT_COMPLETE_ON_UNCHANGED)
			           || gui->npt.initial_txt_hash != hashn(txt, n)) {
				complete = NPT_COMPLETE_ON_UNCHANGED;
			}
		} else {
			SDL_StartTextInput();
			if (flags & NPT_CLEAR_ON_FOCUS) {
				gui->npt.cursor_pos = 0;
			} else {
				const gui_text_style_t *txt_style = &style->active.text;
				gui->npt.cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, txt,
				                                         gui->mouse_pos, txt_style);
			}
			gui->npt.performed_action = false;
			gui->npt.initial_txt_hash = hashn(txt, n);
		}
	}

	render_state = gui__widget_render_state(gui, id, true, false,
	                                        contains_mouse || gui_widget_focused(gui, id));
	elem_style = gui__element_style(gui, render_state, style);
	style->pen(gui, x, y, w, h, &elem_style);
	txt_to_display = !gui_widget_focused(gui, id)
	              || gui->npt.performed_action
	              || !(flags & NPT_CLEAR_ON_FOCUS)
	               ? txt : "";

	if (flags & NPT_PASSWORD) {
		assert(strlen(txt_to_display) < GUI_TXT_MAX_LENGTH);
		const char *p = txt_to_display;
		char *pnext;
		u32 i = 0;
		while (i < GUI_TXT_MAX_LENGTH-1 && utf8_next_codepoint(p, &pnext) != 0) {
			gui->npt.pw_buf[i++] = '*';
			p = pnext;
		}
		gui->npt.pw_buf[i] = '\0';
		gui_txt_styled(gui, x, y, w, h, gui->npt.pw_buf, &elem_style.text);
		displayed_txt = gui->npt.pw_buf;
	} else {
		gui_txt_styled(gui, x, y, w, h, txt_to_display, &elem_style.text);
		displayed_txt = txt_to_display;
	}
	if (gui_widget_focused(gui, id)) {
		if (time_diff_milli(gui->creation_time, gui->frame_start_time) % 1000 < 500) {
			const color_t color = elem_style.text.color;
			const font_t *font = gui__get_font(gui, elem_style.text.size);
			if (font) {
				gui__txt_char_pos(gui, &x, &y, w, h, displayed_txt,
				                  gui->npt.cursor_pos, &elem_style.text);
				x += 1;
				gui_line(gui, x, y + font->descent, x, y + font->ascent, 1, color);
			}
		}
	} else if (txt[0] == 0 && hint) {
		gui_txt_styled(gui, x, y, w, h, hint, &elem_style.text);
	}
	gui__hint_render(gui, id, gui->style.npt.hint);
	gui->prev_widget_id = id;
	return complete;
}

const char *gui_npt_val_buf(const gui_t *gui)
{
	return gui->npt.val_buf;
}

s32 gui_npt_val(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, u32 n_,
                npt_flags_t flags, npt_filter_p filter)
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
btn_val_t gui__btn_logic(gui_t *gui, u64 id, b32 contains_mouse, mouse_button_t mb)
{
	btn_val_t retval = BTN_NONE;

	gui__widget_handle_focus(gui, id, contains_mouse);

	if (   gui__is_most_focused_widget(gui, id)
	    && !gui->lock
	    && gui->key_repeat.triggered
	    && (   gui->key_repeat.val == KB_RETURN
	        || gui->key_repeat.val == KB_KP_ENTER))
		retval = key_pressed(gui, gui->key_repeat.val) ? BTN_PRESS : BTN_HOLD;

	if (gui->active_id == id) {
		gui->active_id_found_this_frame = true;
		if (mouse_released(gui, mb)) {
			if (contains_mouse)
				retval = BTN_PRESS;
			gui->active_id = 0;
		} else if (contains_mouse && gui->mouse_repeat.triggered) {
			retval = BTN_HOLD;
		}
	} else if (gui->hot_id == id) {
		if (!contains_mouse || gui__mouse_covered(gui)) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, mb)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->active_id_found_this_frame = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}
	gui->prev_widget_id = id;
	return retval;
}

static
void gui__btn_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, gui__widget_render_state_t render_state,
                     const gui_widget_style_t *widget_style)
{
	const gui_element_style_t style = gui__element_style(gui, render_state, widget_style);
	widget_style->pen(gui, x, y, w, h, &style);
	gui_txt_styled(gui, x, y, w, h, txt, &style.text);
}

s32 gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return 0;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const btn_val_t ret = gui__btn_logic(gui, id, contains_mouse, MB_LEFT);

	const gui__widget_render_state_t render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.btn);
	gui__hint_render(gui, id, gui->style.btn.hint);

	return ret;
}

s32 gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                img_scale_t scale)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return 0;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const btn_val_t ret = gui__btn_logic(gui, id, contains_mouse, MB_LEFT);
	const gui__widget_render_state_t render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, &style);
	gui_img_boxed(gui, x, y, w, h, fname, scale);
	gui__hint_render(gui, id, gui->style.btn.hint);
	return ret;
}

s32 gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return 0;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const btn_val_t ret = gui__btn_logic(gui, id, contains_mouse, MB_LEFT);
	const gui__widget_render_state_t render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, &style);
	pen(gui, x, y, w, h, &style);
	gui__hint_render(gui, id, gui->style.btn.hint);
	return ret;
}

b32 gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, b32 *val)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	b32 toggled = false;
	gui__widget_render_state_t render_state;

	if (gui__btn_logic(gui, id, contains_mouse, MB_LEFT) == BTN_PRESS) {
		*val = !*val;
		toggled = true;
	}

	render_state = gui__widget_render_state(gui, id, toggled, *val, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.chk);
	gui__hint_render(gui, id, gui->style.chk.hint);
	return toggled;
}

b32 gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen, b32 *val)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	b32 toggled = false;
	gui__widget_render_state_t render_state;
	gui_element_style_t style;

	if (gui__btn_logic(gui, id, contains_mouse, MB_LEFT) == BTN_PRESS) {
		*val = !*val;
		toggled = true;
	}

	render_state = gui__widget_render_state(gui, id, toggled, *val, contains_mouse);
	style = gui__element_style(gui, render_state, &gui->style.chk);
	gui->style.chk.pen(gui, x, y, w, h, &style);
	pen(gui, x, y, w, h, &style);
	gui__hint_render(gui, id, gui->style.chk.hint);
	return toggled;
}

typedef enum gui__slider_orientation
{
	GUI__SLIDER_X,
	GUI__SLIDER_Y,
} gui__slider_orientation_t;

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
                gui__slider_orientation_t orientation)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_hot = (gui->hot_id == id);
	s32 hx, hy, hw, hh;
	b32 contains_mouse;
	b32 triggered_by_key = false;

	assert(*val >= 0.f && *val <= 1.f);

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
	gui__btn_logic(gui, id, contains_mouse, MB_LEFT);

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


	const gui__widget_render_state_t render_state
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
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_selected = *val == opt;
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	gui__widget_render_state_t render_state;
	b32 selected = false;

	if (   gui__btn_logic(gui, id, contains_mouse, MB_LEFT) == BTN_PRESS
	    && !was_selected) {
		*val = opt;
		selected = true;
	}

	render_state = gui__widget_render_state(gui, id, selected, *val == opt,
	                                        contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.select);
	gui__hint_render(gui, id, gui->style.select.hint);
	return selected;
}

void gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                 const char *txt, u32 *val, u32 opt)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	b32 selected = false;
	gui__widget_render_state_t render_state;

	if (gui__btn_logic(gui, id, contains_mouse, MB_LEFT) == BTN_PRESS) {
	  if (!(*val & opt)) {
			*val |= opt;
			selected = true;
		} else if (*val & ~opt) {
			*val &= ~opt;
			selected = true;
		}
	}

	render_state = gui__widget_render_state(gui, id, selected, *val & opt,
	                                        contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.select);
	gui__hint_render(gui, id, gui->style.select.hint);
}

static
void gui__popup_btn_logic(gui_t *gui, u64 id, b32 contains_mouse,
                          s32 px, s32 py, s32 pw, s32 ph)
{
	const b32 was_focused = gui_widget_focused(gui, id);

	if (gui__btn_logic(gui, id, contains_mouse, MB_LEFT) == BTN_PRESS) {
		if (gui_widget_focused(gui, id))
			gui__defocus_widget(gui, id);
		else
			gui__focus_widget(gui, id);
	}

	if (   gui_widget_focused(gui, id)
	    && mouse_pressed(gui, ~0)
	    && !contains_mouse
	    && !gui__popup_contains_mouse(gui, px, py, pw, ph))
		gui__defocus_widget(gui, id);

	if (was_focused && !gui_widget_focused(gui, id))
		gui__defocus_popup(gui);
}

static
void gui__popup_begin(gui_t *gui, u64 id, s32 x, s32 y, s32 w, s32 h)
{
	/* only 1 active popup allowed */
	assert(gui->popup.id == 0 || gui->popup.id == id);

	gui_mask_push(gui, x, y, w, h);
	gui->layer->pri = GUI__LAYER_PRIORITY_POPUP;

	gui->popup.id = id;
	gui->popup.mask.x = x;
	gui->popup.mask.y = y;
	gui->popup.mask.w = w;
	gui->popup.mask.h = h;
	gui->popup.close_at_end = false;
	gui->popup.inside = true;
}

static
void gui__popup_end(gui_t *gui)
{
	/* switch to new layer (mirroring interrupted layer) after last item */
	gui_mask_pop(gui);

	gui->popup.inside = false;

	if (gui->popup.close_at_end || gui->focus_next_widget) {
		gui__defocus_widget(gui, gui->popup.id);
		gui__defocus_popup(gui);
	}
}

b32 gui_dropdown_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                       u32 *val, u32 num_items)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const s32 pw = w;
	const s32 ph = h * num_items;
	const s32 px = x;
	const s32 py = y - ph;
	b32 triggered_by_key = false;
	gui__widget_render_state_t render_state;

	assert(gui->dropdown.id == 0); /* cannot nest dropdowns */

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
	strcpy(gui->dropdown.selected_item_txt, "");

	if (gui_widget_focused(gui, id)) {
		gui__popup_begin(gui, id, px, py, pw, ph);
		pgui_grid_begin(gui, &gui->grid_popup, px, py, pw, ph);
		pgui_col(gui, 0, num_items);
	}

	return true;
}

static
b32 gui__dropdown_item_selected(const gui_t *gui)
{
	return *gui->dropdown.val == gui->dropdown.item_idx;
}

b32 gui_dropdown_item(gui_t *gui, const char *txt)
{
	b32 chosen = false;

	assert(gui->dropdown.id != 0);
	assert(gui->dropdown.item_idx < gui->dropdown.num_items);

	if (gui_widget_focused(gui, gui->dropdown.id)) {
		gui_style_push(gui, btn, gui_style(gui)->dropdown);
		if (pgui_btn_txt(gui, txt) == BTN_PRESS) {
			*gui->dropdown.val = gui->dropdown.item_idx;
			chosen = true;
		} else if (   gui->dropdown.triggered_by_key
		           && gui__dropdown_item_selected(gui)) {
			chosen = true;
		}
		gui_style_pop(gui);
	}

	if (gui__dropdown_item_selected(gui)) {
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

	assert(strlen(txt) > 0);

	if (gui_widget_focused(gui, gui->dropdown.id)) {
		pgui_grid_end(gui, &gui->grid_popup);
		gui__popup_end(gui);
	} else {
		gui__hint_render(gui, gui->dropdown.id, gui->style.dropdown.hint);
	}

	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.dropdown);

	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.dropdown);
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

static
b32 gui__drag(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, mouse_button_t mb,
              u64 *pid, gui_drag_callback_t cb, void *udata)
{
	b32 changed = false;
	u64 id = gui_widget_id(gui, *x, *y);
	const b32 was_hot = (gui->hot_id == id);

	gui__btn_logic(gui, id, contains_mouse, mb);

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
			changed = true;
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
				changed = true;
			}
		}
	}

	*pid = id;
	return changed;
}

static
void gui__drag_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h, u64 id,
                      b32 moved, const gui_widget_style_t *widget_style)
{
	const gui__widget_render_state_t render_state
		= gui__widget_render_state(gui, id, moved, false, true);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, widget_style);
	widget_style->pen(gui, x, y, w, h, &style);
	gui__hint_render(gui, id, widget_style->hint);
}

void gui_drag_callback_default(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                               s32 offset_x, s32 offset_y, void *udata)
{
	*x = mouse_x + offset_x;
	*y = mouse_y + offset_y;
}

b32 gui_drag(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, mouse_button_t mb)
{
	return gui_dragx(gui, x, y, w, h, mb, gui_drag_callback_default, NULL);
}

b32 gui__drag_logic(gui_t *gui, u64 *id, s32 *x, s32 *y, s32 w, s32 h,
                    mouse_button_t mb, gui_drag_callback_t cb, void *udata)
{
	const b32 contains_mouse = gui__widget_contains_mouse(gui, *x, *y, w, h);
	return gui__drag(gui, x, y, contains_mouse, mb, id, cb, udata);
}

b32 gui_dragx(gui_t *gui, s32 *x, s32 *y, s32 w, s32 h, mouse_button_t mb,
              gui_drag_callback_t cb, void *udata)
{
	u64 id;
	const b32 ret = gui__drag_logic(gui, &id, x, y, w, h, mb, cb, udata);
	gui__drag_render(gui, *x, *y, w, h, id, ret, &gui->style.drag);
	return ret;
}

static
void gui__drag_horiz_callback(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                              s32 offset_x, s32 offset_y, void *udata)
{
	*x = mouse_x + offset_x;
}

b32 gui_drag_horiz(gui_t *gui, s32 *x, s32 y, s32 w, s32 h, mouse_button_t mb)
{
	return gui_dragx(gui, x, &y, w, h, mb, gui__drag_horiz_callback, NULL);
}

static
b32 gui__resize_horiz(gui_t *gui, s32 *x, s32 y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, *x, y, w, h);
	if (box2i_contains_point(box, gui->mouse_pos)) {
		SDL_SetCursor(gui->cursors[GUI__CURSOR_RESIZE_EW]);
		gui->use_default_cursor = false;
	}
	if (gui_drag_horiz(gui, x, y, w, h, MB_LEFT)) {
		gui->use_default_cursor = false;
		return true;
	}
	return false;
}

static
void gui__drag_vert_callback(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                             s32 offset_x, s32 offset_y, void *udata)
{
	*y = mouse_y + offset_y;
}

b32 gui_drag_vert(gui_t *gui, s32 x, s32 *y, s32 w, s32 h, mouse_button_t mb)
{
	return gui_dragx(gui, &x, y, w, h, mb, gui__drag_vert_callback, NULL);
}

static
b32 gui__resize_vert(gui_t *gui, s32 x, s32 *y, s32 w, s32 h)
{
	box2i box;
	box2i_from_xywh(&box, x, *y, w, h);
	if (box2i_contains_point(box, gui->mouse_pos)) {
		SDL_SetCursor(gui->cursors[GUI__CURSOR_RESIZE_NS]);
		gui->use_default_cursor = false;
	}
	if (gui_drag_vert(gui, x, y, w, h, MB_LEFT)) {
		gui->use_default_cursor = false;
		return true;
	}
	return false;
}

b32 gui_cdrag(gui_t *gui, s32 *x, s32 *y, s32 r, mouse_button_t mb)
{
	return gui_cdragx(gui, x, y, r, mb, gui_drag_callback_default, NULL);
}

b32 gui_cdragx(gui_t *gui, s32 *x, s32 *y, s32 r, mouse_button_t mb,
              gui_drag_callback_t cb, void *udata)
{
	const v2i pos = { *x, *y };
	u64 id;
	box2i box;
	box2i_from_center(&box, pos, (v2i){r, r});
	const b32 contains_mouse =    v2i_dist_sq(pos, gui->mouse_pos) < r * r
	                           && gui__box_half_visible(gui, box)
	                           && !gui->lock;
	const b32 ret = gui__drag(gui, x, y, contains_mouse, mb, &id, cb, udata);
	gui_style_push_pen(gui, drag.pen, gui_pen_circ);
	gui__drag_render(gui, *x - r, *y - r, 2 * r, 2 * r, id, ret, &gui->style.drag);
	gui_style_pop(gui);
	return ret;
}

b32 gui_menu_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                   const char *txt, s32 item_w, u32 num_items)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const s32 pw = item_w;
	const s32 ph = h * num_items;
	const s32 px = x;
	const s32 py = y - ph;
	gui__widget_render_state_t render_state;

	gui__popup_btn_logic(gui, id, contains_mouse, px, py, pw, ph);

	render_state = gui__widget_render_state(gui, id, false, false, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.dropdown);

	if (gui_widget_focused(gui, id)) {
		gui__popup_begin(gui, id, px, py, pw, ph);
		pgui_grid_begin(gui, &gui->grid_popup, px, py, pw, ph);
		pgui_col(gui, 0, num_items);
		return true;
	} else {
		gui__hint_render(gui, id, gui->style.dropdown.hint);
		return false;
	}
}

void gui_menu_end(gui_t *gui)
{
	/* TODO(rgriege): focus_id -> focus_stack to support nested menus */
	assert(gui->popup.id != 0 && gui->popup.id == gui->focus_ids[0]);
	pgui_grid_end(gui, &gui->grid_popup);
	gui__popup_end(gui);
}

b32 gui_color_picker_sv(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
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
	if (gui_drag(gui, &nx, &ny, w, h, MB_LEFT)) {
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

	return changed;
}

b32 gui_color_picker_h(gui_t *gui, s32 x, s32 y, s32 w, s32 h, colorf_t *c)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
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
	if (gui_drag(gui, &nx, &ny, w, h, MB_LEFT)) {
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

	return changed;
}

b32 gui_color_picker(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     s32 pw, s32 ph, colorf_t *c)
{
	if (!gui_box_visible(gui, x, y, w, h)) {
		gui__widget_bounds_extend(gui, x, y, w, h);
		return false;
	}

	const u64 id = gui_widget_id(gui, x, y);
	const b32 contains_mouse = gui__widget_contains_mouse(gui, x, y, w, h);
	const s32 px = x;
	const s32 py = y - ph;
	gui__widget_render_state_t render_state;
	gui_style_t *style = gui_style(gui);
	color_t color = colorf_to_color(*c);

	gui__popup_btn_logic(gui, id, contains_mouse, px, py, pw, ph);

	render_state = gui__widget_render_state(gui, id, false, false, contains_mouse);

	gui_style_push_current(gui, btn);
	style->btn.hot.outline_color = g_white;
	style->btn.inactive.bg_color = color;
	style->btn.hot.bg_color      = color;
	style->btn.active.bg_color   = color;
	gui__btn_render(gui, x, y, w, h, "", render_state, &gui->style.btn);
	gui_style_pop(gui);

	if (gui_widget_focused(gui, id)) {
		static const r32 cols_major[] = { 0, 80 };
		static const r32 rows[] = { 0, 2, 0.1f };
		static const r32 cols[] = { 0, 2, 0.1f };
		static const r32 cols_npt[] = { 20, 0 };
		static const r32 rows_npt[] = {
			20, 5, 20, 5, 20, 10, 20, 5, 20, 5, 20, 10, 20
		};
		const s32 gx = px + style->panel.padding;
		const s32 gy = py + style->panel.padding;
		const s32 gw = pw - style->panel.padding*2;
		const s32 gh = ph - style->panel.padding*2;
		const npt_flags_t flags = NPT_CLEAR_ON_FOCUS
		                        | NPT_COMPLETE_ON_TAB
		                        | NPT_COMPLETE_ON_CLICK_OUT;
		npt_filter_p filter = &g_gui_npt_filter_hex;
		b32 changed = false, changed_npt = false;
		u8 ch, cs, cv;

		gui__popup_begin(gui, id, px, py, pw, ph);
		pgui_grid_begin(gui, &gui->grid_popup, gx, gy, gw, gh);
		/* NOTE(rgriege): shrink required to pass entire rect through layer */
		gui_rect(gui, px+1, py, pw-1, ph-1, style->panel.bg_color,
		         style->panel.border_color);

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

		gui_style_push_u32(gui, txt.align, GUI_ALIGN_MIDRIGHT);

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

		pgui_grid_end(gui, &gui->grid_popup);
		gui__popup_end(gui);
		return changed;
	} else {
		return false;
	}
}

b32 gui_color_picker8(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                      s32 pw, s32 ph, color_t *c)
{
	const u64 id = gui_widget_id(gui, x, y);
	if (gui_widget_focused(gui, id)) {
		if (gui_color_picker(gui, x, y, w, h, pw, ph, &gui->color_picker8_color)) {
			*c = colorf_to_color(gui->color_picker8_color);
			return true;
		}
	} else {
		colorf_t cf = color_to_colorf(*c);
		gui_color_picker(gui, x, y, w, h, pw, ph, &cf);
		if (gui_widget_focused(gui, id))
			gui->color_picker8_color = cf;
	}
	return false;
}

static
void gui__scroll_area_usable_rect(const gui_t *gui,
                                  const gui_scroll_area_t *scroll_area,
                                  s32 *x, s32 *y, s32 *w, s32 *h)
{
	const s32 padding = gui->style.scroll_area.padding;
	const s32 scrollbar_track_width
		= max(gui->style.scroll_area.scrollbar_track_width - padding, 0);

	*x = scroll_area->pos.x;
	*y = scroll_area->pos.y;
	*w = scroll_area->dim.x;
	*h = scroll_area->dim.y;

	*x += padding;
	*y += padding;
	*w -= padding*2;
	*h -= padding*2;

	if (scrollbar_track_width > 0) {
		if (scroll_area->dim.x < scroll_area->last_max_dim.x) {
			*y += scrollbar_track_width;
			*h -= scrollbar_track_width;
		}
		if (scroll_area->dim.y < scroll_area->last_max_dim.y)
			*w -= scrollbar_track_width;
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
	gui__widget_bounds_push(gui, &scroll_area->widget_bounds);

	scroll_area->prev = gui->scroll_area;
	gui->scroll_area = scroll_area;
}

void gui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area)
{
	const v2i pos = scroll_area->pos;
	const v2i dim = scroll_area->dim;
	const v2i last_max_dim = scroll_area->last_max_dim;
	const b32 contains_mouse
		= gui__widget_contains_mouse(gui, pos.x, pos.y, dim.x, dim.y);
	const color_t bg_color = gui->style.scroll_area.bg_color;
	s32 rx, ry, rw, rh;

	assert(gui->scroll_area == scroll_area);

	gui->scroll_area = gui->scroll_area->prev;

	gui__widget_bounds_pop(gui, false);

	gui_mask_pop(gui);

	gui_rect(gui, pos.x, pos.y, dim.x, dim.y, bg_color, g_nocolor);

	gui__scroll_area_usable_rect(gui, scroll_area, &rx, &ry, &rw, &rh);

	if (rw < last_max_dim.x) {
		const s32 needed = last_max_dim.x - rw;
		scroll_area->scroll.x = -clamp(0, -scroll_area->scroll.x, needed);
		if (key_mod(gui, KBM_SHIFT) && !gui__mouse_covered(gui) && contains_mouse) {
			s32 scroll;
			mouse_scroll(gui, &scroll);
			scroll_area->scroll.x -= scroll * GUI_SCROLL_RATE;
			scroll_area->scroll.x = -clamp(0, -scroll_area->scroll.x, needed);
		}
		if (gui->style.scroll_area.scrollbar_track_width > 0) {
			const s32 scrollbar_track_width = gui->style.scroll_area.scrollbar_track_width;
			const s32 x = pos.x;
			const s32 y = pos.y;
			const s32 w = dim.x;
			const s32 h = scrollbar_track_width;
			const s32 handle_size = max(scrollbar_track_width, w * rw / last_max_dim.x);
			r32 slider_val = (r32)-scroll_area->scroll.x / (r32)needed;
			gui_style_push(gui, slider, gui->style.scroll_area.scrollbar);
			gui__slider(gui, x, y, w, h, &slider_val, handle_size, GUI__SLIDER_X);
			gui_style_pop(gui);
			scroll_area->scroll.x = (s32)(-slider_val * (r32)needed);
			scroll_area->scroll.x = -clamp(0, -scroll_area->scroll.x, needed);
		}
	} else {
		scroll_area->scroll.x = 0;
	}

	if (rh < last_max_dim.y) {
		const s32 needed = last_max_dim.y - rh;
		scroll_area->scroll.y = clamp(0, scroll_area->scroll.y, needed);
		if (!key_mod(gui, KBM_SHIFT) && !gui__mouse_covered(gui) && contains_mouse) {
			s32 scroll;
			mouse_scroll(gui, &scroll);
			scroll_area->scroll.y -= scroll * GUI_SCROLL_RATE;
			scroll_area->scroll.y = clamp(0, scroll_area->scroll.y, needed);
		}
		if (gui->style.scroll_area.scrollbar_track_width > 0) {
			const s32 scrollbar_track_width = gui->style.scroll_area.scrollbar_track_width;
			const s32 x = pos.x + dim.x - scrollbar_track_width;
			const s32 y = pos.y;
			const s32 w = scrollbar_track_width;
			const s32 h = dim.y;
			const s32 handle_size = max(scrollbar_track_width, h * rh / last_max_dim.y);
			r32 slider_val = 1.f - ((r32)scroll_area->scroll.y / (r32)needed);
			gui_style_push(gui, slider, gui->style.scroll_area.scrollbar);
			gui__slider(gui, x, y, w, h, &slider_val, handle_size, GUI__SLIDER_Y);
			gui_style_pop(gui);
			scroll_area->scroll.y = (s32)(-(slider_val - 1.f) * (r32)needed);
			scroll_area->scroll.y = clamp(0, scroll_area->scroll.y, needed);
		}
	} else {
		scroll_area->scroll.y = 0;
	}

	scroll_area->last_max_dim = box2i_get_extent(scroll_area->widget_bounds.bbox);

	if (contains_mouse && gui->mouse_covered_by_widget_id == ~0)
		gui->mouse_covered_by_widget_id = gui_widget_id(gui, pos.x, pos.y);
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

#define GUI__DEFAULT_PANEL_ID UINT_MAX
#define GUI__POPUP_PANEL_ID  (UINT_MAX-1)
#define GUI__MAX_PANEL_ID    (UINT_MAX-2)

u64 gui__widget_id(const gui_t *gui, u32 base_id, s32 x, s32 y)
{
	return (((u64)x) << 48) | (((u64)y) << 32) | base_id;
}

u64 gui_widget_id(const gui_t *gui, s32 x, s32 y)
{
	if (gui->popup.inside)
		return gui__widget_id(gui, GUI__POPUP_PANEL_ID, x, y);
	else if (gui->panel)
		return gui__widget_id(gui, gui->panel->id, x, y);
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

void gui_lock_if(gui_t *gui, b32 cond, u32 *restore_val)
{
	*restore_val = gui->lock;
	if (cond)
		gui_lock(gui);
}

void gui_lock_restore(gui_t *gui, u32 val)
{
	gui->lock = val;
}


/* Window */

static
void gui__window_drag_cb(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                         s32 offset_x, s32 offset_y, void *udata) {}

void gui_window_drag(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	gui_style_push(gui, drag, g_gui_style_invis.drag);
	if (gui_dragx(gui, &x, &y, w, h, MB_LEFT, gui__window_drag_cb, NULL)) {
		gui->dragging_window = true;
	  if (!v2i_equal(gui->mouse_pos, gui->mouse_pos_last)) {
			const v2i delta = v2i_sub(gui->mouse_pos, gui->mouse_pos_last);
			gui_move(gui, delta.x, -delta.y);
			gui->mouse_pos = gui->mouse_pos_last;
		}
	} else {
		gui->dragging_window = false;
	}
	gui_style_pop(gui);
}


/* Grid layout */

void pgui_grid_begin(gui_t *gui, gui_grid_t *grid, s32 x, s32 y, s32 w, s32 h)
{
	assert(grid != gui->grid);
	grid->prev = gui->grid;
	grid->depth = 0;
	grid->start.x = x;
	grid->start.y = y + h; /* widgets are drawn from high to low */
	grid->dim.x = w;
	grid->dim.y = h;
	grid->pos = grid->start;
	gui->grid = grid;
	box2i_from_xywh(&grid->widget_bounds.bbox, x, y, w, h);
	gui__widget_bounds_push(gui, &grid->widget_bounds);
}

void pgui_grid_end(gui_t *gui, gui_grid_t *grid)
{
	assert(grid);
	assert(gui->grid == grid);
	assert(gui->grid->depth == 0);
	gui->grid = gui->grid->prev;
	gui__widget_bounds_pop(gui, true);
}

static
void gui__strip_set_dim(gui_t *gui, gui_grid_strip_t *strip,
                        s32 dim, b32 vertical)
{
	gui_grid_t *grid = gui->grid;
	if (dim == GUI_GRID_FLEX) {
		if (grid->depth == 0) {
			assert(grid->dim.d[vertical] != GUI_GRID_FLEX);
			if (vertical)
				strip->dim.y = grid->pos.y - (grid->start.y - grid->dim.y);
			else
				strip->dim.x = grid->start.x + grid->dim.x - grid->pos.x;
		} else {
			v2i dim_;
			pgui_cell(gui, NULL, NULL, &dim_.x, &dim_.y);
			strip->dim.d[vertical] = dim_.d[vertical];
		}
	} else {
		strip->dim.d[vertical] = dim;
	}
}

static
void pgui__strip(gui_t *gui, b32 vertical, s32 minor_dim,
                 const r32 *cells, u32 num_cells)
{
	gui_grid_t *grid = gui->grid;
	s32 major_dim;
	b32 major_dim_inherited;
	s32 unspecified_cell_cnt;
	s32 cell_total_major_dim;
	gui_grid_strip_t *strip;

	assert(grid);
	assert(num_cells > 0);
	assert(grid->depth < GUI_GRID_MAX_DEPTH);
	assert(num_cells < GUI_GRID_MAX_CELLS);
	if (grid->depth != 0)
		assert(  (grid->strips[grid->depth-1].max_cell - grid->cells) + num_cells
		       < GUI_GRID_MAX_CELLS);
	/* TODO(rgriege): allow row within row & same for colums */
	if (grid->depth != 0)
		assert(grid->strips[grid->depth-1].vertical != vertical);

	strip = &grid->strips[grid->depth];
	strip->vertical     = vertical;
	strip->num_cells    = num_cells;
	strip->dim          = g_v2i_zero;
	strip->current_cell =   grid->depth > 0
	                      ? grid->strips[grid->depth-1].max_cell
	                      : grid->cells;
	strip->max_cell     = strip->current_cell + num_cells;


	/* compute strip dimensions */
	major_dim = 0;
	major_dim_inherited = false;
	for (u32 i = 0; i < num_cells; ++i) {
		if (cells[i] > 1.f)
			major_dim += cells[i];
		else
			major_dim_inherited = true;
	}
	if (major_dim_inherited) {
		gui__strip_set_dim(gui, strip, 0, vertical);
		assert(strip->dim.d[vertical] >= major_dim);
	} else {
		gui__strip_set_dim(gui, strip, major_dim, vertical);
	}
	gui__strip_set_dim(gui, strip, minor_dim, !vertical);

	/* compute cell dimensions */
	cell_total_major_dim = 0;
	unspecified_cell_cnt = 0;
	for (u32 i = 0; i < num_cells; ++i) {
		if (cells[i] > 1.f) {
			strip->current_cell[i] = cells[i];
			cell_total_major_dim += cells[i];
		} else if (cells[i] > 0.f) {
			const s32 dim = cells[i] * strip->dim.d[vertical];
			strip->current_cell[i] = dim;
			cell_total_major_dim += dim;
		} else {
			assert(cells[i] == 0.f);
			strip->current_cell[i] = 0.f;
			++unspecified_cell_cnt;
		}
	}
	if (unspecified_cell_cnt) {
		const s32 total = max((strip->dim.d[vertical] - cell_total_major_dim), 0);
		const s32 dim = total / unspecified_cell_cnt;
		const s32 leftover = total % unspecified_cell_cnt;
		s32 leftover_aggregate = 0, leftover_distributions = 1;
		for (u32 i = 0; i < num_cells; ++i) {
			if (strip->current_cell[i] != 0.f)
				continue;
			strip->current_cell[i] = dim;
			leftover_aggregate += leftover;
			if (leftover_aggregate >= leftover_distributions * unspecified_cell_cnt) {
				++strip->current_cell[i];
				++leftover_distributions;
			}
		}
		cell_total_major_dim += total;
	}

	++grid->depth;
}

static
void pgui__grid_advance(gui_grid_t *grid)
{
	gui_grid_strip_t *strip;

	assert(grid);
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

void pgui_row(gui_t *gui, s32 height, u32 num_cells)
{
	const r32 cells[GUI_GRID_MAX_CELLS] = { 0 };
	static_assert(GUI_GRID_FLEX == 0, invalid_initialization);
	assert(num_cells < GUI_GRID_MAX_CELLS);
	pgui_row_cells(gui, height, cells, num_cells);
}

void pgui_row_cells(gui_t *gui, s32 height, const r32 *cells, u32 num_cells)
{
	pgui__strip(gui, false, height, cells, num_cells);
}

void pgui_row_empty(gui_t *gui, s32 height)
{
	pgui_row(gui, height, 1);
	pgui_spacer_blank(gui);
}

void pgui_row_centered(gui_t *gui, s32 height, r32 width)
{
	const r32 cells[3] = { 0, width, 0 };
	static_assert(GUI_GRID_FLEX == 0, invalid_initialization);
	assert(countof(cells) < GUI_GRID_MAX_CELLS);
	pgui_row_cells(gui, height, B2PC(cells));
}

void pgui_col(gui_t *gui, s32 width, u32 num_cells)
{
	const r32 cells[GUI_GRID_MAX_CELLS] = { 0 };
	static_assert(GUI_GRID_FLEX == 0, invalid_initialization);
	assert(num_cells < GUI_GRID_MAX_CELLS);
	pgui_col_cells(gui, width, cells, num_cells);
}

void pgui_col_cells(gui_t *gui, s32 width, const r32 *cells, u32 num_cells)
{
	pgui__strip(gui, true, width, cells, num_cells);
}

void pgui_col_empty(gui_t *gui, s32 width)
{
	pgui_col(gui, width, 1);
	pgui_spacer_blank(gui);
}

void pgui_col_centered(gui_t *gui, s32 width, r32 height)
{
	const r32 cells[3] = { 0, height, 0 };
	static_assert(GUI_GRID_FLEX == 0, invalid_initialization);
	assert(countof(cells) < GUI_GRID_MAX_CELLS);
	pgui_row_cells(gui, width, B2PC(cells));
}

void pgui_cell(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	const gui_grid_t *grid = gui->grid;
	const gui_grid_strip_t *strip;
	v2i pos, dim;

	assert(grid);
	assert(grid->depth > 0);

	strip = &grid->strips[grid->depth - 1];

	pos = grid->pos;
	if (strip->vertical) {
		dim.x = strip->dim.x;
		dim.y = (s32)*strip->current_cell;
	} else {
		dim.x = (s32)*strip->current_cell;
		dim.y = strip->dim.y;
	}
	pos.y -= dim.y;

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
void pgui__cell_consume(gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	pgui_cell(gui, x, y, w, h);
	pgui__grid_advance(gui->grid);
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
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui_rect(gui, x, y, w, h, style->cell_bg_color, style->cell_border_color);
}

void pgui_spacer_blank(gui_t *gui)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui__widget_bounds_extend(gui, x, y, w, h);
}

void pgui_txt(gui_t *gui, const char *str)
{
	const gui_panel_style_t *style = &gui->style.panel;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui_rect(gui, x, y, w, h, style->cell_bg_color, style->cell_border_color);
	gui_txt_styled(gui, x, y, w, h, str, &gui->style.txt);
}

void pgui_img(gui_t *gui, const char *fname, img_scale_t scale)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui_img_boxed(gui, x, y, w, h, fname, scale);
}

s32 pgui_btn_txt(gui_t *gui, const char *lbl)
{
	btn_val_t result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_txt(gui, x, y, w, h, lbl);
	if (result == BTN_PRESS)
		gui->popup.close_at_end = true;
	return result;
}

s32 pgui_btn_img(gui_t *gui, const char *fname, img_scale_t scale)
{
	btn_val_t result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_img(gui, x, y, w, h, fname, scale);
	if (result == BTN_PRESS)
		gui->popup.close_at_end = true;
	return result;
}

s32 pgui_btn_pen(gui_t *gui, gui_pen_t pen)
{
	btn_val_t result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_pen(gui, x, y, w, h, pen);
	if (result == BTN_PRESS)
		gui->popup.close_at_end = true;
	return result;
}

b32 pgui_chk(gui_t *gui, const char *lbl, b32 *val)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_chk(gui, x, y, w, h, lbl, val);
}

s32 pgui_npt_txt(gui_t *gui, char *lbl, u32 n, const char *hint,
                 npt_flags_t flags)
{
	return pgui_npt_txt_ex(gui, lbl, n, hint, flags, &g_gui_npt_filter_print);
}

s32 pgui_npt_txt_ex(gui_t *gui, char *lbl, u32 n, const char *hint,
                    npt_flags_t flags, npt_filter_p filter)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_npt_txt_ex(gui, x, y, w, h, lbl, n, hint, flags, filter);
}

s32 pgui_npt_val(gui_t *gui, const char *txt, u32 n,
                 npt_flags_t flags, npt_filter_p filter)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_npt_val(gui, x, y, w, h, txt, n, flags, filter);
}

b32 pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_select(gui, x, y, w, h, lbl, val, opt);
}

void pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui_mselect(gui, x, y, w, h, txt, val, opt);
}

b32 pgui_dropdown_begin(gui_t *gui, u32 *val, u32 num_items)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_dropdown_begin(gui, x, y, w, h, val, num_items);
}

b32 pgui_dropdown_item(gui_t *gui, const char *txt)
{
	return gui_dropdown_item(gui, txt);
}

void pgui_dropdown_end(gui_t *gui)
{
	gui_dropdown_end(gui);
}

b32 pgui_slider_x(gui_t *gui, r32 *val)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_slider_x(gui, x, y, w, h, val);
}

b32 pgui_slider_y(gui_t *gui, r32 *val)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
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
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_menu_begin(gui, x, y, w, h, txt, item_w, num_items);
}

void pgui_menu_end(gui_t *gui)
{
	gui_menu_end(gui);
}

b32 pgui_color_picker_sv(gui_t *gui, colorf_t *color)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker_sv(gui, x, y, w, h, color);
}

b32 pgui_color_picker_h(gui_t *gui, colorf_t *color)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker_h(gui, x, y, w, h, color);
}

b32 pgui_color_picker(gui_t *gui, s32 pw, s32 ph, colorf_t *color)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker(gui, x, y, w, h, pw, ph, color);
}

b32 pgui_color_picker8(gui_t *gui, s32 pw, s32 ph, color_t *color)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_color_picker8(gui, x, y, w, h, pw, ph, color);
}

void pgui_scroll_area_begin(gui_t *gui, gui_scroll_area_t *scroll_area)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui_scroll_area_begin(gui, x, y, w, h, scroll_area);
}

void pgui_scroll_area_end(gui_t *gui, gui_scroll_area_t *scroll_area)
{
	gui_scroll_area_end(gui, scroll_area);
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

void gui__split_init(gui_split_t *split, gui_split_flags_t flags,
                     gui_split_t *parent)
{
	split->in_use     = true;
	split->flags      = flags;
	split->parent     = parent;
	split->sp1        = NULL;
	split->sp2        = NULL;
	split->sz         = 0;
	split->default_sz = 0;
	box2i_from_xywh(&split->box, 0, 0, 0, 0);
	split->panel      = NULL;
}

b32 gui_create_root_split(gui_t *gui, gui_split_t **root)
{
	gui_split_t *split = NULL;
	if (gui->root_split)
		assert(false);
	if (!gui__get_free_splits(gui, 1, &split))
		return false;
	gui__split_init(split, 0, NULL);
	gui->root_split = split;
	if (root)
		*root = split;
	return true;
}

b32 gui__split2(gui_t *gui, gui_split_t *split_, b32 vertical,
                gui_split_t **psp1, r32 sz, gui_split_t **psp2,
                gui_split_flags_t flags)
{
	gui_split_t *splits[3];
	gui_split_t *split, *sp1 = NULL, *sp2 = NULL;
	u32 i = 0;
	b32 success = false;

	if (!gui__get_free_splits(gui, !gui->root_split + 2, splits))
		goto out;

	if (!gui->root_split) {
		gui->root_split = splits[i++];
		gui__split_init(gui->root_split, 0, NULL);
	}

	split = split_ ? split_ : gui->root_split;

	sp1 = splits[i++];
	sp2 = splits[i++];

	split->vertical   = vertical;
	split->flags      = flags | GUI_SPLIT_DIVISIBLE;
	split->sp1        = sp1;
	split->sp2        = sp2;
	split->sz         = sz;
	split->default_sz = sz;
	gui__split_init(sp1, flags & GUI_SPLIT_DIVISIBLE, split);
	gui__split_init(sp2, flags & GUI_SPLIT_DIVISIBLE, split);
	success = true;

out:
	if (psp1)
		*psp1 = sp1;
	if (psp2)
		*psp2 = sp2;
	return success;
}

b32 gui_split2h(gui_t *gui, gui_split_t *split, gui_split_t **sp1, r32 sz,
                gui_split_t **sp2, gui_split_flags_t flags)
{
	return gui__split2(gui, split, false, sp1, sz, sp2, flags);
}

b32 gui_split2v(gui_t *gui, gui_split_t *split, gui_split_t **sp1, r32 sz,
                gui_split_t **sp2, gui_split_flags_t flags)
{
	return gui__split2(gui, split, true, sp1, sz, sp2, flags);
}

static
s32 gui__split_dim(r32 sz, r32 w)
{
	if (sz > 1.f)
		return sz;
	else if (sz < -1.f)
		return w + sz;
	else if (sz > 0.f)
		return w * sz;
	else if (sz < 0.f)
		return w * (1.f + sz);
	else
		return w * 0.5f;
}

static
b32 gui__split_is_leaf(const gui_split_t *split)
{
	return !split->sp1 && !split->sp2;
}

static
b32 gui__find_split_from(gui_split_t *split, v2i point, gui_split_t **splitp)
{
	if (!gui__split_is_leaf(split)) {
		return gui__find_split_from(split->sp1, point, splitp)
		    || gui__find_split_from(split->sp2, point, splitp);
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

enum gui__split_division_type
{
	GUI__SPLIT_DIVISION_NONE,
	GUI__SPLIT_DIVISION_CENTER,
	GUI__SPLIT_DIVISION_LEFT,
	GUI__SPLIT_DIVISION_RIGHT,
	GUI__SPLIT_DIVISION_TOP,
	GUI__SPLIT_DIVISION_BOTTOM,
};

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
	box2i_to_xywh(split->box, &x, &y, &w, &h);

	/* checking use_default_cursor ensures only 1 resize happens per frame */
	if (   (split->flags & GUI_SPLIT_RESIZABLE)
	    && gui->use_default_cursor
	    && !gui__mouse_covered(gui)) {
		gui_style_push(gui, drag.inactive.bg_color, g_nocolor);
		gui_style_push(gui, drag.hot.bg_color, g_nocolor);
		gui_style_push(gui, drag.active.bg_color, g_nocolor);
		if (split->vertical) {
			const s32 orig_drag_x =   x - GUI_SPLIT_RESIZE_BORDER / 2
			                        + gui__split_dim(split->sz, w);
			s32 drag_x = orig_drag_x;
			if (gui__resize_horiz(gui, &drag_x, y, GUI_SPLIT_RESIZE_BORDER, h)) {
				if (fabsf(split->sz) > 1.f)
					split->sz += drag_x - orig_drag_x;
				else
					split->sz = (r32)(drag_x + GUI_SPLIT_RESIZE_BORDER / 2 - x) / w;
				split->default_sz = split->sz;
				*changed = true;
			}
		} else {
			const s32 orig_drag_y =   y + h - GUI_SPLIT_RESIZE_BORDER / 2
			                        - gui__split_dim(split->sz, h);
			s32 drag_y = orig_drag_y;
			if (gui__resize_vert(gui, x, &drag_y, w, GUI_SPLIT_RESIZE_BORDER)) {
				if (fabsf(split->sz) > 1.f)
					split->sz += orig_drag_y - drag_y;
				else
					split->sz = (r32)(y + h - drag_y - GUI_SPLIT_RESIZE_BORDER / 2) / h;
				split->default_sz = split->sz;
				*changed = true;
			}
		}
		gui_style_pop(gui);
		gui_style_pop(gui);
		gui_style_pop(gui);
	}

	if (split->vertical) {
		const s32 xm = split->sp1->box.max.x;
		gui_line_styled(gui, xm, y, xm, y + h, &gui->style.split);
	} else {
		const s32 ym = split->sp1->box.min.y;
		gui_line_styled(gui, x, ym, x + w, ym, &gui->style.split);
	}

	if (split->sp1)
		gui__split_render(gui, split->sp1, changed);
	if (split->sp2)
		gui__split_render(gui, split->sp2, changed);
}

static
void gui__split_render_leaf(gui_t *gui, gui_split_t *split)
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
	gui__split_render(gui, gui->root_split, changed);
}

void gui_splits_render(gui_t *gui)
{
	b32 changed;
	gui__splits_render(gui, &changed);
}

static
void gui_splits_compute_from(gui_t *gui, gui_split_t *split)
{
	s32 x, y, w, h, sz1;

	assert(split);
	assert(!gui__split_is_leaf(split));

	box2i_to_xywh(split->box, &x, &y, &w, &h);

	sz1 = gui__split_dim(split->sz, (split->vertical) ? w : h);
	split->sp1->box = split->box;
	split->sp2->box = split->box;

	if (split->vertical) {
		const s32 xm = split->box.min.x + sz1;
		split->sp1->box.max.x = xm;
		split->sp2->box.min.x = xm;
	} else {
		const s32 ym = split->box.max.y - sz1;
		split->sp1->box.min.y = ym;
		split->sp2->box.max.y = ym;
	}

	if (!gui__split_is_leaf(split->sp1))
		gui_splits_compute_from(gui, split->sp1);
	if (!gui__split_is_leaf(split->sp2))
		gui_splits_compute_from(gui, split->sp2);
}

void gui_splits_compute(gui_t *gui)
{
	const v2i halfdim = v2i_scale_inv(gui->window_dim, 2);
	assert(gui->root_split);
	box2i_from_center(&gui->root_split->box, halfdim, halfdim);
	if (!gui__split_is_leaf(gui->root_split))
		gui_splits_compute_from(gui, gui->root_split);
}

void pgui__panel_init(gui_t *gui, gui_panel_t *panel, u32 id,
                      s32 x, s32 y, s32 w, s32 h,
                      const char *title, gui_panel_flags_t flags)
{
	assert(id <= GUI__MAX_PANEL_ID);
	assert(!(flags & GUI_PANEL_TITLEBAR) || title);

	panel->x = x;
	panel->y = y;
	panel->width = w;
	panel->height = h;
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
}

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, u32 id,
                     s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_t flags)
{
	pgui__panel_init(gui, panel, id, x, y, w, h, title, flags);
	pgui_panel_to_front(gui, panel);
}

void pgui_panel_init_centered(gui_t *gui, gui_panel_t *panel, u32 id,
                              s32 w, s32 h,
                              const char *title, gui_panel_flags_t flags)
{
	const s32 x = (gui->window_dim.x - w)/2;
	const s32 y = (gui->window_dim.y - h)/2;
	pgui__panel_init(gui, panel, id, x, y, w, h, title, flags);
}

void pgui_panel_init_in_split(gui_t *gui, gui_panel_t *panel, u32 id,
                              gui_split_t *split,
                              const char *title, gui_panel_flags_t flags)
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
void pgui__validate_tab_flags(gui_panel_flags_t flags0, gui_panel_flags_t flags1)
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
		    && split->parent
		    && (split->parent->flags & GUI_SPLIT_TEMPORARY)) {
			gui_split_t *parent = split->parent;
			if (parent->sp1 == split) {
				if (parent->sp2->panel) {
					parent->panel = parent->sp2->panel;
					parent->sp2->panel->split = parent;
				}
			} else {
				if (parent->sp1->panel) {
					parent->panel = parent->sp1->panel;
					parent->sp1->panel->split = parent;
				}
			}
			parent->sp1->in_use = false;
			parent->sp2->in_use = false;
			parent->sp1 = NULL;
			parent->sp2 = NULL;
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
		y = panel->y + panel->height - GUI_PANEL_TITLEBAR_HEIGHT;
		height = GUI_PANEL_TITLEBAR_HEIGHT;
	} else {
		y = panel->y;
		height = panel->height;
	}

	box2i_from_xywh(&box, panel->x, y, panel->width, height);
	gui__mask_box(gui, &box);
	return box2i_contains_point(box, gui->mouse_pos);
}

static
void pgui__panel_resize(gui_t *gui, gui_panel_t *panel)
{
	v2i resize;
	s32 resize_delta;

	if (   (panel->flags & GUI_PANEL_RESIZABLE)
	    && !panel->split
	    && !panel->collapsed
	    && gui->use_default_cursor) {
		gui_style_push(gui, drag, g_gui_style_invis.drag);

		resize.x = panel->x - GUI_PANEL_RESIZE_BORDER;
		if (gui__resize_horiz(gui, &resize.x, panel->y, GUI_PANEL_RESIZE_BORDER,
		                      panel->height)) {
			resize_delta = panel->x - GUI_PANEL_RESIZE_BORDER - resize.x;
			if (panel->width + resize_delta < GUI_PANEL_MIN_DIM) {
				const r32 dx = panel->width - GUI_PANEL_MIN_DIM;
				panel->width = GUI_PANEL_MIN_DIM;
				panel->x += dx;
				gui->active_id = gui_widget_id(gui, panel->x - GUI_PANEL_RESIZE_BORDER,
				                               panel->y);
			} else {
				panel->width += resize_delta;
				panel->x -= resize_delta;
			}
		}

		resize.x = panel->x + panel->width;
		if (gui__resize_horiz(gui, &resize.x, panel->y, GUI_PANEL_RESIZE_BORDER,
		                      panel->height)) {
			panel->width += resize.x - (panel->x + panel->width);
			if (panel->width < GUI_PANEL_MIN_DIM) {
				panel->width = GUI_PANEL_MIN_DIM;
				gui->active_id = gui_widget_id(gui, panel->x + panel->width, panel->y);
			}
		}

		resize.y = panel->y - GUI_PANEL_RESIZE_BORDER;
		if (gui__resize_vert(gui, panel->x, &resize.y, panel->width,
		                     GUI_PANEL_RESIZE_BORDER)) {
			resize_delta = panel->y - GUI_PANEL_RESIZE_BORDER - resize.y;
			if (panel->height + resize_delta < GUI_PANEL_MIN_DIM) {
				const r32 dy = panel->height - GUI_PANEL_MIN_DIM;
				panel->height = GUI_PANEL_MIN_DIM;
				panel->y += dy;
				gui->active_id = gui_widget_id(gui, panel->x,
				                               panel->y - GUI_PANEL_RESIZE_BORDER);
			} else {
				panel->height += resize_delta;
				panel->y -= resize_delta;
			}
		}

		resize.y = panel->y + panel->height;
		if (gui__resize_vert(gui, panel->x, &resize.y, panel->width,
		                     GUI_PANEL_RESIZE_BORDER)) {
			panel->height += resize.y - (panel->y + panel->height);
			if (panel->height < GUI_PANEL_MIN_DIM) {
				panel->height = GUI_PANEL_MIN_DIM;
				gui->active_id = gui_widget_id(gui, panel->x, panel->y + panel->height);
			}
		}

		gui_style_pop(gui);
	}
}

static
void pgui__panel_collapse_toggle(gui_panel_t *panel)
{
	panel->collapsed = !panel->collapsed;
	if (panel->split && panel->split->parent) {
		/* TODO(rgriege): allow collapsing second child in split */
		gui_split_t *parent = panel->split->parent;
		assert(panel->split == parent->sp1);
		parent->sz = panel->collapsed
		           ? GUI_PANEL_TITLEBAR_HEIGHT : parent->default_sz;
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
	const s32 dim = GUI_PANEL_TITLEBAR_HEIGHT;
	s32 y;
	u64 drag_id = ~0;
	s32 rw, rx;
	s32 tab_count;
	gui_panel_t *panel_to_remove_from_tabs = NULL;

	y = panel->y + panel->height - dim;

	if (panel->flags & GUI_PANEL_DRAGGABLE) {
		*dragging = gui__drag_logic(gui, &drag_id, &panel->x, &y, dim, dim,
		                            MB_LEFT, gui_drag_callback_default, NULL);
		if (*dragging) {
			panel->y = y - panel->height + dim;
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

	gui_rect(gui, panel->x, y, panel->width,
	         dim, gui->style.panel.titlebar.bg_color,
	         gui->style.panel.titlebar.outline_color);

	gui__drag_render(gui, panel->x, y, dim, dim, drag_id,
	                 *dragging, &gui->style.panel.drag);

	rw = 0;
	if (panel->flags & GUI_PANEL_COLLAPSABLE)
		rw += dim;
	if (panel->flags & GUI_PANEL_CLOSABLE)
		rw += dim;

	tab_count = 0;
	for (gui_panel_t *p = panel; p; p = p->next)
		++tab_count;
	if (tab_count > 1) {
		s32 tab_dim, tab_idx;
		u32 selected_tab_idx;
		tab_dim = (panel->width - dim - rw) / tab_count;
		tab_dim = min(tab_dim, 100);
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
					p->y   = drag_y - p->height + dim;
					gui->active_id = gui__widget_id(gui, p->id, p->x, drag_y);
					panel_to_remove_from_tabs = p;
				}
			}
			++tab_idx;
		}
		gui_style_pop(gui);
	} else {
		gui_txt_styled(gui, panel->x + dim, y, panel->width, dim,
		               panel->title, &gui->style.panel.titlebar.text);
	}

	rx = panel->x + panel->width - rw;
	if (panel->flags & GUI_PANEL_COLLAPSABLE) {
		if (panel->collapsed)
			gui_style_push(gui, btn, gui->style.panel.restore);
		else
			gui_style_push(gui, btn, gui->style.panel.collapse);
		if (gui_btn_pen(gui, rx, y, dim, dim, gui->style.btn.pen) == BTN_PRESS)
			pgui__panel_collapse_toggle(panel);
		gui_style_pop(gui);
		rx += dim;
	}

	if (panel->flags & GUI_PANEL_CLOSABLE) {
		gui_style_push(gui, btn, gui->style.panel.close);
		if (gui_btn_pen(gui, rx, y, dim, dim, gui->style.btn.pen) == BTN_PRESS)
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
	gui_split_t *split, *sp1, *sp2;

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
			gui_split2v(gui, split, &sp1, 0.5f, &sp2, split->flags);
			sp1->panel = panel;
			panel->split = sp1;
			sp2->panel = split->panel;
			if (split->panel)
				split->panel->split = sp2;
			split->panel = NULL;
			gui_splits_compute_from(gui, split);
		}
	break;
	case GUI__SPLIT_DIVISION_RIGHT:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2v(gui, split, &sp1, 0.5f, &sp2, split->flags);
			sp1->panel = split->panel;
			if (split->panel)
				split->panel->split = sp1;
			sp2->panel = panel;
			panel->split = sp2;
			split->panel = NULL;
			gui_splits_compute_from(gui, split);
		}
	break;
	case GUI__SPLIT_DIVISION_TOP:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2h(gui, split, &sp1, 0.5f, &sp2, split->flags);
			sp1->panel = panel;
			panel->split = sp1;
			sp2->panel = split->panel;
			if (split->panel)
				split->panel->split = sp2;
			split->panel = NULL;
			gui_splits_compute_from(gui, split);
		}
	break;
	case GUI__SPLIT_DIVISION_BOTTOM:
		if (split->flags & GUI_SPLIT_DIVISIBLE) {
			gui_split2h(gui, split, &sp1, 0.5f, &sp2, split->flags);
			sp1->panel = split->panel;
			if (split->panel)
				split->panel->split = sp1;
			sp2->panel = panel;
			panel->split = sp2;
			split->panel = NULL;
			gui_splits_compute_from(gui, split);
		}
	break;
	}
}

static
s32 pgui__panel_body_height(const gui_panel_t *panel)
{
	return (panel->flags & GUI_PANEL_TITLEBAR)
	     ? panel->height - GUI_PANEL_TITLEBAR_HEIGHT
	     : panel->height;
}

static
void pgui__style_push_panel_scroll_area(gui_t *gui)
{
	const s32 scrollbar_track_width = gui->style.panel.padding/2;
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
	b32 dragging = false;

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

		panel->x           = first->x;
		panel->y           = first->y;
		panel->width       = first->width;
		panel->height      = first->height;
		panel->split       = first->split;
		panel->closed      = first->closed;
		panel->collapsed   = first->collapsed;

		gui->panel = panel;
	} else {
		if (panel->closed)
			return false;

		gui->panel = panel;

		if (panel->split)
			box2i_to_xywh(panel->split->box, &panel->x, &panel->y,
			              &panel->width, &panel->height);

		pgui__panel_resize(gui, panel);
		if (panel->flags & GUI_PANEL_TITLEBAR)
			pgui__panel_titlebar(gui, panel, &dragging);

		if (dragging) {
			if (panel->flags & GUI_PANEL_DOCKABLE)
				gui->is_dragging_dockable_panel = true;
			if (gui->active_id == 0)
				pgui__panel_stop_dragging(gui, panel);
		}

		if (panel->collapsed) {
			/* background outline display */
			const s32 h = GUI_PANEL_TITLEBAR_HEIGHT;
			gui_rect(gui, panel->x, panel->y + panel->height - h, panel->width, h,
			         g_nocolor, gui->style.panel.border_color);
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

	if (!pgui_panel_content_visible(gui, panel)) {
		gui->panel = NULL;
		return false;
	}

	/* background outline display */
	gui_rect(gui, panel->x, panel->y, panel->width, panel->height,
	         g_nocolor, gui->style.panel.border_color);

	{
		const s32 body_height = pgui__panel_body_height(panel);
		box2i box;
		v2i pos, dim;
		box2i_from_xywh(&box, panel->x, panel->y, panel->width, body_height);
		pos = box.min;
		dim = box2i_get_extent(box);

		pgui__style_push_panel_scroll_area(gui);
		gui_scroll_area_begin(gui, pos.x, pos.y, dim.x, dim.y, &panel->scroll_area);
		pgui__style_pop_panel_scroll_area(gui);
	}
	return true;
}

void pgui_panel_collapse(gui_panel_t *panel)
{
	assert(panel->flags & GUI_PANEL_COLLAPSABLE);
	if (!panel->collapsed)
		pgui__panel_collapse_toggle(panel);
}

void pgui_panel_restore(gui_panel_t *panel)
{
	assert(panel->flags & GUI_PANEL_COLLAPSABLE);
	if (panel->collapsed)
		pgui__panel_collapse_toggle(panel);
}

void pgui_panel_open(gui_t *gui, gui_panel_t *panel)
{
	assert(panel->closed);
	panel->closed = false;
	pgui_panel_to_front(gui, panel);
}

void pgui_panel_close(gui_t *gui, gui_panel_t *panel)
{
	assert(panel->flags & GUI_PANEL_CLOSABLE);
	assert(!panel->closed);
	panel->closed = true;
	pgui_panel_remove_tab(gui, panel);
}

void pgui_panel_finish(gui_t *gui, gui_panel_t *panel)
{
	const b32 contains_mouse = gui__current_panel_contains_mouse(gui);

	assert(gui->panel == panel);

	if (panel->collapsed || panel->closed)
		goto out;

	pgui__style_push_panel_scroll_area(gui);
	gui_scroll_area_end(gui, &panel->scroll_area);
	pgui__style_pop_panel_scroll_area(gui);

	/* NOTE(rgriege): would be great to avoid the additional layer here,
	 * but otherwise the next widgets are on top of the panel bg & scrollbars */
	gui__layer_new(gui);

out:
	if (contains_mouse && gui->mouse_covered_by_widget_id == ~0)
		gui->mouse_covered_by_widget_id = gui_widget_id(gui, ~0, ~0);

	gui->panel = NULL;
}

b32 pgui_panel_content_visible(const gui_t *gui, const gui_panel_t *panel)
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

	pgui__style_push_panel_scroll_area(gui);
	pgui_scroll_area_grid_begin(gui, &gui->grid_panel, flex);
	pgui__style_pop_panel_scroll_area(gui);
}

void pgui_panel_grid_end(gui_t *gui)
{
	assert(gui->panel);
	assert(gui->grid);
	assert(gui->grid == &gui->grid_panel);
	pgui_grid_end(gui, gui->grid);
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
		error("style stack limit exceeded");
	}
}

void gui_style_push_current_(gui_t *gui, size_t offset, size_t size)
{
	const gui__style_stack_item_t item = { .offset = offset, .size = size };
	if (gui__style_stack_can_push_item(gui, item))
		gui__style_stack_push_item(gui, item);
	else
		error("style stack limit exceeded");
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
		gui->style_stack_sz = 0;
		error("style stack empty");
	}
}

#define GUI_STYLE_STACK_PUSH_LITERAL(name, type) \
void gui_style_push_##name##_(gui_t *gui, size_t offset, type val) \
{ \
	gui_style_push_(gui, &val, offset, sizeof(val)); \
}

GUI_STYLE_STACK_PUSH_LITERAL(color, color_t);
GUI_STYLE_STACK_PUSH_LITERAL(b32, b32);
GUI_STYLE_STACK_PUSH_LITERAL(u32, u32);
GUI_STYLE_STACK_PUSH_LITERAL(s32, s32);
GUI_STYLE_STACK_PUSH_LITERAL(r32, r32);
GUI_STYLE_STACK_PUSH_LITERAL(ptr, const void*);

void gui_style_push_pen_(gui_t *gui, size_t offset, gui_pen_t pen)
{
	const size_t size =   offsetof(gui_style_t, btn.inactive)
	                    - offsetof(gui_style_t, btn.pen);
	void(*fn)(void) = (void(*)(void))pen;
	assert(pen);
	gui_style_push_(gui, &fn, offset, size);
}



/* Shaders */

#ifdef __EMSCRIPTEN__
static const char *g_vertex_shader =
	"uniform vec2 window_halfdim;\n"
	"attribute vec2 position;\n"
	"attribute vec4 color;\n"
	"attribute vec2 tex_coord;\n"
	"varying vec2 TexCoord;\n"
	"varying vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  vec2 p = (position - window_halfdim) / window_halfdim;\n"
	"  gl_Position = vec4(p.xy, 0.0, 1.0);\n"
	"  TexCoord = tex_coord;\n"
	"  Color = color;\n"
	"}";

static const char *g_fragment_shader =
	"precision mediump float;\n"
	"uniform sampler2D tex;\n"
	"varying vec2 TexCoord;\n"
	"varying vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  vec2 TexCoord_flipped = vec2(TexCoord.x, 1.0 - TexCoord.y);\n"
	"  gl_FragColor = texture2D(tex, TexCoord_flipped) * Color;\n"
	"}";
#else
static const char *g_vertex_shader =
	"#version 330\n"
	"layout(location = 0) in vec2 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"layout(location = 2) in vec2 tex_coord;\n"
	"uniform vec2 window_halfdim;\n"
	"out vec2 TexCoord;\n"
	"out vec4 Color;\n"
	"\n"
	"void main() {\n"
	"  vec2 p = (position - window_halfdim) / window_halfdim;\n"
	"  gl_Position = vec4(p.xy, 0.0, 1.0);\n"
	"  TexCoord = tex_coord;\n"
	"  Color = color;\n"
	"}";

static const char *g_fragment_shader =
	"#version 330\n"
	"in vec2 TexCoord;\n"
	"in vec4 Color;\n"
	"uniform sampler2D tex;\n"
	"out vec4 FragColor;\n"
	"\n"
	"void main() {\n"
	"  vec2 TexCoord_flipped = vec2(TexCoord.x, 1.0 - TexCoord.y);\n"
	"  FragColor = texture(tex, TexCoord_flipped) * Color;\n"
	"}";
#endif

#undef GUI_IMPLEMENTATION
#endif // GUI_IMPLEMENTATION
