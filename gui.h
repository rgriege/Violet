#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core.h"
#include "violet/key.h"

/* Color */

typedef struct color_t
{
	u8 r, g, b, a;
} color_t;

#define gi_black        { .r=0,    .g=0,    .b=0,    .a=0xff }
#define gi_grey77       { .r=0x4d, .g=0x4d, .b=0x4d, .a=0xff }
#define gi_grey128      { .r=0x80, .g=0x80, .b=0x80, .a=0xff }
#define gi_white        { .r=0xff, .g=0xff, .b=0xff, .a=0xff }

#define gi_blue         { .r=0,    .g=0,    .b=0xff, .a=0xff }
#define gi_fuchsia      { .r=0xff, .g=0,    .b=0xff, .a=0xff }
#define gi_green        { .r=0,    .g=0xff, .b=0,    .a=0xff }
#define gi_lightblue    { .r=0x3f, .g=0xa8, .b=0xf5, .a=0xff }
#define gi_medblue      { .r=0x3f, .g=0x68, .b=0xf5, .a=0xff }
#define gi_orange       { .r=0xff, .g=0x92, .b=0x1e, .a=0xff }
#define gi_red          { .r=0xff, .g=0,    .b=0,    .a=0xff }
#define gi_yellow       { .r=0xff, .g=0xff, .b=0,    .a=0xff }

#define gi_nocolor      { .r=0,    .g=0,    .b=0,    .a=0 }


#define g_black     (color_t)gi_black
#define g_grey77    (color_t)gi_grey77
#define g_grey128   (color_t)gi_grey128
#define g_white     (color_t)gi_white

#define g_blue      (color_t)gi_blue
#define g_fuchsia   (color_t)gi_fuchsia
#define g_green     (color_t)gi_green
#define g_lightblue (color_t)gi_lightblue
#define g_medblue   (color_t)gi_medblue
#define g_orange    (color_t)gi_orange
#define g_red       (color_t)gi_red
#define g_yellow    (color_t)gi_yellow

#define g_nocolor   (color_t)gi_nocolor

void    color_as_float_array(r32 *f, color_t c);
color_t color_from_float(const r32 *f);
void    color_to_hex(color_t c, char *hex, u32 n);
color_t color_from_hex(const char *hex);
b32     color_equal(color_t lhs, color_t rhs);
color_t color_blend(color_t src, color_t dst);


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

typedef struct texture_t
{
	u32 handle;
	u32 width;
	u32 height;
} texture_t;

b32  texture_load(texture_t *tex, const char *filename);
void texture_init(texture_t *tex, u32 w, u32 h, u32 fmt, const void *data);
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

typedef struct font_t
{
	const char *filename;
	u32 sz;
	s32 ascent, descent, line_gap, newline_dist;
	void *char_info;
	texture_t texture;
} font_t;

b32  font_load(font_t *f, const char *filename, u32 sz);
void font_destroy(font_t *f);


/* Utility */

u32 gui_triangulate_out_sz(u32 n);
b32 gui_triangulate(const v2f *v, u32 n, v2f **triangles);


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
void   gui_minimize(gui_t *gui);
void   gui_maximize(gui_t *gui);
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
b32  mouse_pressed(const gui_t *gui, u32 mask);
b32  mouse_pressed_bg(const gui_t *gui, u32 mask);
b32  mouse_down(const gui_t *gui, u32 mask);
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
void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t stroke);
void gui_poly(gui_t *gui, const v2i *v, u32 n, color_t fill, color_t stroke);
void gui_polyf(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t stroke);
void gui_img(gui_t *gui, s32 x, s32 y, const char *img);
void gui_img_ex(gui_t *gui, s32 x, s32 y, const img_t *img, r32 sx, r32 sy,
                r32 opacity);
void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                   img_scale_t scale);
void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt, color_t c,
             gui_align_t align);
void gui_txt_dim(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt,
                 gui_align_t align, s32 *px, s32 *py, s32 *pw, s32 *ph);
s32  gui_txt_width(gui_t *gui, const char *txt, u32 sz);
void gui_mask(gui_t *gui, s32 x, s32 y, s32 w, s32 h);
void gui_unmask(gui_t *gui);


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


/* Widgets */

typedef enum npt_flags_t
{
	NPT_PASSWORD            = 1 << 0,
	NPT_CLEAR_ON_FOCUS      = 1 << 1,
	NPT_COMPLETE_ON_DEFOCUS = 1 << 2,
} npt_flags_t;

typedef enum btn_val_t
{
	BTN_NONE,
	BTN_PRESS,
	BTN_HOLD,
} btn_val_t;

const b32 g_gui_npt_chars_print[128];
const b32 g_gui_npt_chars_numeric[128];

b32       gui_npt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                  const char *hint, npt_flags_t flags);
b32       gui_npt_chars(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                        const char *hint, npt_flags_t flags, const b32 chars[128]);
btn_val_t gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt);
btn_val_t gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                      img_scale_t scale);
btn_val_t gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen);
b32       gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt,
                  b32 *val);
b32       gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen,
                      b32 *val);
b32       gui_slider_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
b32       gui_slider_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
b32       gui_range_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                      r32 min, r32 max);
b32       gui_range_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                      r32 min, r32 max);
b32       gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, u32 *val, u32 opt);
void      gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                      const char *txt, u32 *val, u32 opt);
void      gui_dropdown_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                             u32 *val, u32 num_items);
b32       gui_dropdown_item(gui_t *gui, const char *txt);
typedef void(*gui_drag_callback_t)(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                                   s32 offset_x, s32 offset_y, void *udata);
b32       gui_drag(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb);
b32       gui_drag_horiz(gui_t *gui, s32 *x, s32 y, u32 w, u32 h, mouse_button_t mb);
b32       gui_drag_vert(gui_t *gui, s32 x, s32 *y, u32 w, u32 h, mouse_button_t mb);
b32       gui_dragx(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb,
                    gui_drag_callback_t cb, void *udata);
b32       gui_cdrag(gui_t *gui, s32 *x, s32 *y, u32 r, mouse_button_t mb);
b32       gui_cdragx(gui_t *gui, s32 *x, s32 *y, u32 r, mouse_button_t mb,
                     gui_drag_callback_t cb, void *udata);

u64       gui_widget_id(const gui_t *gui, s32 x, s32 y);
void      gui_widget_focus_next(gui_t *gui);
b32       gui_widget_active(const gui_t *gui, u64 id);
b32       gui_widget_focused(const gui_t *gui, u64 id);
b32       gui_any_widget_hot(const gui_t *gui);
b32       gui_any_widget_active(const gui_t *gui);
b32       gui_any_widget_has_focus(const gui_t *gui);
/* NOTE: this will not interrupt currently active widgets */
void      gui_lock(gui_t *gui);
void      gui_unlock(gui_t *gui);
/* NOTE: I usually hate 'conditional' methods, but this cleans up usage code */
void      gui_lock_if(gui_t *gui, b32 cond, u32 *restore_val);
void      gui_lock_restore(gui_t *gui, u32 val);

/* Splits */

#ifndef GUI_SPLIT_RESIZE_BORDER
#define GUI_SPLIT_RESIZE_BORDER 4
#endif

typedef enum gui_split_flags
{
	GUI_SPLIT_RESIZABLE = 0x1,
	GUI_SPLIT_FULL      = 0x1,
} gui_split_flags_t;

typedef struct gui_split
{
	b32 vert, leaf;
	gui_split_flags_t flags;
	struct gui_split *parent, *sp1, *sp2;
	r32 sz, default_sz;
	box2i box;
} gui_split_t;

void gui_split_init(gui_t *gui, gui_split_t *split,
                    gui_split_t *sp1, r32 sz, gui_split_t *sp2,
                    gui_split_flags_t flags);
void gui_vsplit_init(gui_t *gui, gui_split_t *split,
                     gui_split_t *sp1, r32 sz, gui_split_t *sp2,
                     gui_split_flags_t flags);
void gui_compute_split_boxes(gui_t *gui);


/* Panels */

#define GUI_GRID_REMAINING 0
#define GUI_GRID_MINOR_DIM_INHERITED 0
#define GUI_COL_WIDTH_INHERITED GUI_GRID_MINOR_DIM_INHERITED
#define GUI_ROW_HEIGHT_INHERITED GUI_GRID_MINOR_DIM_INHERITED

#ifndef GUI_PANEL_MAX_GRID_DEPTH
#define GUI_PANEL_MAX_GRID_DEPTH 8
#endif

#ifndef GUI_PANEL_MAX_GRID_CELLS
#define GUI_PANEL_MAX_GRID_CELLS 64
#endif

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
	GUI_PANEL_FULL        = 0x3f,
} gui_panel_flags_t;

typedef struct gui_panel_grid_strip
{
	b32 vertical;
	u32 *current_cell;
	u32 *max_cell;
	u32 num_cells;
	v2i dim;
} gui_panel_grid_strip_t;

typedef struct gui_panel
{
	s32 x, y, width, height;
	const char *title;
	gui_panel_flags_t flags;
	u32 id;
	intptr userdata;
	struct
	{
		u32 cells[GUI_PANEL_MAX_GRID_CELLS];
		gui_panel_grid_strip_t strips[GUI_PANEL_MAX_GRID_DEPTH];
		u32 depth;
		v2i start, pos;
	} grid;
	v2i scroll;
	v2i scroll_rate;
	v2i padding;
	v2i required_dim;
	s32 body_height;
	s32 pri;
	struct gui_panel *parent, *prev, *next;
	gui_split_t split;
	b32 closed;
	b32 collapsed;
	b32 tabbed_out;
} gui_panel_t;

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_t flags);
void pgui_panel_add_tab(gui_panel_t *panel, gui_panel_t *tab);
b32  pgui_panel(gui_t *gui, gui_panel_t *panel);
void pgui_panel_collapse(gui_panel_t *panel);
void pgui_panel_restore(gui_panel_t *panel);
void pgui_panel_finish(gui_t *gui, gui_panel_t *panel);

void pgui_row(gui_t *gui, r32 height, u32 num_cells);
void pgui_row_cells(gui_t *gui, r32 height, const r32 *cells, u32 num_cells);
#define pgui_row_cellsv(gui, height, cells) \
	pgui_row_cells(gui, height, cells, countof(cells))
void pgui_row_empty(gui_t *gui, r32 height);

void pgui_col(gui_t *gui, r32 width, u32 num_cells);
void pgui_col_cells(gui_t *gui, r32 width, const r32 *cells, u32 num_cells);
#define pgui_col_cellsv(gui, width, cells) \
	pgui_col_cells(gui, width, cells, countof(cells))
void pgui_col_empty(gui_t *gui, r32 width);

void      pgui_spacer(gui_t *gui);
void      pgui_spacer_blank(gui_t *gui);
void      pgui_txt(gui_t *gui, const char *str);
void      pgui_img(gui_t *gui, const char *fname, img_scale_t scale);
btn_val_t pgui_btn_txt(gui_t *gui, const char *lbl);
btn_val_t pgui_btn_img(gui_t *gui, const char *fname, img_scale_t scale);
btn_val_t pgui_btn_pen(gui_t *gui, gui_pen_t pen);
b32       pgui_chk(gui_t *gui, const char *lbl, b32 *val);
b32       pgui_npt(gui_t *gui, char *lbl, u32 n, const char *hint,
                   npt_flags_t flags);
b32       pgui_npt_chars(gui_t *gui, char *lbl, u32 n, const char *hint,
                         npt_flags_t flags, const b32 chars[128]);
b32       pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt);
void      pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt);
void      pgui_dropdown_begin(gui_t *gui, u32 *val, u32 num_items);
b32       pgui_dropdown_item(gui_t *gui, const char *txt);
b32       pgui_slider_x(gui_t *gui, r32 *val);
b32       pgui_slider_y(gui_t *gui, r32 *val);
b32       pgui_range_x(gui_t *gui, r32 *val, r32 min, r32 max);
b32       pgui_range_y(gui_t *gui, r32 *val, r32 min, r32 max);

void pgui_subpanel_init(gui_t *gui, gui_panel_t *subpanel);
void pgui_subpanel(gui_t *gui, gui_panel_t *subpanel);
void pgui_subpanel_finish(gui_t *gui, gui_panel_t *subpanel);

void pgui_panel_to_front(gui_t *gui, gui_panel_t *panel);
int  pgui_panel_sort(const void *lhs, const void *rhs);

void pgui_cell(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h);

u64  pgui_next_widget_id(const gui_t *gui);

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
} gui_widget_style_t;

typedef struct gui_slider_style
{
	gui_widget_style_t handle;
	gui_widget_style_t track;
	b32 track_narrow;
} gui_slider_style_t;

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
	r32 padding;
} gui_panel_style_t;

typedef struct gui_style
{
	color_t bg_color;
	gui_line_style_t   line;
	gui_text_style_t   txt;
	gui_widget_style_t npt;
	gui_widget_style_t btn;
	gui_widget_style_t chk;
	gui_slider_style_t slider;
	gui_widget_style_t select;
	gui_widget_style_t dropdown;
	gui_widget_style_t drag;
	gui_panel_style_t  panel;
	gui_line_style_t   split;
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
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"
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
#include "stb_truetype.h"

#include "violet/array.h"
#include "violet/fmath.h"
#include "violet/graphics.h"
#include "violet/imath.h"

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

static void gui__split(gui_t *gui, gui_split_t *split, b32 render);

static const char *g_vertex_shader;
static const char *g_fragment_shader;


/* Color */

void color_as_float_array(r32 *f, color_t c)
{
	f[0] = c.r / 255.f;
	f[1] = c.g / 255.f;
	f[2] = c.b / 255.f;
	f[3] = c.a / 255.f;
}

color_t color_from_float(const r32 *f)
{
	return (color_t) {
		.r = f[0] * 255.f,
		.g = f[1] * 255.f,
		.b = f[2] * 255.f,
		.a = f[3] * 255.f,
	};
}

void color_to_hex(color_t c, char *hex, u32 n)
{
	snprintf(hex, n, "#%.2x%.2x%.2x%.2x", c.r, c.g, c.b, c.a);
}

color_t color_from_hex(const char *hex)
{
	color_t c = g_nocolor;
	u32 val;

	if (*hex == '#')
		++hex;

	val = strtoul(hex, NULL, 16);
	switch (strlen(hex)) {
	case 6:
		// no alpha
		c.r = (val >> 16) & 0xff;
		c.g = (val >> 8) & 0xff;
		c.b = val & 0xff;
		c.a = 255;
		break;
	case 8:
		// with alpha
		c.r = (val >> 24) & 0xff;
		c.g = (val >> 16) & 0xff;
		c.b = (val >> 8) & 0xff;
		c.a = val & 0xff;
		break;
	default:
		log_error("invalid color string '%s'", hex);
		break;
	}

	return c;
}

b32 color_equal(color_t lhs, color_t rhs)
{
	return memcmp(&lhs, &rhs, sizeof(color_t)) == 0;
}

color_t color_blend(color_t src, color_t dst)
{
	return (color_t) {
		.r = src.r / 2 + dst.r / 2,
		.g = src.g / 2 + dst.g / 2,
		.b = src.b / 2 + dst.b / 2,
		.a = src.a / 2 + dst.a / 2,
	};
}


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
	GL_CHECK(glGenTextures, 1, &tex->handle);
	tex->width = w;
	tex->height = h;
	texture_bind(tex);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
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

/* NOTE(rgriege): based on a cursory glance at C:\Windoge\Fonts,
 * 1MB seems to be sufficient for most fonts */
#define TTF_BUFFER_SZ (1 << 20)
static unsigned char ttf_buffer[TTF_BUFFER_SZ];

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

#define GUI__FONT_MIN_CHAR 32
#define GUI__FONT_MAX_CHAR 254
#define GUI__FONT_NUM_CHAR (GUI__FONT_MAX_CHAR - GUI__FONT_MIN_CHAR + 1)

b32 font_load(font_t *f, const char *filename, u32 sz)
{
#define BMP_DIM 512
	b32 retval = false;
	unsigned char bitmap[4*BMP_DIM*BMP_DIM], row[4*BMP_DIM];
	FILE *fp;
	stbtt_pack_context context;
	stbtt_fontinfo info;
	int ascent, descent, line_gap;
	r32 scale;

	fp = fopen(filename, "rb");
	if (!fp)
		goto out;

	if (fread(ttf_buffer, 1, TTF_BUFFER_SZ, fp) == 0) {
		fclose(fp);
		goto out;
	}
	fclose(fp);

	if (!stbtt_InitFont(&info, ttf_buffer,
	                    stbtt_GetFontOffsetForIndex(ttf_buffer, 0)))
		goto out;

	/* NOTE(rgriege): otherwise bitmap has noise at the bottom */
	memset(bitmap, 0, 4*BMP_DIM*BMP_DIM);

	if (!stbtt_PackBegin(&context, bitmap, BMP_DIM, BMP_DIM, 4*BMP_DIM, 1, NULL))
		goto out;

	/* TODO(rgriege): oversample with smaller fonts */
	// stbtt_PackSetOversampling(&context, 2, 2);

	f->char_info = malloc(GUI__FONT_NUM_CHAR * sizeof(stbtt_packedchar));
	if (!rgtt_PackFontRange(&context, &info, sz, GUI__FONT_MIN_CHAR,
	                        GUI__FONT_NUM_CHAR, f->char_info))
		goto pack_fail;

	stbtt_PackEnd(&context);

	for (int i = 0; i < BMP_DIM; ++i) {
		memset(row, ~0, 4*BMP_DIM);
		for (int j = 0; j < BMP_DIM; ++j)
			row[j*4+3] = bitmap[4*i*BMP_DIM+j];
		memcpy(&bitmap[4*i*BMP_DIM], row, 4*BMP_DIM);
	}

	texture_init(&f->texture, BMP_DIM, BMP_DIM, GL_RGBA, bitmap);

	f->filename = filename;
	f->sz = sz;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	scale = stbtt_ScaleForPixelHeight(&info, sz);
	f->ascent = scale * ascent;
	f->descent = scale * descent;
	f->line_gap = scale * line_gap;
	f->newline_dist = scale * (ascent - descent + line_gap);
	retval = true;
	goto out;

pack_fail:
	free(f->char_info);
out:
	return retval;
}

void font_destroy(font_t *f)
{
	free(f->char_info);
	texture_destroy(&f->texture);
}

static
void font__align_anchor(s32 *x, s32 *y, s32 w, s32 h, gui_align_t align)
{
	if (align & GUI_ALIGN_CENTER)
		*x += w / 2;
	else if (align & GUI_ALIGN_RIGHT)
		*x += w;
	else {} /* default to GUI_ALIGN_LEFT */
	if (align & GUI_ALIGN_MIDDLE)
		*y += h / 2;
	else if (align & GUI_ALIGN_TOP)
		*y += h;
	else {} /* default to GUI_ALIGN_BOTTOM */
}

static
s32 font__line_width(font_t *f, const char *txt)
{
	r32 width = 0, y = 0;
	stbtt_aligned_quad q;

	for (const char *c = txt; *c != '\0'; ++c) {
		const int ch = *(const u8*)c;
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR)
			stbtt_GetPackedQuad(f->char_info, f->texture.width, f->texture.height,
			                    ch - GUI__FONT_MIN_CHAR, &width, &y, &q, 1);
		else if (*c == '\n')
			goto out;
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


/* Polygon triangulation algorithm
 * Modified from John W. Ratcliff
 * www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml */
static
b32 gui__triangle_contains(v2f a, v2f b, v2f c, v2f p)
{
	const r32 ab_cross_ap = v2f_cross(v2f_sub(b, a), v2f_sub(p, a));
	const r32 bc_cross_bp = v2f_cross(v2f_sub(c, b), v2f_sub(p, b));
	const r32 ca_cross_cp = v2f_cross(v2f_sub(a, c), v2f_sub(p, c));
	return ab_cross_ap >= 0.f && bc_cross_bp >= 0.f && ca_cross_cp >= 0.f;
}

static
b32 gui__triangulate_snip(const v2f *poly, u32 u, u32 v, u32 w, u32 n)
{
	const v2f a = poly[u];
	const v2f b = poly[v];
	const v2f c = poly[w];
	static const float EPSILON = 0.0000000001f;

	/* cannot snip if b is a concave vertex */
	if ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) < EPSILON)
		return false;

	/* cannot snip if triangle abc contains another (concave) vtx in poly */
	for (u32 i = 0; i < n; ++i)
		if (   i != u && i != v && i != w
		    && !v2f_equal(poly[i], a)
		    && !v2f_equal(poly[i], b)
		    && !v2f_equal(poly[i], c)
		    && gui__triangle_contains(a, b, c, poly[i]))
			return false;
	return true;
}

u32 gui_triangulate_out_sz(u32 n)
{
	return 3 * (n - 2);
}

b32 gui_triangulate(const v2f *v_, u32 n_, v2f **triangles)
{
	const u32 out_vtx_cnt = gui_triangulate_out_sz(n_);
	v2f *v;
	u32 n;

	array_reserve(*triangles, out_vtx_cnt + n_);

	/* Copy the original poly into the last verts of triangle buffer
	 * so we can remove the vertices later. */
	v = &(*triangles)[out_vtx_cnt];
	memcpy(v, v_, n_ * sizeof(*v_));
	n = n_;

	while (n > 2) {
		const u32 old_n = n;
		for (u32 i = 0; i < n; ++i) {
			/* iterate in reverse order to likely reduce cost of vtx removal */
			const u32 a = (2 * n - i - 2) % n, b = n - i - 1, c = (n - i) % n;
			if (gui__triangulate_snip(v, a, b, c, n)) {
				array_append(*triangles, v[a]);
				array_append(*triangles, v[b]);
				array_append(*triangles, v[c]);
				/* remove b from remaining polygon */
				for (u32 j = b + 1; j < n; ++j)
					(*triangles)[out_vtx_cnt + j - 1] = (*triangles)[out_vtx_cnt + j];
				--n;
				break;
			}
		}
		if (old_n == n) {
			array_clear(*triangles);
			return false;
		}
	}

	return true;
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
}

#define gi__gui_slider_style_default { \
	.handle = gi__gui_chk_style_default, \
	.track = { \
		.pen = gui_pen_rect, \
		.inactive = gi__gui_btn_inactive_style_default, \
		.hot = gi__gui_btn_inactive_style_default, \
		.active = gi__gui_btn_inactive_style_default, \
		.disabled = gi__gui_btn_disabled_style_default, \
	}, \
	.track_narrow = false, \
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
		},
		.scrollbar = gi__gui_slider_style_default,
		.cell_bg_color = gi_nocolor,
		.cell_border_color = gi_nocolor,
		.padding = 10.f,
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
		.padding           = 10.f,
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

#ifndef GUI_MAX_SCISSORS
#define GUI_MAX_SCISSORS 32
#endif

typedef struct draw_call
{
	GLint idx;
	GLsizei cnt;
	u32 type;
	GLuint tex;
} draw_call_t;

typedef struct gui__scissor
{
	u32 draw_call_idx, draw_call_cnt;
	s32 x, y, w, h;
} gui__scissor_t;

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
	gui__scissor_t scissors[GUI_MAX_SCISSORS];
	u32 scissor_idx;
	u32 scissor_cnt;

	/* mouse */
	v2i win_halfdim;
	v2i mouse_pos;
	v2i mouse_pos_last;
	v2i mouse_pos_press;
	u32 mouse_btn;
	u32 mouse_btn_diff;
	b32 mouse_covered_by_panel;
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

	/* general widget state */
	u32 lock;
	u64 hot_id;    /* hover */
	u64 active_id; /* mouse down */
	u64 focus_id;  /* widgets like input boxes change state post-click */
	u64 active_id_at_frame_start;
	b32 hot_id_found_this_frame;
	b32 active_id_found_this_frame;
	b32 focus_id_found_this_frame;
	b32 focus_next_widget;
	u64 focus_prev_widget_id;
	u64 prev_widget_id;

	/* specific widget state */
	u32 npt_cursor_pos;
	v2i drag_offset;
	char *pw_buf;
	v2f *vert_buf;
	struct
	{
		u64 id;
		s32 x, y, w, h;
		u32 val;
		u32 num_items;
		u32 item_idx;
		b32 triggered_by_key;
		b32 contains_mouse;
	} current_dropdown;
	struct
	{
		u64 id;
		s32 x, y, w, h;
		u32 *val;
		u32 num_items;
		u32 item_idx;
		b32 render_items;
		b32 close_at_end;
		char selected_item_txt[128];
		struct
		{
			s32 x, y, w, h;
		} prev_mask;
	} focused_dropdown;

	/* splits & panels */
	gui_split_t *root_split;
	gui_panel_t *panel;
	u32 next_panel_id;
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
	if (SDL_GetDisplayUsableBounds(0, &usable_bounds) != 0) {
		log_error("SDL_GetDisplayUsableBounds failed: %s", SDL_GetError());
		goto err_ctx;
	}


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
	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x, &gui->win_halfdim.y);
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div_eq(&gui->win_halfdim, g_v2i_2);

	gui->creation_time = time_current();
	gui->frame_start_time = gui->creation_time;
	gui->last_input_time = gui->creation_time;
	gui->frame_time_milli = 0;

	memset(gui->prev_keys, 0, KB_COUNT);
	memset(gui->key_toggles, 0, KBT_COUNT * sizeof(gui__key_toggle_state_t));
	gui__repeat_init(&gui->key_repeat);

	gui_style_set(gui, &g_gui_style_default);

	gui->style_stack_sz = 0;

	SDL_GetMouseState(&gui->mouse_pos.x, &gui->mouse_pos.y);
	gui->mouse_pos_last = gui->mouse_pos;
	gui->mouse_pos_press = gui->mouse_pos;
	gui->mouse_debug = false;
	gui__repeat_init(&gui->mouse_repeat);

	gui->lock = 0;
	gui->hot_id = 0;
	gui->active_id = 0;
	gui->focus_id = 0;
	gui->active_id_at_frame_start = 0;
	gui->focus_next_widget = false;
	gui->focus_prev_widget_id = 0;
	gui->prev_widget_id = 0;


	gui->npt_cursor_pos = 0;
	gui->drag_offset = g_v2i_zero;
	gui->pw_buf = array_create();
	gui->vert_buf = array_create();

	gui->root_split = NULL;
	gui->panel = NULL;
	gui->next_panel_id = 1;
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
	array_destroy(gui->pw_buf);
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
	const v2i dp = { .x = dx, .y = dy };
	v2i pos;
	SDL_GetWindowPosition(gui->window, &pos.x, &pos.y);
	v2i_add_eq(&pos, dp);
	SDL_SetWindowPosition(gui->window, pos.x, pos.y);
}

void gui_dim(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->win_halfdim.x * 2;
	*y = gui->win_halfdim.y * 2;
}

void gui_minimize(gui_t *gui)
{
	SDL_MinimizeWindow(gui->window);
}

void gui_maximize(gui_t *gui)
{
	int display_idx;
	SDL_Rect usable_bounds;

	display_idx = SDL_GetWindowDisplayIndex(gui->window);
	if (display_idx < 0) {
		log_error("SDL_GetWindowDisplayIndex failed: %s", SDL_GetError());
		return;
	}

	if (SDL_GetDisplayUsableBounds(display_idx, &usable_bounds) != 0) {
		log_error("SDL_GetDisplayUsableBounds failed: %s", SDL_GetError());
		return;
	}

	SDL_SetWindowPosition(gui->window, usable_bounds.x, usable_bounds.y);
	SDL_SetWindowSize(gui->window, usable_bounds.w, usable_bounds.h);
}

void gui_fullscreen(gui_t *gui)
{
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
void gui__tab_focus_adjacent_widget(gui_t *gui)
{
	gui->focus_id = 0;
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

	gui->focus_id = id;
	gui->focus_next_widget = false;
	gui->focus_prev_widget_id = 0;
	gui->focus_id_found_this_frame = true;
}

static
void gui__defocus_dropdown(gui_t *gui)
{
	gui->focused_dropdown.id                   = 0;
	gui->focused_dropdown.val                  = NULL;
	gui->focused_dropdown.num_items            = 0;
	gui->focused_dropdown.item_idx             = 0;
	gui->focused_dropdown.render_items         = false;
	gui->focused_dropdown.close_at_end         = false;
	gui->focused_dropdown.selected_item_txt[0] = '\0';
}

b32 gui_begin_frame(gui_t *gui)
{
	s32 key_cnt;
	b32 quit = false;
	float bg_color[4];
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
			gui->mouse_btn |= (evt.wheel.y > 0 ? MB_WHEELUP : MB_WHEELDOWN);
			gui->last_input_time = now;
		break;
		case SDL_WINDOWEVENT:
			if (evt.window.event == SDL_WINDOWEVENT_CLOSE)
				quit = true;
			else
				gui->last_input_time = now;
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

	gui->mouse_pos_last = gui->mouse_pos;
	gui->mouse_btn |= SDL_GetMouseState(&gui->mouse_pos.x, &gui->mouse_pos.y);
	gui->mouse_btn_diff = gui->mouse_btn ^ last_mouse_btn;
	if (mouse_pressed(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT))
		gui->mouse_pos_press = gui->mouse_pos;
	gui__repeat_update(&gui->mouse_repeat, gui->mouse_btn,
	                   __builtin_popcount(gui->mouse_btn),
	                   gui->frame_time_milli);

	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x, &gui->win_halfdim.y);
	gui->mouse_pos.y = gui->win_halfdim.y - gui->mouse_pos.y;
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div_eq(&gui->win_halfdim, g_v2i_2);

	gui->mouse_covered_by_panel = false;

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
	if (gui->focus_id != 0 && !gui->focus_id_found_this_frame) {
		log_warn("focus widget %" PRIu64 " was not drawn", gui->focus_id);
		gui->focus_id = 0;
		if (SDL_IsTextInputActive())
			SDL_StopTextInput();
	}
	gui->focus_id_found_this_frame = false;

	if (   gui->key_repeat.triggered
	    && gui->key_repeat.val == KB_TAB
	    && gui->focus_id == 0)
		gui__tab_focus_adjacent_widget(gui);

	gui->active_id_at_frame_start = gui->active_id;

	gui->vert_cnt = 0;
	gui->draw_call_cnt = 0;
	gui->scissor_idx = 0;
	gui->scissor_cnt = 0;
	gui_unmask(gui);

	GL_CHECK(glViewport, 0, 0, 2*gui->win_halfdim.x, 2*gui->win_halfdim.y);

	/* NOTE(rgriege): reset the scissor for glClear */
	GL_CHECK(glScissor, 0, 0, gui->win_halfdim.x * 2, gui->win_halfdim.y * 2);

	color_as_float_array(bg_color, gui->style.bg_color);
	GL_CHECK(glClearColor, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
	GL_CHECK(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* focused dropdown */
	if (gui->focused_dropdown.id != 0) {
		const s32 x = gui->focused_dropdown.x;
		const s32 y = gui->focused_dropdown.y;
		const s32 w = gui->focused_dropdown.w;
		const s32 h = gui->focused_dropdown.h;
		const u32 num_items = gui->focused_dropdown.num_items;
		box2i box;
		box2i_from_dims(&box, x, y + h, x + w, y - (s32)num_items * h);

		gui->focused_dropdown.render_items = true;

		/* catch mouse_covered_by_panel */
		if (box2i_contains_point(box, gui->mouse_pos)) {
			gui->mouse_covered_by_panel = true;
		} else if (mouse_pressed(gui, ~0)) {
			gui__defocus_dropdown(gui);
			gui->focus_id = 0;
		}
	}

	if (gui->focused_dropdown.id != 0) {
		/* reserve first scissor for dropdown items */
		assert(gui->scissor_idx == 0 && gui->scissor_cnt == 1);
		gui->scissor_idx = 1;
		gui->scissor_cnt = 2;
		memcpy(&gui->scissors[1], &gui->scissors[0], sizeof(gui->scissors[0]));
	}

	gui->use_default_cursor = true;
	if (gui->root_split) {
		box2i_from_center(&gui->root_split->box, gui->win_halfdim, gui->win_halfdim);
		gui__split(gui, gui->root_split, true);
	}

	gui->next_panel_pri = 0;
	gui->min_panel_pri = 0;

	if (gui->mouse_debug && mouse_pressed(gui, MB_LEFT | MB_MIDDLE | MB_RIGHT))
		gui_circ(gui, gui->mouse_pos.x, gui->mouse_pos.y, 10, g_yellow, g_nocolor);

	/* kinda wasteful, but ensures split resizers & mouse debug are drawn on top */
	gui_unmask(gui);

	return !quit;
}

typedef enum draw_call_type
{
	DRAW_POINTS,
	DRAW_LINE_STRIP,
	DRAW_LINE_LOOP,
	DRAW_LINES,
	DRAW_TRIANGLE_STRIP,
	DRAW_TRIANGLE_FAN,
	DRAW_TRIANGLES,
	DRAW_QUAD_STRIP,
	DRAW_QUADS,
	DRAW_POLYGON,
	DRAW_COUNT
} draw_call_type_t;

static const GLenum g_draw_call_types[DRAW_COUNT] = {
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
void gui__triangles(gui_t *gui, const v2f *v, u32 n, color_t fill)
{
	if (color_equal(fill, g_nocolor))
		return;

	assert(n % 3 == 0);

	for (u32 i = 0; i < n; ++i) {
		gui->verts[gui->vert_cnt+i] = v[i];
		gui->vert_colors[gui->vert_cnt+i] = fill;
		gui->vert_tex_coords[gui->vert_cnt+i] = g_v2f_zero;
	}
	gui->draw_calls[gui->draw_call_cnt].idx = gui->vert_cnt;
	gui->draw_calls[gui->draw_call_cnt].cnt = n;
	gui->draw_calls[gui->draw_call_cnt].type = DRAW_TRIANGLES;
	gui->draw_calls[gui->draw_call_cnt].tex = gui->texture_white.handle;
	++gui->draw_call_cnt;
	gui->vert_cnt += n;
}

static
void gui__current_mask(const gui_t *gui, box2i *box)
{
	assert(gui->scissor_idx < gui->scissor_cnt);
	box2i_from_xywh(box, gui->scissors[gui->scissor_idx].x,
	                gui->scissors[gui->scissor_idx].y,
	                gui->scissors[gui->scissor_idx].w,
	                gui->scissors[gui->scissor_idx].h);
}

static
void gui__current_maskf(const gui_t *gui, box2f *box)
{
	box2i mask;
	gui__current_mask(gui, &mask);
	box->min.x = mask.min.x;
	box->min.y = mask.min.y;
	box->max.x = mask.max.x;
	box->max.y = mask.max.y;
}

static
void gui__mask_box(const gui_t *gui, box2i *box)
{
	box2i mask;
	gui__current_mask(gui, &mask);
	box2i_clamp_point(mask, &box->min);
	box2i_clamp_point(mask, &box->max);
}

b32 gui_point_visible(const gui_t *gui, s32 x, s32 y)
{
	const v2i pt = { .x = x, .y = y };
	box2i mask;
	gui__current_mask(gui, &mask);
	return box2i_contains_point(mask, pt);
}

static
b32 gui__box_half_visible(const gui_t *gui, box2i box)
{
	box2i mask, clipped;
	gui__current_mask(gui, &mask);
	clipped = box2i_intersection(mask, box);
	return    clipped.max.x - clipped.min.x >= (box.max.x - box.min.x) / 2
	       && clipped.max.y - clipped.min.y >= (box.max.y - box.min.y) / 2;
}

static
void gui__poly(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t stroke)
{
	box2f mask, bbox;

	gui__current_maskf(gui, &mask);
	polyf_bounding_box(v, n, &bbox);
	if (!box2f_overlaps(mask, bbox))
		return;

	if (!color_equal(fill, g_nocolor)) {
		draw_call_t *draw_call = &gui->draw_calls[gui->draw_call_cnt];
		assert(gui->vert_cnt + n <= GUI_MAX_VERTS);
		assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

		for (u32 i = 0; i < n; ++i) {
			gui->verts[gui->vert_cnt+i] = v[i];
			gui->vert_colors[gui->vert_cnt+i] = fill;
			gui->vert_tex_coords[gui->vert_cnt+i] = g_v2f_zero;
		}
		draw_call->idx = gui->vert_cnt;
		draw_call->cnt = n;
		draw_call->type = DRAW_TRIANGLE_FAN;
		draw_call->tex = gui->texture_white.handle;
		++gui->draw_call_cnt;
		gui->vert_cnt += n;
	}

	if (!color_equal(stroke, g_nocolor)) {
		draw_call_t *draw_call = &gui->draw_calls[gui->draw_call_cnt];
		const r32 dash_len = gui->style.line.dash_len;
		assert(gui->vert_cnt + n <= GUI_MAX_VERTS);
		assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

		if (dash_len != 0.f) {
			const u32 vn = n > 2 ? n + 1 : n;
			r32 dist = 0.f;
			for (u32 i = 0; i < vn; ++i) {
				gui->verts[gui->vert_cnt+i] = v[i%n];
				gui->vert_colors[gui->vert_cnt+i] = stroke;
				gui->vert_tex_coords[gui->vert_cnt+i].x = dist / dash_len;
				gui->vert_tex_coords[gui->vert_cnt+i].y = 0;
				dist += v2f_dist(v[i%n], v[(i+1)%n]);
			}
			draw_call->tex = gui->texture_white_dotted.handle;
			draw_call->idx = gui->vert_cnt;
			draw_call->cnt = vn;
			draw_call->type = DRAW_LINE_STRIP;
			++gui->draw_call_cnt;
			gui->vert_cnt += vn;
		} else {
			for (u32 i = 0; i < n; ++i) {
				gui->verts[gui->vert_cnt+i] = v[i];
				gui->vert_colors[gui->vert_cnt+i] = stroke;
				gui->vert_tex_coords[gui->vert_cnt+i] = g_v2f_zero;
			}
			draw_call->tex = gui->texture_white.handle;
			draw_call->idx = gui->vert_cnt;
			draw_call->cnt = n;
			draw_call->type = DRAW_LINE_LOOP;
			++gui->draw_call_cnt;
			gui->vert_cnt += n;
		}
	}
}

static
void texture__render(gui_t *gui, const texture_t *texture, s32 x, s32 y,
                     r32 sx, r32 sy, color_t color)
{
	r32 w, h;
	box2f mask, bbox;

	assert(gui->vert_cnt + 4 < GUI_MAX_VERTS);
	assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

	w = texture->width * sx;
	h = texture->height * sy;

	gui__current_maskf(gui, &mask);
	box2f_from_xywh(&bbox, x, y, w, h);
	if (!box2f_overlaps(mask, bbox))
		return;

	v2f_set(&gui->verts[gui->vert_cnt],   x,     y);
	v2f_set(&gui->verts[gui->vert_cnt+1], x,     y + h);
	v2f_set(&gui->verts[gui->vert_cnt+2], x + w, y + h);
	v2f_set(&gui->verts[gui->vert_cnt+3], x + w, y);

	gui->vert_colors[gui->vert_cnt]   = color;
	gui->vert_colors[gui->vert_cnt+1] = color;
	gui->vert_colors[gui->vert_cnt+2] = color;
	gui->vert_colors[gui->vert_cnt+3] = color;

	v2f_set(&gui->vert_tex_coords[gui->vert_cnt],   0, 0);
	v2f_set(&gui->vert_tex_coords[gui->vert_cnt+1], 0, 1);
	v2f_set(&gui->vert_tex_coords[gui->vert_cnt+2], 1, 1);
	v2f_set(&gui->vert_tex_coords[gui->vert_cnt+3], 1, 0);

	gui->draw_calls[gui->draw_call_cnt].idx = gui->vert_cnt;
	gui->draw_calls[gui->draw_call_cnt].cnt = 4;
	gui->draw_calls[gui->draw_call_cnt].type = DRAW_TRIANGLE_FAN;
	gui->draw_calls[gui->draw_call_cnt].tex = texture->handle;
	++gui->draw_call_cnt;

	gui->vert_cnt += 4;
}

static
void text__render(gui_t *gui, const texture_t *texture, r32 x0, r32 y0,
                  r32 x1, r32 y1, r32 s0, r32 t0, r32 s1, r32 t1, color_t color)
{
	box2f mask, bbox;

	assert(gui->vert_cnt + 4 < GUI_MAX_VERTS);
	assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

	gui__current_maskf(gui, &mask);
	box2f_from_dims(&bbox, x0, y1, x1, y0);
	if (!box2f_overlaps(mask, bbox))
		return;

	v2f_set(&gui->verts[gui->vert_cnt],   x0, y1);
	v2f_set(&gui->verts[gui->vert_cnt+1], x0, y0);
	v2f_set(&gui->verts[gui->vert_cnt+2], x1, y0);
	v2f_set(&gui->verts[gui->vert_cnt+3], x1, y1);

	gui->vert_colors[gui->vert_cnt]   = color;
	gui->vert_colors[gui->vert_cnt+1] = color;
	gui->vert_colors[gui->vert_cnt+2] = color;
	gui->vert_colors[gui->vert_cnt+3] = color;

	v2f_set(&gui->vert_tex_coords[gui->vert_cnt],   s0, 1.f-t0);
	v2f_set(&gui->vert_tex_coords[gui->vert_cnt+1], s0, 1.f-t1);
	v2f_set(&gui->vert_tex_coords[gui->vert_cnt+2], s1, 1.f-t1);
	v2f_set(&gui->vert_tex_coords[gui->vert_cnt+3], s1, 1.f-t0);

	gui->draw_calls[gui->draw_call_cnt].idx = gui->vert_cnt;
	gui->draw_calls[gui->draw_call_cnt].cnt = 4;
	gui->draw_calls[gui->draw_call_cnt].type = DRAW_TRIANGLE_FAN;
	gui->draw_calls[gui->draw_call_cnt].tex = texture->handle;
	++gui->draw_call_cnt;

	gui->vert_cnt += 4;
}

static
void gui__complete_scissor(gui_t *gui)
{
	gui__scissor_t *scissor;

	if (gui->scissor_cnt == 0)
		return;

	scissor = &gui->scissors[gui->scissor_idx];
	if (   gui->scissor_idx + 1 == gui->scissor_cnt
	    && scissor->draw_call_idx == gui->draw_call_cnt)
		--gui->scissor_cnt;
	else
		scissor->draw_call_cnt = gui->draw_call_cnt - scissor->draw_call_idx;
}

void gui_end_frame(gui_t *gui)
{
#ifdef __EMSCRIPTEN__
	const s32 *loc = gui->shader_attrib_loc;
#else
	const s32 loc[VBO_COUNT] = { VBO_VERT, VBO_COLOR, VBO_TEX };
#endif
	GLuint current_texture = 0;

	gui__complete_scissor(gui);

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
	         gui->win_halfdim.x, gui->win_halfdim.y);

	/* NOTE(rgriege): This method of ordering creates an inconsistency:
	 * panels/layers must be called from top-to-bottom, but widgets/primitives
	 * within a layer must be called from bottom-to-top.  Without introducing a
	 * frame of delay, top-to-bottom panels are unavoidable to ensure that
	 * the top panels receive the input events.  Bottom-to-top widget rendering
	 * is nice for overlaying text on top of a movable widget. Will be a problem
	 * if overlapping widges are in the same panel/layer, but that doesn't seem
	 * like a use case to design for other than dragging icons on a desktop,
	 * which could be 'solved' by placing the dragged icon on a separate layer. */
	for (gui__scissor_t *scissor_first = gui->scissors,
	                    *scissor = scissor_first + gui->scissor_cnt - 1;
	     scissor >= scissor_first; --scissor) {
		GL_CHECK(glScissor, scissor->x, scissor->y, scissor->w, scissor->h);
		for (draw_call_t *draw_call = gui->draw_calls + scissor->draw_call_idx,
		                 *draw_call_end = draw_call + scissor->draw_call_cnt;
		     draw_call != draw_call_end; ++draw_call) {
			if (draw_call->tex != current_texture) {
				GL_CHECK(glBindTexture, GL_TEXTURE_2D, draw_call->tex);
				current_texture = draw_call->tex;
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

b32 mouse_pressed(const gui_t *gui, u32 mask)
{
	return (gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b32 mouse_pressed_bg(const gui_t *gui, u32 mask)
{
	return    mouse_pressed(gui, mask)
	       && gui->active_id == 0
	       && gui->active_id_at_frame_start == 0
	       && !gui->mouse_covered_by_panel;
}

b32 mouse_down(const gui_t *gui, u32 mask)
{
	return gui->mouse_btn & mask;
}

b32 mouse_released(const gui_t *gui, u32 mask)
{
	return !(gui->mouse_btn & mask) && (gui->mouse_btn_diff & mask);
}

b32 mouse_released_bg(const gui_t *gui, u32 mask)
{
	return    mouse_released(gui, mask)
	       && gui->active_id == 0
	       && gui->active_id_at_frame_start == 0
	       && !gui->mouse_covered_by_panel;
}

b32 mouse_over_bg(const gui_t *gui)
{
	return !gui->mouse_covered_by_panel;
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

	gui__poly(gui, poly, 4, c, g_nocolor);
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
		gui__poly(gui, poly, 2, g_nocolor, c);
	} else {
		gui__line_wide(gui, x0, y0, x1, y1, w, c);
	}
}

void gui_linef(gui_t *gui, r32 x0, r32 y0, r32 x1, r32 y1, r32 w, color_t c)
{
	assert(w >= 1);
	if (fabsf(w - 1) < 0.01f) {
		const v2f poly[2] = { { x0, y0 }, { x1, y1 } };
		gui__poly(gui, poly, 2, g_nocolor, c);
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
	gui__poly(gui, poly, 4, fill, stroke);
}

#define gui__circ_poly_sz(radius) ((u32)(4 + 2 * radius))

static
void gui__circ_poly(r32 x, r32 y, r32 r, v2f *v, u32 nmax)
{
	const u32 n = gui__circ_poly_sz(r);
	const r32 radians_slice = 2 * fPI / n;
	assert(n <= nmax);
	for (u32 i = 0; i < n; ++i) {
		const r32 radians = i * radians_slice;
		v[i] = (v2f){ .x=x+r*cos(radians), .y=y+r*sin(radians) };
	}
}

void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t stroke)
{
	array_set_sz(gui->vert_buf, gui__circ_poly_sz(r));
	gui__circ_poly(x, y, r, A2PN(gui->vert_buf));
	gui__poly(gui, gui->vert_buf, gui__circ_poly_sz(r), fill, stroke);
	array_clear(gui->vert_buf);
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
		gui__poly(gui, v, n, fill, stroke);
	} else {
		if (gui_triangulate(v, n, &gui->vert_buf)) {
			gui__triangles(gui, A2PN(gui->vert_buf), fill);
			array_clear(gui->vert_buf);
		}
		if (stroke.a != 0)
			gui__poly(gui, v, n, g_nocolor, stroke);
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
		gui_img_ex(gui, x, y, img, 1.f, 1.f, 1.f);
}

void gui_img_ex(gui_t *gui, s32 x, s32 y, const img_t *img, r32 sx, r32 sy,
                r32 opacity)
{
	color_t color = g_white;
	color.a = opacity * 255;
	texture__render(gui, &img->texture, x, y, sx, sy, color);
}

void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                   img_scale_t scale)
{
	const img_t *img = gui__find_or_load_img(gui, fname);
	if (img) {
		switch (scale) {
		case IMG_CENTERED:;
			const s32 x_off = (w - img->texture.width) / 2;
			const s32 y_off = (h - img->texture.height) / 2;
			if (x_off >= 0 && y_off >= 0) {
				gui_img_ex(gui, x + x_off, y + y_off, img, 1.f, 1.f, 1.f);
				break;
			}
		case IMG_SCALED:;
			const r32 sx = (r32)w / img->texture.width;
			const r32 sy = (r32)h / img->texture.height;
			gui_img_ex(gui, x, y, img, sx, sy, 1.f);
		break;
		}
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
void gui__add_char_to_line_width(font_t *font, char c, r32 *line_width)
{
	const int ch = c;
	r32 y = 0;
	stbtt_aligned_quad q;

	if (ch < GUI__FONT_MIN_CHAR && ch > GUI__FONT_MAX_CHAR)
		return;

	stbtt_GetPackedQuad(font->char_info, font->texture.width, font->texture.height,
	                    ch - GUI__FONT_MIN_CHAR, line_width, &y, &q, 1);
}

static
void gui__wrap_txt(gui_t *gui, char *txt, const gui_text_style_t *style, r32 max_width)
{
	char *p = txt;
	r32 line_width = style->padding;
	font_t *font = gui__get_font(gui, style->size);
	assert(font);

	while (*p != 0) {
		char *p_word_start = NULL;
		r32 line_width_word_start;

		if (*p == ' ') {
			p_word_start = p;
			gui__add_char_to_line_width(font, *p, &line_width);
			++p;
		} else if (*p == '\n') {
			++p;
			line_width = style->padding;
		}
		line_width_word_start = line_width;

		while (*p != ' ' && *p != '\n' && *p != 0) {
			gui__add_char_to_line_width(font, *p, &line_width);
			++p;
		}

		if (line_width >= max_width) {
			if (p_word_start)
				*p_word_start = '\n';
			line_width = style->padding + line_width - line_width_word_start;
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

	font = gui__get_font(gui, style->size);
	assert(font);

	if (style->wrap) {
		const u32 len = (u32)strlen(txt);
		array_init_ex(buf, len + 1, g_temp_allocator);
		memcpy(buf, txt, len + 1);
		gui__wrap_txt(gui, buf, style, w);
		txt = buf;
	}

	font__align_anchor(&ix_, &iy_, w, h, style->align);
	x = ix_ + font__line_offset_x(font, txt, style);
	y = iy_ + font__offset_y(font, txt, style);

	if (pos == 0)
		goto out;

	for (u32 i = 0; i < pos; ++i) {
		const int ch = *(const u8*)&txt[i];
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
		} else if (txt[i] == '\n') {
			y -= font->newline_dist;
			x = ix_ + font__line_offset_x(font, &txt[i+1], style);
		}
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

	font = gui__get_font(gui, style->size);
	assert(font);

	x += font__line_offset_x(font, txt, style);
	y += font__offset_y(font, txt, style);

	for (const char *c = txt; *c != '\0'; ++c) {
		const int ch = *(const u8*)c;
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
			gui__fixup_stbtt_aligned_quad(&q, y);
			text__render(gui, &font->texture, q.x0, q.y0, q.x1, q.y1, q.s0, q.t0,
			             q.s1, q.t1, style->color);
		} else if (*c == '\n') {
			y -= font->newline_dist;
			x = *ix + font__line_offset_x(font, c + 1, style);
		}
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
	const char *txt = txt_;

	if (style->wrap) {
		const u32 len = (u32)strlen(txt);
		array_init_ex(buf, len + 1, g_temp_allocator);
		memcpy(buf, txt, len + 1);
		gui__wrap_txt(gui, buf, style, w);
		txt = buf;
	}

	font = gui__get_font(gui, style->size);
	assert(font);

	font__align_anchor(&xi, &yi, w, h, style->align);
	x = xi + font__line_offset_x(font, txt, style);
	y = yi + font__offset_y(font, txt, style);

	v2i_set(&p, x, y + font->ascent / 2);
	closest_pos = 0;
	closest_dist = v2i_dist_sq(p, mouse);

	for (const char *c = txt; *c != '\0'; ++c) {
		const int ch = *(const u8*)c;
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
		} else if (*c == '\n') {
			y -= font->newline_dist;
			x = xi + font__line_offset_x(font, c + 1, style);
		} else {
			continue;
		}
		v2i_set(&p, roundf(x), roundf(y + font->ascent / 2));
		dist = v2i_dist_sq(p, mouse);
		if (dist < closest_dist) {
			closest_dist = dist;
			closest_pos = c - txt + 1;
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
	assert(font);

	x += font__line_offset_x(font, txt, &style);
	y += font__offset_y(font, txt, &style);

	x_range.l = x_range.r = x;
	y_range.l = y_range.r = y;

	for (const char *c = txt; *c != '\0'; ++c) {
		const int ch = *(const u8*)c;
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
			gui__fixup_stbtt_aligned_quad(&q, y);

			x_range.l = min(x_range.l, q.x0);
			x_range.r = max(x_range.r, q.x1);
			y_range.l = min(y_range.l, q.y0);
			y_range.r = max(y_range.r, q.y1);
		} else if (*c == '\n') {
			y -= font->newline_dist;
			x = x_ + font__line_offset_x(font, c + 1, &style);
		}
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
	assert(font);
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

void gui__mask(gui_t *gui, u32 idx, s32 x, s32 y, s32 w, s32 h)
{
	gui->scissors[idx].draw_call_idx = gui->draw_call_cnt;
	gui->scissors[idx].draw_call_cnt = 0;
	gui->scissors[idx].x = x;
	gui->scissors[idx].y = y;
	gui->scissors[idx].w = w;
	gui->scissors[idx].h = h;
	gui->scissor_idx = idx;
}

void gui_mask(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	gui__complete_scissor(gui);
	assert(gui->scissor_cnt < GUI_MAX_SCISSORS);
	gui__mask(gui, gui->scissor_cnt, x, y, w, h);
	++gui->scissor_cnt;
}

void gui_unmask(gui_t *gui)
{
	gui_mask(gui, 0, 0, gui->win_halfdim.x * 2, gui->win_halfdim.y * 2);
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

	font__align_anchor(&x, &y, w, h, style->align);

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
	return    (gui->hot_id == 0 || !gui->hot_id_found_this_frame)
	       && gui->active_id == 0
	       && !gui->mouse_covered_by_panel;
}

static
b32 gui__allow_new_interaction(const gui_t *gui)
{
	return    gui__allow_new_panel_interaction(gui)
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
	else if (gui->focus_id == id)
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

const b32 g_gui_npt_chars_print[128] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
};

const b32 g_gui_npt_chars_numeric[128] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static
void gui__npt_move_cursor_vertical(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                                   const char *txt, s32 diff, u32 fallback)
{
	const u32 orig_pos = gui->npt_cursor_pos;
	const gui_text_style_t *style = &gui->style.npt.active.text;
	v2i cursor = { .x = x, .y = y };

	gui__txt_char_pos(gui, &cursor.x, &cursor.y, w, h, txt, orig_pos, style);
	cursor.y += diff;
	gui->npt_cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, txt, cursor, style);
	if (gui->npt_cursor_pos == orig_pos && orig_pos != fallback)
		gui->npt_cursor_pos = fallback;
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

b32 gui_npt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
            const char *hint, npt_flags_t flags)
{
	return gui_npt_chars(gui, x, y, w, h, txt, n, hint, flags,
	                     g_gui_npt_chars_print);
}

b32 gui_npt_chars(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                  const char *hint, npt_flags_t flags, const b32 chars[128])
{
	const u64 id = gui_widget_id(gui, x, y);
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	const b32 contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                           && gui__box_half_visible(gui, box)
	                           && !gui->lock;
	b32 complete = false;
	const char *displayed_txt;

	if (gui->focus_id == id) {
		if (contains_mouse && mouse_pressed(gui, MB_LEFT)) {
			const gui_text_style_t *style = &gui->style.npt.active.text;
			gui->npt_cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, txt,
			                                         gui->mouse_pos, style);
		} else {
			const u32 len = (u32)strlen(txt);
			gui->npt_cursor_pos = clamp(0, gui->npt_cursor_pos, len);
		}
		if (strlen(gui->text_npt) > 0 && !key_mod(gui, KBM_CTRL)) {
			u32 len = (u32)strlen(txt);
			for (size_t k = 0, kn = strlen(gui->text_npt); k < kn; ++k) {
				if (gui->text_npt[k] > 0 && len < n - 1 && chars[(u8)gui->text_npt[k]]) {
					for (u32 i = len + 1; i > gui->npt_cursor_pos; --i)
						txt[i] = txt[i-1];
					txt[gui->npt_cursor_pos++] = gui->text_npt[k];
					++len;
				}
			}
		} else if (gui->key_repeat.triggered) {
			const u32 key_idx = gui->key_repeat.val;
			const u32 len = (u32)strlen(txt);
			if (key_idx == KB_BACKSPACE) {
				if (gui->npt_cursor_pos > 0) {
					for (u32 i = gui->npt_cursor_pos - 1; i < len; ++i)
						txt[i] = txt[i+1];
					--gui->npt_cursor_pos;
				}
			} else if (key_idx == KB_DELETE) {
				for (u32 i = gui->npt_cursor_pos; i < len; ++i)
					txt[i] = txt[i+1];
			} else if (key_idx == KB_RETURN || key_idx == KB_KP_ENTER) {
				gui->focus_id = 0;
				SDL_StopTextInput();
				complete = true;
			} else if (key_idx == KB_ESCAPE) {
				gui->focus_id = 0;
				SDL_StopTextInput();
			} else if (key_idx == KB_V && key_mod(gui, KBM_CTRL)) {
				char *clipboard;
				u32 cnt;
				if (   SDL_HasClipboardText()
				    && (clipboard = SDL_GetClipboardText())
				    && (cnt = min(n - 1 - len, (u32)strlen(clipboard))) > 0) {
					memmove(&txt[gui->npt_cursor_pos + cnt],
					        &txt[gui->npt_cursor_pos],
					        len - gui->npt_cursor_pos + 1);
					memcpy(&txt[gui->npt_cursor_pos], clipboard, cnt);
					gui->npt_cursor_pos += cnt;
					SDL_free(clipboard);
				}
			} else if (gui__key_up(gui)) {
				const gui_text_style_t *style = &gui->style.npt.active.text;
				const font_t *font = gui__get_font(gui, style->size);
				gui__npt_move_cursor_vertical(gui, x, y, w, h, txt,
				                              font->newline_dist, 0);
			} else if (gui__key_down(gui)) {
				const gui_text_style_t *style = &gui->style.npt.active.text;
				const font_t *font = gui__get_font(gui, style->size);
				gui__npt_move_cursor_vertical(gui, x, y, w, h, txt,
				                              -font->newline_dist, len);
			} else if (gui__key_left(gui)) {
				if (gui->npt_cursor_pos > 0)
					--gui->npt_cursor_pos;
			} else if (gui__key_right(gui)) {
				if (gui->npt_cursor_pos < len)
					++gui->npt_cursor_pos;
			} else if (gui__key_home(gui)) {
				gui->npt_cursor_pos = 0;
			} else if (gui__key_end(gui)) {
				gui->npt_cursor_pos = len;
			} else if (key_idx == KB_TAB) {
				if (flags & NPT_COMPLETE_ON_DEFOCUS)
					complete = true;
				gui__tab_focus_adjacent_widget(gui);
			}
		}
		if (mouse_pressed(gui, MB_LEFT) && !contains_mouse) {
			gui->focus_id = 0;
			SDL_StopTextInput();
			if (flags & NPT_COMPLETE_ON_DEFOCUS)
				complete = true;
		}
	} else if (gui->focus_next_widget && !gui->lock) {
		gui__on_widget_tab_focused(gui, id);
		SDL_StartTextInput();
		if (flags & NPT_CLEAR_ON_FOCUS)
			txt[0] = '\0';
		gui->npt_cursor_pos = (u32)strlen(txt);
	} else if (gui->focus_prev_widget_id == id) {
		if (gui->lock) {
			gui->focus_prev_widget_id = gui->prev_widget_id;
		} else {
			gui__on_widget_tab_focused(gui, id);
			SDL_StartTextInput();
			if (flags & NPT_CLEAR_ON_FOCUS)
				txt[0] = '\0';
			gui->npt_cursor_pos = (u32)strlen(txt);
		}
	} else if (gui->active_id == id) {
		if (mouse_released(gui, MB_LEFT)) {
			if (contains_mouse) {
				gui->focus_id = id;
				SDL_StartTextInput();
				if (flags & NPT_CLEAR_ON_FOCUS) {
					txt[0] = '\0';
					gui->npt_cursor_pos = 0;
				}
			}
			gui->active_id = 0;
		} else {
			gui->active_id_found_this_frame = true;
		}
	} else if (gui->hot_id == id) {
		if (!contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, MB_LEFT)) {
			const gui_text_style_t *style = &gui->style.npt.active.text;
			gui->active_id = id;
			gui->npt_cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, txt,
			                                         gui->mouse_pos, style);
			gui->hot_id = 0;
			gui->active_id_found_this_frame = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}

	const gui__widget_render_state_t render_state
		= gui__widget_render_state(gui, id, true, false, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.npt);
	gui->style.npt.pen(gui, x, y, w, h, &style);

	if (flags & NPT_PASSWORD) {
		const u32 sz = (u32)strlen(txt);
		array_reserve(gui->pw_buf, sz+1);
		for (u32 i = 0; i < sz; ++i)
			gui->pw_buf[i] = '*';
		gui->pw_buf[sz] = '\0';
		gui_txt_styled(gui, x, y, w, h, gui->pw_buf, &style.text);
		displayed_txt = gui->pw_buf;
	} else {
		gui_txt_styled(gui, x, y, w, h, txt, &style.text);
		displayed_txt = txt;
	}
	if (gui->focus_id == id) {
		gui->focus_id_found_this_frame = true;
		if (time_diff_milli(gui->creation_time, gui->frame_start_time) % 1000 < 500) {
			const color_t color = style.text.color;
			const font_t *font = gui__get_font(gui, style.text.size);
			gui__txt_char_pos(gui, &x, &y, w, h, displayed_txt,
			                  gui->npt_cursor_pos, &style.text);
			gui_line(gui, x + 1, y + font->descent, x + 1, y + font->ascent, 1, color);
		}
	} else if (hint && strlen(txt) == 0) {
		gui_txt_styled(gui, x, y, w, h, hint, &style.text);
	}
	gui->prev_widget_id = id;
	return complete;
}

static
btn_val_t gui__btn_logic(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         u64 id, b32 *contains_mouse)
{
	btn_val_t retval = BTN_NONE;
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	*contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                  && gui__box_half_visible(gui, box)
	                  && !gui->lock;

	if (gui->focus_id == id) {
		if (gui->lock) {
			gui->focus_id = 0;
		} else if (gui->key_repeat.triggered && gui->key_repeat.val == KB_TAB) {
			gui__tab_focus_adjacent_widget(gui);
		} else {
			gui->focus_id_found_this_frame = true;
			if (mouse_pressed(gui, ~0) && !*contains_mouse)
				gui->focus_id = 0;
			else if (   gui->key_repeat.triggered
			         && (   gui->key_repeat.val == KB_RETURN
			             || gui->key_repeat.val == KB_KP_ENTER))
				retval = key_pressed(gui, gui->key_repeat.val) ? BTN_PRESS : BTN_HOLD;
		}
	} else if (gui->focus_next_widget && !gui->lock) {
		gui__on_widget_tab_focused(gui, id);
	} else if (gui->focus_prev_widget_id == id) {
		if (gui->lock)
			gui->focus_prev_widget_id = gui->prev_widget_id;
		else
			gui__on_widget_tab_focused(gui, id);
	}

	if (gui->hot_id == id) {
		if (!*contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, MB_LEFT)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->active_id_found_this_frame = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui->active_id == id) {
		gui->active_id_found_this_frame = true;
		if (mouse_released(gui, MB_LEFT)) {
			if (*contains_mouse)
				retval = BTN_PRESS;
			gui->active_id = 0;
		} else if (*contains_mouse && gui->mouse_repeat.triggered) {
			retval = BTN_HOLD;
		}
	} else if (gui__allow_new_interaction(gui) && *contains_mouse) {
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

btn_val_t gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, id, &contains_mouse);
	const gui__widget_render_state_t render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.btn);
	return ret;
}

btn_val_t gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                      img_scale_t scale)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, id, &contains_mouse);
	const gui__widget_render_state_t render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, &style);
	gui_img_boxed(gui, x, y, w, h, fname, scale);
	return ret;
}

btn_val_t gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, id, &contains_mouse);
	const gui__widget_render_state_t render_state
		= gui__btn_render_state(gui, id, ret, contains_mouse);
	const gui_element_style_t style
		= gui__element_style(gui, render_state, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, &style);
	pen(gui, x, y, w, h, &style);
	return ret;
}

b32 gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, b32 *val)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	b32 toggled = false;
	gui__widget_render_state_t render_state;

	if (gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS) {
		*val = !*val;
		toggled = true;
	}

	render_state = gui__widget_render_state(gui, id, toggled, *val, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.chk);
	return toggled;
}

b32 gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen, b32 *val)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	b32 toggled = false;
	gui__widget_render_state_t render_state;
	gui_element_style_t style;

	if (gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS) {
		*val = !*val;
		toggled = true;
	}

	render_state = gui__widget_render_state(gui, id, toggled, *val, contains_mouse);
	style = gui__element_style(gui, render_state, &gui->style.chk);
	gui->style.chk.pen(gui, x, y, w, h, &style);
	pen(gui, x, y, w, h, &style);
	return toggled;
}

typedef enum gui__slider_orientation
{
	GUI__SLIDER_X,
	GUI__SLIDER_Y,
} gui__slider_orientation_t;

static
void gui__slider_move(s32 x, s32 y, s32 w, s32 h, s32 hnd_len, s32 pos,
                      gui__slider_orientation_t orientation, r32 *val)
{
	if (orientation == GUI__SLIDER_X) {
		const r32 min_x = x+hnd_len/2;
		const r32 max_x = x+w-hnd_len/2;
		*val = fmath_clamp(0, (pos-min_x)/(max_x-min_x), 1.f);
	} else {
		const r32 min_y = y+hnd_len/2;
		const r32 max_y = y+h-hnd_len/2;
		*val = fmath_clamp(0, (pos-min_y)/(max_y-min_y), 1.f);
	}
}

b32 gui__slider(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val, s32 hnd_len,
                gui__slider_orientation_t orientation)
{
	const u64 id = gui_widget_id(gui, x, y);
	box2i box;
	b32 contains_mouse;
	b32 moved_while_focused = false;

	assert(*val >= 0.f && *val <= 1.f);

	if (orientation == GUI__SLIDER_X) {
		box.min = (v2i){ .x=x+(w-hnd_len)**val, .y=y };
		box.max = (v2i){ .x=x+hnd_len+(w-hnd_len)**val, .y=y+h };
	} else {
		box.min = (v2i){ .x=x, .y=y+(h-hnd_len)**val };
		box.max = (v2i){ .x=x+w, .y=y+hnd_len+(h-hnd_len)**val };
	}
	contains_mouse = box2i_contains_point(box, gui->mouse_pos)
	              && gui__box_half_visible(gui, box)
	              && !gui->lock;

	if (gui->focus_id == id) {
		if (gui->lock) {
			gui->focus_id = 0;
		} else if (gui->key_repeat.triggered && gui->key_repeat.val == KB_TAB) {
			gui__tab_focus_adjacent_widget(gui);
		} else {
			gui->focus_id_found_this_frame = true;
			if (mouse_pressed(gui, ~0) && !contains_mouse) {
				gui->focus_id = 0;
			} else if (gui->key_repeat.triggered) {
				if (orientation == GUI__SLIDER_X) {
					const s32 mid = (box.min.x + box.max.x) / 2;
					const s32 dim = box.max.x - box.min.x;
					if (gui__key_left(gui)) {
						gui__slider_move(x, y, w, h, hnd_len, mid - dim, orientation, val);
						moved_while_focused = true;
					} else if (gui__key_right(gui)) {
						gui__slider_move(x, y, w, h, hnd_len, mid + dim, orientation, val);
						moved_while_focused = true;
					}
				} else {
					const s32 mid = (box.min.y + box.max.y) / 2;
					const s32 dim = box.max.y - box.min.y;
					if (gui__key_up(gui)) {
						gui__slider_move(x, y, w, h, hnd_len, mid + dim, orientation, val);
						moved_while_focused = true;
					} else if (gui__key_down(gui)) {
						gui__slider_move(x, y, w, h, hnd_len, mid - dim, orientation, val);
						moved_while_focused = true;
					}
				}
			}
		}
	} else if (gui->focus_next_widget && !gui->lock) {
		gui__on_widget_tab_focused(gui, id);
	} else if (gui->focus_prev_widget_id == id) {
		if (gui->lock)
			gui->focus_prev_widget_id = gui->prev_widget_id;
		else
			gui__on_widget_tab_focused(gui, id);
	}

	if (gui->hot_id == id) {
		if (!contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, MB_LEFT)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->active_id_found_this_frame = true;
			gui->drag_offset.x = box.min.x+(box.max.x-box.min.x)/2-gui->mouse_pos.x;
			gui->drag_offset.y = box.min.y+(box.max.y-box.min.y)/2-gui->mouse_pos.y;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui->active_id == id) {
		if (!mouse_released(gui, MB_LEFT)) {
			if (orientation == GUI__SLIDER_Y) {
				const r32 min_y = y+hnd_len/2;
				const r32 max_y = y+h-hnd_len/2;
				const s32 mouse_y = gui->mouse_pos.y+gui->drag_offset.y;
				*val = fmath_clamp(0, (mouse_y-min_y)/(max_y-min_y), 1.f);
			} else {
				const r32 min_x = x+hnd_len/2;
				const r32 max_x = x+w-hnd_len/2;
				const s32 mouse_x = gui->mouse_pos.x+gui->drag_offset.x;
				*val = fmath_clamp(0, (mouse_x-min_x)/(max_x-min_x), 1.f);
			}
			gui->active_id_found_this_frame = true;
		} else {
			gui->active_id = 0;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}

	const gui__widget_render_state_t render_state
		= gui__widget_render_state(gui, id, moved_while_focused, false, contains_mouse);
	const gui_element_style_t style_track =
		gui__element_style(gui, render_state, &gui->style.slider.track);
	const gui_element_style_t style_handle =
		gui__element_style(gui, render_state, &gui->style.slider.handle);
	if (orientation == GUI__SLIDER_Y) {
		if (gui->style.slider.track_narrow)
			gui->style.slider.track.pen(gui, x, y + hnd_len / 2, w, h - hnd_len,
			                            &style_track);
		else
			gui->style.slider.track.pen(gui, x, y, w, h, &style_track);
		gui->style.slider.handle.pen(gui, x, y + (h - hnd_len) * *val, w, hnd_len,
		                             &style_handle);
	} else {
		if (gui->style.slider.track_narrow)
			gui->style.slider.track.pen(gui, x + hnd_len / 2, y, w - hnd_len, h,
			                            &style_track);
		else
			gui->style.slider.track.pen(gui, x, y, w, h, &style_track);
		gui->style.slider.handle.pen(gui, x + (w - hnd_len) * *val, y, hnd_len, h,
		                             &style_handle);
	}
	gui->prev_widget_id = id;
	return gui->active_id == id;
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
	const u64 id = gui_widget_id(gui, x, y);
	const b32 was_selected = *val == opt;
	gui__widget_render_state_t render_state;
	b32 contains_mouse;
	b32 selected = false;

	if (   gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS
	    && !was_selected) {
		*val = opt;
		selected = true;
	}

	render_state = gui__widget_render_state(gui, id, selected, *val == opt,
	                                        contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, render_state, &gui->style.select);
	return selected;
}

void gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                 const char *txt, u32 *val, u32 opt)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	b32 selected = false;
	gui__widget_render_state_t render_state;

	if (gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS) {
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
}

void gui_dropdown_begin(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        u32 *val, u32 num_items)
{
	const u64 id = gui_widget_id(gui, x, y);
	const b32 render_items = gui->focus_id == id;
	b32 contains_mouse, triggered_by_key = false;
	gui__widget_render_state_t render_state;
	box2i box;

	assert(gui->current_dropdown.num_items == 0); /* cannot nest dropdown menus */

	box2i_from_dims(&box, x, y+h, x+w, y);
	contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                 && gui__box_half_visible(gui, box)
	                 && !gui->lock;

	if (gui->focus_id == id) {
		if (   gui->lock
		    || (mouse_released(gui, MB_LEFT) && contains_mouse)) {
			gui__defocus_dropdown(gui);
			gui->focus_id = 0;
		} else if (gui->key_repeat.triggered && gui->key_repeat.val == KB_TAB) {
			gui__defocus_dropdown(gui);
			gui__tab_focus_adjacent_widget(gui);
		} else {
			gui->focus_id_found_this_frame = true;
			if (gui->key_repeat.triggered) {
				if (gui__key_up(gui) && *val > 0) {
					--*val;
					triggered_by_key = true;
				} else if (gui__key_down(gui) && *val + 1 < num_items) {
					++*val;
					triggered_by_key = true;
				}
			}
		}
	} else if (gui->focus_next_widget && !gui->lock) {
		gui__on_widget_tab_focused(gui, id);
	} else if (gui->focus_prev_widget_id == id) {
		if (gui->lock)
			gui->focus_prev_widget_id = gui->prev_widget_id;
		else
			gui__on_widget_tab_focused(gui, id);
	}

	if (gui->hot_id == id) {
		if (!contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, MB_LEFT)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->active_id_found_this_frame = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui->active_id == id) {
		gui->active_id_found_this_frame = true;
		if (mouse_released(gui, MB_LEFT)) {
			if (contains_mouse) {
				gui->focus_id = id;
				gui->focus_id_found_this_frame = true;
			}
			gui->active_id = 0;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}
	gui->prev_widget_id = id;

	/* store current dropdown state */
	gui->current_dropdown.id               = id;
	gui->current_dropdown.x                = x;
	gui->current_dropdown.y                = y;
	gui->current_dropdown.w                = w;
	gui->current_dropdown.h                = h;
	gui->current_dropdown.val              = *val;
	gui->current_dropdown.num_items        = num_items;
	gui->current_dropdown.item_idx         = 0;
	gui->current_dropdown.triggered_by_key = triggered_by_key;
	gui->current_dropdown.contains_mouse   = contains_mouse;

	/* render appropriate label in gui_dropdown_item */
	render_state = gui__widget_render_state(gui, id, triggered_by_key,
	                                        false, contains_mouse);
	gui__btn_render(gui, x, y, w, h, "", render_state, &gui->style.dropdown);
	{
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
	}

	if (gui->focus_id == id) {
		gui->focused_dropdown.id               = id;
		gui->focused_dropdown.x                = x;
		gui->focused_dropdown.y                = y;
		gui->focused_dropdown.w                = w;
		gui->focused_dropdown.h                = h;
		gui->focused_dropdown.val              = val;
		gui->focused_dropdown.num_items        = num_items;
		gui->focused_dropdown.item_idx         = 0;
		gui->focused_dropdown.render_items     = render_items;
		gui->focused_dropdown.close_at_end     = false;
		if (!render_items)
			gui->focused_dropdown.selected_item_txt[0] = '\0';
	}
}

static
void gui__dropdown_selected_text(gui_t *gui, const char *txt)
{
	const u64 id               = gui->current_dropdown.id;
	s32 x                      = gui->current_dropdown.x;
	s32 w                      = gui->current_dropdown.w;
	const s32 y                = gui->current_dropdown.y;
	const s32 h                = gui->current_dropdown.h;
	const b32 triggered_by_key = gui->current_dropdown.triggered_by_key;
	const b32 contains_mouse   = gui->current_dropdown.contains_mouse;
	gui__widget_render_state_t render_state;
	gui_element_style_t style;
	render_state = gui__widget_render_state(gui, id, triggered_by_key,
	                                        false, contains_mouse);
	style = gui__element_style(gui, render_state, &gui->style.dropdown);
	if (style.text.align & GUI_ALIGN_LEFT) {
		w -= h;
	} else if (style.text.align & GUI_ALIGN_RIGHT) {
		x += h;
		w -= h;
	}
	gui_txt_styled(gui, x, y, w, h, txt, &style.text);
}

b32 gui_dropdown_item(gui_t *gui, const char *txt)
{
	b32 chosen = false;

	assert(gui->current_dropdown.num_items != 0);

	/* cache item text so it can be rendered in the pre-dropdown mask */
	if (   gui->current_dropdown.id == gui->focused_dropdown.id
	    && gui->current_dropdown.val == gui->current_dropdown.item_idx) {
		const size_t n = countof(gui->focused_dropdown.selected_item_txt);
		strncpy(gui->focused_dropdown.selected_item_txt, txt, n);
		gui->focused_dropdown.selected_item_txt[n-1] = '\0';
	}

	/* render item button for focused dropdown */
	if (   gui->current_dropdown.id == gui->focused_dropdown.id
	    && gui->focused_dropdown.render_items) {
		const s32 w = gui->focused_dropdown.w;
		const s32 h = gui->focused_dropdown.h;
		const s32 x = gui->focused_dropdown.x;
		const s32 y = gui->focused_dropdown.y
		            - (s32)(gui->focused_dropdown.item_idx + 1) * h;

		assert(gui->focused_dropdown.num_items != 0);

		/* switch to reserved first scissor before first item */
		if (gui->focused_dropdown.item_idx == 0) {
			const s32 mask_h = gui->focused_dropdown.num_items
			                 * gui->focused_dropdown.h;
			const s32 mask_y = gui->focused_dropdown.y - mask_h;
			gui->focused_dropdown.prev_mask.x = gui->scissors[gui->scissor_idx].x;
			gui->focused_dropdown.prev_mask.y = gui->scissors[gui->scissor_idx].y;
			gui->focused_dropdown.prev_mask.w = gui->scissors[gui->scissor_idx].w;
			gui->focused_dropdown.prev_mask.h = gui->scissors[gui->scissor_idx].h;
			gui__dropdown_selected_text(gui, gui->focused_dropdown.selected_item_txt);
			gui__complete_scissor(gui);
			gui__mask(gui, 0, x, mask_y, w, mask_h);
		}

		/* mouse_covered_by_panel set by dropdown handling in gui_begin_frame */
		gui->mouse_covered_by_panel = false;
		gui_style_push(gui, btn, gui_style(gui)->dropdown);
		if (gui_btn_txt(gui, x, y, w, h, txt) == BTN_PRESS) {
			*gui->focused_dropdown.val = gui->current_dropdown.item_idx;
			gui->focused_dropdown.close_at_end = true;
			chosen = true;
		} else if (   gui->current_dropdown.triggered_by_key
		           && *gui->focused_dropdown.val == gui->current_dropdown.item_idx) {
			chosen = true;
		}
		gui_style_pop(gui);
		gui->mouse_covered_by_panel = true;

		/* switch to new scissor (mirroring interrupted scissor) after last item */
		if (++gui->focused_dropdown.item_idx == gui->focused_dropdown.num_items) {
			gui_mask(gui, gui->focused_dropdown.prev_mask.x,
			         gui->focused_dropdown.prev_mask.y,
			         gui->focused_dropdown.prev_mask.w,
			         gui->focused_dropdown.prev_mask.h);
		  if (gui->focused_dropdown.close_at_end) {
				gui__defocus_dropdown(gui);
				gui->focus_id = 0;
			}
		}
	} else if (gui->current_dropdown.val == gui->current_dropdown.item_idx) {
		gui__dropdown_selected_text(gui, txt);
	}

	/* end current dropdown list */
	if (++gui->current_dropdown.item_idx == gui->current_dropdown.num_items)
		memclr(gui->current_dropdown);

	return chosen;
}

static
b32 gui__drag(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, mouse_button_t mb,
              u64 *id, gui_drag_callback_t cb, void *udata)
{
	b32 retval = false;
	*id = gui_widget_id(gui, *x, *y);

	if (gui->focus_id == *id) {
		if (gui->lock) {
			gui->focus_id = 0;
		} if (gui->key_repeat.triggered && gui->key_repeat.val == KB_TAB) {
			gui__tab_focus_adjacent_widget(gui);
		} else {
			gui->focus_id_found_this_frame = true;
			if (mouse_pressed(gui, ~0) && !contains_mouse) {
				gui->focus_id = 0;
			} else if (gui->key_repeat.triggered) {
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
					*id = gui_widget_id(gui, *x, *y);
					gui->focus_id = *id;
					retval = true;
				}
			}
		}
	} else if (gui->focus_next_widget && !gui->lock) {
		gui__on_widget_tab_focused(gui, *id);
	} else if (gui->focus_prev_widget_id == *id) {
		if (gui->lock)
			gui->focus_prev_widget_id = gui->prev_widget_id;
		else
			gui__on_widget_tab_focused(gui, *id);
	}

	if (gui->hot_id == *id) {
		if (!contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, mb)) {
			gui->hot_id = 0;
			gui->active_id = *id;
			gui->active_id_found_this_frame = true;
			gui->drag_offset.x = *x - gui->mouse_pos.x;
			gui->drag_offset.y = *y - gui->mouse_pos.y;
			retval = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui->active_id == *id) {
		cb(x, y, gui->mouse_pos.x, gui->mouse_pos.y,
		   gui->drag_offset.x, gui->drag_offset.y, udata);
		*id = gui_widget_id(gui, *x, *y);
		retval = true;
		if (mouse_released(gui, mb)) {
			gui->active_id = 0;
		} else {
			gui->active_id = *id;
			gui->active_id_found_this_frame = true;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = *id;
		gui->hot_id_found_this_frame = true;
	}
	gui->prev_widget_id = *id;
	return retval;
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
}

static
void gui__drag_default_callback(s32 *x, s32 *y, s32 mouse_x, s32 mouse_y,
                                s32 offset_x, s32 offset_y, void *udata)
{
	*x = mouse_x + offset_x;
	*y = mouse_y + offset_y;
}

b32 gui_drag(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb)
{
	return gui_dragx(gui, x, y, w, h, mb, gui__drag_default_callback, NULL);
}

b32 gui__drag_logic(gui_t *gui, u64 *id, s32 *x, s32 *y, u32 w, u32 h,
                    mouse_button_t mb, gui_drag_callback_t cb, void *udata)
{
	box2i box;
	b32 contains_mouse;

	box2i_from_dims(&box, *x, *y+h, *x+w, *y);
	contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                 && gui__box_half_visible(gui, box)
	                 && !gui->lock;
	return gui__drag(gui, x, y, contains_mouse, mb, id, cb, udata);
}

b32 gui_dragx(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb,
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

b32 gui_drag_horiz(gui_t *gui, s32 *x, s32 y, u32 w, u32 h, mouse_button_t mb)
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

b32 gui_drag_vert(gui_t *gui, s32 x, s32 *y, u32 w, u32 h, mouse_button_t mb)
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

b32 gui_cdrag(gui_t *gui, s32 *x, s32 *y, u32 r, mouse_button_t mb)
{
	return gui_cdragx(gui, x, y, r, mb, gui__drag_default_callback, NULL);
}

b32 gui_cdragx(gui_t *gui, s32 *x, s32 *y, u32 r, mouse_button_t mb,
              gui_drag_callback_t cb, void *udata)
{
	const v2i pos = { *x, *y };
	u64 id;
	box2i box;
	box2i_from_center(&box, pos, (v2i){r, r});
	const b32 contains_mouse =    (u32)v2i_dist_sq(pos, gui->mouse_pos) < r * r
	                           && gui__box_half_visible(gui, box)
	                           && !gui->lock;
	const b32 ret = gui__drag(gui, x, y, contains_mouse, mb, &id, cb, udata);
	gui_style_push_pen(gui, drag.pen, gui_pen_circ);
	gui__drag_render(gui, *x - r, *y - r, 2 * r, 2 * r, id, ret, &gui->style.drag);
	gui_style_pop(gui);
	return ret;
}

#define GUI__DEFAULT_PANEL_ID UINT_MAX
#define GUI__SCISSOR_PANEL_ID (UINT_MAX-1)
#define GUI__MAX_PANEL_ID     (UINT_MAX-2)

u64 gui_widget_id(const gui_t *gui, s32 x, s32 y)
{
	if (   gui->current_dropdown.num_items != 0
	    && gui->current_dropdown.id == gui->focused_dropdown.id)
		return (((u64)x) << 48) | (((u64)y) << 32) | GUI__SCISSOR_PANEL_ID;
	else if (gui->panel)
		return (((u64)x) << 48) | (((u64)y) << 32) | gui->panel->id;
	else
		return (((u64)x) << 48) | (((u64)y) << 32) | GUI__DEFAULT_PANEL_ID;
}

void gui_widget_focus_next(gui_t *gui)
{
	if (   gui->focus_id == 0
	    && !gui->focus_next_widget
	    && gui->focus_prev_widget_id == 0)
		gui->focus_next_widget = true;
}

b32 gui_widget_active(const gui_t *gui, u64 id)
{
	return gui->active_id == id;
}

b32 gui_widget_focused(const gui_t *gui, u64 id)
{
	return gui->focus_id == id;
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
	return gui->focus_id != 0;
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

void gui__split_init(gui_t *gui, gui_split_t *split,
                     gui_split_t *sp1, r32 sz,
                     gui_split_t *sp2, gui_split_flags_t flags,
                     b32 vert)
{
	if (!gui->root_split) {
		gui->root_split = split;
		split->parent = NULL;
	} else {
		assert(split->parent);
	}
	split->flags      = flags;
	split->vert       = vert;
	split->leaf       = false;
	split->sp1        = sp1;
	split->sp2        = sp2;
	split->sz         = sz;
	split->default_sz = sz;
	sp1->parent       = split;
	sp2->parent       = split;
	sp1->leaf         = true;
	sp2->leaf         = true;
}

void gui_split_init(gui_t *gui, gui_split_t *split,
                    gui_split_t *sp1, r32 sz, gui_split_t *sp2,
                    gui_split_flags_t flags)
{
	gui__split_init(gui, split, sp1, sz, sp2, flags, false);
}

void gui_vsplit_init(gui_t *gui, gui_split_t *split,
                     gui_split_t *sp1, r32 sz, gui_split_t *sp2,
                     gui_split_flags_t flags)
{
	gui__split_init(gui, split, sp1, sz, sp2, flags, true);
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
void gui__split(gui_t *gui, gui_split_t *split, b32 render)
{
	s32 x, y, w, h, sz1;

	assert(split);
	assert(!split->leaf);

	box2i_to_xywh(split->box, &x, &y, &w, &h);

	/* checking use_default_cursor ensures only 1 resize happens per frame */
	if (   (split->flags & GUI_SPLIT_RESIZABLE)
	    && gui->use_default_cursor
	    && render) {
		gui_style_push(gui, drag.hot.bg_color, gui->style.drag.inactive.bg_color);
		gui_style_push(gui, drag.active.bg_color, gui->style.drag.inactive.bg_color);
		if (split->vert) {
			const s32 orig_drag_x =   x - GUI_SPLIT_RESIZE_BORDER / 2
			                        + gui__split_dim(split->sz, w);
			s32 drag_x = orig_drag_x;
			if (gui__resize_horiz(gui, &drag_x, y, GUI_SPLIT_RESIZE_BORDER, h)) {
				if (fabsf(split->sz) > 1.f)
					split->sz += drag_x - orig_drag_x;
				else
					split->sz = (r32)(drag_x + GUI_SPLIT_RESIZE_BORDER / 2 - x) / w;
				split->default_sz = split->sz;
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
			}
		}
		gui_style_pop(gui);
		gui_style_pop(gui);
	}

	sz1 = gui__split_dim(split->sz, split->vert ? w : h);
	split->sp1->box = split->box;
	split->sp2->box = split->box;

	if (split->vert) {
		const s32 xm = split->box.min.x + sz1;
		split->sp1->box.max.x = xm;
		split->sp2->box.min.x = xm;
		if (render)
			gui_line_styled(gui, xm, split->box.min.y, xm, split->box.max.y,
			                &gui->style.split);
	} else {
		const s32 ym = split->box.max.y - sz1;
		split->sp1->box.min.y = ym;
		split->sp2->box.max.y = ym;
		if (render)
			gui_line_styled(gui, split->box.min.x, ym, split->box.max.x, ym,
			                &gui->style.split);
	}

	if (!split->sp1->leaf)
		gui__split(gui, split->sp1, render);
	if (!split->sp2->leaf)
		gui__split(gui, split->sp2, render);
}

void gui_compute_split_boxes(gui_t *gui)
{
	assert(gui->root_split);
	box2i_from_center(&gui->root_split->box, gui->win_halfdim, gui->win_halfdim);
	gui__split(gui, gui->root_split, false);
}

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_t flags)
{
	panel->x = x;
	panel->y = y;
	panel->width = w;
	panel->height = h;
	panel->id = ++gui->next_panel_id;
	assert(gui->next_panel_id <= GUI__MAX_PANEL_ID);

	panel->title = title;
	panel->flags = flags;
	assert(!(flags & GUI_PANEL_TITLEBAR) || title);

	panel->grid.depth = 0;

	panel->scroll = g_v2i_zero;
	panel->scroll_rate.x = GUI_SCROLL_RATE;
	panel->scroll_rate.y = GUI_SCROLL_RATE;
	panel->required_dim = g_v2i_zero;

	panel->parent = NULL;
	panel->prev   = NULL;
	panel->next   = NULL;
	panel->split.leaf = false;

	panel->closed = false;
	panel->collapsed = false;
	panel->tabbed_out = false;

	pgui_panel_to_front(gui, panel);
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
	    && !panel->split.leaf
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
	if (panel->split.leaf && panel->split.parent) {
		/* TODO(rgriege): allow collapsing second child in split */
		gui_split_t *parent = panel->split.parent;
		assert(&panel->split == parent->sp1);
		parent->sz = panel->collapsed
		           ? GUI_PANEL_TITLEBAR_HEIGHT : parent->default_sz;
	}
}

static
void pgui__panel_titlebar(gui_t *gui, gui_panel_t *panel, b32 *dragging)
{
	if (panel->flags & GUI_PANEL_TITLEBAR) {
		const s32 dim = GUI_PANEL_TITLEBAR_HEIGHT;
		u64 drag_id = ~0;
		s32 rw, rx;
		s32 tab_count;

		panel->grid.pos.y = panel->y + panel->height - dim;

		if ((panel->flags & GUI_PANEL_DRAGGABLE) && !panel->split.leaf) {
			*dragging = gui__drag_logic(gui, &drag_id, &panel->x, &panel->grid.pos.y,
			                            dim, dim, MB_LEFT,
			                            gui__drag_default_callback, NULL);
			if (*dragging)
				panel->y = panel->grid.pos.y - panel->height + dim;
		} else {
			*dragging = false;
		}

		gui_rect(gui, panel->x, panel->grid.pos.y, panel->width,
		         dim, gui->style.panel.titlebar.bg_color,
		         gui->style.panel.titlebar.outline_color);

		gui__drag_render(gui, panel->x, panel->grid.pos.y, dim, dim, drag_id,
		                 *dragging, &gui->style.panel.drag);

		rw = 0;
		if (panel->flags & GUI_PANEL_COLLAPSABLE)
			rw += dim;
		if ((panel->flags & GUI_PANEL_CLOSABLE) && !panel->split.leaf)
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
				if (gui_select(gui, panel->x + dim + tab_idx * tab_dim, panel->grid.pos.y,
				               tab_dim, dim, p->title, &selected_tab_idx, (u32)tab_idx)) {
					for (gui_panel_t *px = panel; px; px = px->next)
						px->tabbed_out = true;
					p->tabbed_out = false;
				}
				++tab_idx;
			}
			gui_style_pop(gui);
		} else {
			gui_txt_styled(gui, panel->x + dim, panel->grid.pos.y, panel->width, dim,
			               panel->title, &gui->style.panel.titlebar.text);
		}

		rx = panel->x + panel->width - rw;
		if (panel->flags & GUI_PANEL_COLLAPSABLE) {
			const s32 y = panel->grid.pos.y;
			if (panel->collapsed)
				gui_style_push(gui, btn, gui->style.panel.restore);
			else
				gui_style_push(gui, btn, gui->style.panel.collapse);
			if (gui_btn_pen(gui, rx, y, dim, dim, gui->style.btn.pen) == BTN_PRESS)
				pgui__panel_collapse_toggle(panel);
			gui_style_pop(gui);
			rx += dim;
		}

		if ((panel->flags & GUI_PANEL_CLOSABLE) && !panel->split.leaf) {
			const s32 y = panel->grid.pos.y;
			gui_style_push(gui, btn, gui->style.panel.close);
			if (gui_btn_pen(gui, rx, y, dim, dim, gui->style.btn.pen) == BTN_PRESS)
				panel->closed = true;
			gui_style_pop(gui);
			rx += dim;
		}

		panel->grid.pos.y -= panel->padding.y;
		panel->body_height = panel->height - dim;
	} else {
		*dragging = false;
		panel->grid.pos.y = panel->y + panel->height - panel->padding.y;
		panel->body_height = panel->height;
	}
}

b32 pgui_panel(gui_t *gui, gui_panel_t *panel)
{
	b32 dragging;

	if (panel->prev) {
		gui_panel_t *first = panel->prev;
		while (first->prev)
			first = first->prev;

		panel->x           = first->x;
		panel->y           = first->y;
		panel->width       = first->width;
		panel->height      = first->height;
		panel->grid.pos.y  = first->grid.pos.y;
		panel->padding     = first->padding;
		panel->body_height = first->body_height;
		panel->parent      = first->parent;
		memcpy(&panel->split, &first->split, sizeof(first->split));
		panel->closed      = first->closed;
		panel->collapsed   = first->collapsed;

		gui->panel = panel;
	} else {
		assert(panel->parent == gui->panel);

		if (panel->closed)
			return false;

		gui->panel = panel;

		if (panel->split.leaf)
			box2i_to_xywh(panel->split.box, &panel->x, &panel->y,
			              &panel->width, &panel->height);

		if (gui->style.panel.padding >= 1.f) {
			panel->padding.x = gui->style.panel.padding;
			panel->padding.y = gui->style.panel.padding;
		} else {
			panel->padding.x = panel->width  * gui->style.panel.padding;
			panel->padding.y = panel->height * gui->style.panel.padding;
		}

		pgui__panel_resize(gui, panel);
		pgui__panel_titlebar(gui, panel, &dragging);

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

	if (panel->closed || panel->collapsed || panel->tabbed_out) {
		gui->panel = panel->parent;
		return false;
	}

	/* background outline display */
	gui_rect(gui, panel->x, panel->y, panel->width, panel->height,
	         g_nocolor, gui->style.panel.border_color);

	panel->grid.pos.x = panel->x + panel->padding.x;
	panel->grid.start = panel->grid.pos;
	panel->grid.depth = 0;

	panel->required_dim = v2i_scale(panel->padding, 2);

	{
		box2i box;
		box2i_from_xywh(&box, panel->x + panel->padding.x,
		                panel->y + panel->padding.y,
		                panel->width - panel->padding.x * 2,
		                panel->body_height - panel->padding.y * 2);
		gui__mask_box(gui, &box);
		gui_mask(gui, box.min.x, box.min.y, box.max.x - box.min.x, box.max.y - box.min.y);
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

void pgui_panel_finish(gui_t *gui, gui_panel_t *panel)
{
	b32 contains_mouse;

	assert(gui->panel == panel);

	if (panel->collapsed || panel->closed) {
		contains_mouse = gui__current_panel_contains_mouse(gui);
		goto out;
	}

	/* NOTE(rgriege): would be great to avoid the additional mask here */
	if (panel->parent)
		gui_mask(gui, panel->parent->x + panel->parent->padding.x,
		         panel->parent->y + panel->parent->padding.y,
		         panel->parent->width - panel->parent->padding.x * 2,
		         panel->parent->body_height - panel->parent->padding.y * 2);
	else
		gui_unmask(gui);

	/* background display */
	gui_rect(gui, panel->x, panel->y, panel->width, panel->height,
	         gui->style.panel.bg_color, g_nocolor);

	/* scrolling */
	contains_mouse = gui__current_panel_contains_mouse(gui);
	if (panel->body_height < panel->required_dim.y) {
		const s32 needed = panel->required_dim.y - panel->body_height;
		if (   !key_mod(gui, KBM_SHIFT)
		    && !gui->mouse_covered_by_panel
		    && contains_mouse) {
			s32 scroll;
			mouse_scroll(gui, &scroll);
			scroll *= -panel->scroll_rate.y;
			panel->scroll.y = clamp(0, panel->scroll.y + scroll, needed);
		} else {
			panel->scroll.y = clamp(0, panel->scroll.y, needed);
		}
		if (panel->flags & GUI_PANEL_SCROLLBARS) {
			const s32 scroll_handle_sz
				= max(panel->padding.x / 2,   panel->body_height * panel->body_height
				                            / panel->required_dim.y);
			r32 slider_val = 1.f - ((r32)panel->scroll.y / needed);
			gui_style_push(gui, slider, gui->style.panel.scrollbar);
			gui__slider(gui, panel->x, panel->y + panel->padding.y / 2, panel->padding.x / 2,
			            panel->body_height - panel->padding.y / 2, &slider_val,
			            scroll_handle_sz, GUI__SLIDER_Y);
			gui_style_pop(gui);
			panel->scroll.y = -(slider_val - 1.f) * needed;
		}
	} else {
		panel->scroll.y = 0;
	}

	if (panel->width < panel->required_dim.x) {
		const s32 needed = panel->required_dim.x - panel->width;
		if (   key_mod(gui, KBM_SHIFT)
		    && !gui->mouse_covered_by_panel
		    && contains_mouse) {
			s32 scroll;
			mouse_scroll(gui, &scroll);
			scroll *= -panel->scroll_rate.x;
			panel->scroll.x = -clamp(0, -panel->scroll.x + scroll, needed);
		} else {
			panel->scroll.x = -clamp(0, -panel->scroll.x, needed);
		}
		if (panel->flags & GUI_PANEL_SCROLLBARS) {
			const s32 scroll_handle_sz
				= max(panel->padding.y / 2,   panel->width * panel->width
				                            / panel->required_dim.x);
			r32 slider_val = -(r32)panel->scroll.x / needed;
			gui_style_push(gui, slider, gui->style.panel.scrollbar);
			gui__slider(gui, panel->x + panel->padding.x / 2, panel->y,
									panel->width - panel->padding.x / 2, panel->padding.y / 2, &slider_val,
									scroll_handle_sz, GUI__SLIDER_X);
			gui_style_pop(gui);
			panel->scroll.x = -slider_val * needed;
		}
	} else {
		panel->scroll.x = 0;
	}

	if (panel->parent)
		gui_mask(gui, panel->parent->x + panel->parent->padding.x,
		         panel->parent->y + panel->parent->padding.y,
		         panel->parent->width - panel->parent->padding.x * 2,
		         panel->parent->body_height - panel->parent->padding.y * 2);
	else
		gui_unmask(gui);

out:
	if (panel->flags & GUI_PANEL_TITLEBAR)
		panel->body_height = panel->height - GUI_PANEL_TITLEBAR_HEIGHT;
	else
		panel->body_height = panel->height;

	if (contains_mouse && !panel->parent)
		gui->mouse_covered_by_panel = true;

	gui->panel = panel->parent;
}

void pgui_row(gui_t *gui, r32 height, u32 num_cells)
{
	const r32 cells[GUI_PANEL_MAX_GRID_CELLS] = { 0 };
	static_assert(GUI_GRID_REMAINING == 0, invalid_initialization);
	assert(num_cells < GUI_PANEL_MAX_GRID_CELLS);
	pgui_row_cells(gui, height, cells, num_cells);
}

static
void pgui__strip(gui_t *gui, b32 vertical, r32 minor_dim_,
                 const r32 *cells, u32 num_cells)
{
	gui_panel_t *panel = gui->panel;
	b32 major_dim_inherited;
	u32 unspecified_cell_cnt;
	s32 cell_total_major_dim;
	gui_panel_grid_strip_t *strip;

	assert(panel);
	assert(panel->grid.depth < GUI_PANEL_MAX_GRID_DEPTH);
	assert((minor_dim_ == GUI_GRID_MINOR_DIM_INHERITED) == (panel->grid.depth > 0));
	assert(   panel->grid.depth == 0
	       || panel->grid.strips[panel->grid.depth-1].vertical != vertical);

	strip = &panel->grid.strips[panel->grid.depth];

	if (minor_dim_ == GUI_GRID_MINOR_DIM_INHERITED) {
		pgui_cell(gui, NULL, NULL, &strip->dim.x, &strip->dim.y);
		major_dim_inherited = true;
	} else {
		r32 major_dim = 0.f;
		strip->dim.d[!vertical] = minor_dim_;
		major_dim_inherited = false;
		for (u32 i = 0; i < num_cells; ++i) {
			if (cells[i] > 1.f)
				major_dim += cells[i];
			else
				major_dim_inherited = true;
		}
		if (major_dim_inherited) {
			if (vertical)
				strip->dim.y = panel->grid.pos.y - panel->y - panel->padding.y;
			else
				strip->dim.x = panel->x + panel->width - panel->padding.x - panel->grid.pos.x;
			if (strip->dim.d[vertical] < major_dim)
				strip->dim.d[vertical] = major_dim;
		} else {
			strip->dim.d[vertical] = major_dim;
		}
	}

	strip->vertical = vertical;
	strip->num_cells = num_cells;
	strip->current_cell =   panel->grid.depth > 0
	                      ? panel->grid.strips[panel->grid.depth-1].max_cell
	                      : panel->grid.cells;
	strip->max_cell = strip->current_cell + num_cells;

	assert(strip->max_cell < panel->grid.cells + GUI_PANEL_MAX_GRID_CELLS);

	cell_total_major_dim = 0;
	unspecified_cell_cnt = 0;
	for (u32 i = 0; i < num_cells; ++i) {
		if (cells[i] > 1.f) {
			strip->current_cell[i] = cells[i];
			cell_total_major_dim += cells[i];
		} else if (cells[i] > 0.f) {
			const u32 dim = cells[i] * strip->dim.d[vertical];
			strip->current_cell[i] = dim;
			cell_total_major_dim += dim;
		} else {
			assert(cells[i] == 0.f);
			strip->current_cell[i] = 0.f;
			++unspecified_cell_cnt;
		}
	}
	if (unspecified_cell_cnt) {
		const u32 total = max((strip->dim.d[vertical] - cell_total_major_dim), 0);
		const u32 dim = total / unspecified_cell_cnt;
		const u32 leftover = total % unspecified_cell_cnt;
		u32 leftover_aggregate = 0, leftover_distributions = 1;
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

	if (panel->grid.depth == 0) {
		const v2i dim = {
			.x =   panel->grid.pos.x + strip->dim.x + panel->padding.x
			     - (panel->grid.start.x - panel->padding.x),
			.y =   panel->grid.start.y + panel->padding.y
			     - (panel->grid.pos.y - strip->dim.y - panel->padding.y),
		};
		panel->required_dim.x = max(panel->required_dim.x, dim.x);
		panel->required_dim.y = max(panel->required_dim.y, dim.y);
	}
	++panel->grid.depth;
}

void pgui_row_cells(gui_t *gui, r32 height, const r32 *cells, u32 num_cells)
{
	pgui__strip(gui, false, height, cells, num_cells);
}

static
void pgui__cell_advance(gui_panel_t *panel)
{
	gui_panel_grid_strip_t *strip;

	assert(panel->grid.depth > 0);

	do {
		strip = &panel->grid.strips[panel->grid.depth - 1];
		if (strip->vertical)
			panel->grid.pos.y -= *strip->current_cell;
		else
			panel->grid.pos.x += *strip->current_cell;
		++strip->current_cell;
		if (strip->current_cell == strip->max_cell) {
			--panel->grid.depth;
			if (strip->vertical)
				panel->grid.pos.y += strip->dim.y;
			else
				panel->grid.pos.x -= strip->dim.x;
		} else {
			break;
		}
	} while (panel->grid.depth > 0);

	if (panel->grid.depth == 0) {
		strip = &panel->grid.strips[0];
		if (strip->vertical)
			panel->grid.pos.x += strip->dim.x;
		else
			panel->grid.pos.y -= strip->dim.y;
	}
}

static
void pgui__cell_consume(gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	pgui_cell(gui, x, y, w, h);
	pgui__cell_advance(gui->panel);
}

void pgui_row_empty(gui_t *gui, r32 height)
{
	pgui_row(gui, height, 1);
	pgui__cell_advance(gui->panel);
}

void pgui_col(gui_t *gui, r32 width, u32 num_cells)
{
	const r32 cells[GUI_PANEL_MAX_GRID_CELLS] = { 0 };
	static_assert(GUI_GRID_REMAINING == 0, invalid_initialization);
	assert(num_cells < GUI_PANEL_MAX_GRID_CELLS);
	pgui_col_cells(gui, width, cells, num_cells);
}

void pgui_col_cells(gui_t *gui, r32 width, const r32 *cells, u32 num_cells)
{
	pgui__strip(gui, true, width, cells, num_cells);
}

void pgui_col_empty(gui_t *gui, r32 width)
{
	pgui_col(gui, width, 1);
	pgui__cell_advance(gui->panel);
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
	pgui__cell_advance(gui->panel);
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

btn_val_t pgui_btn_txt(gui_t *gui, const char *lbl)
{
	btn_val_t result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_txt(gui, x, y, w, h, lbl);
	return result;
}

btn_val_t pgui_btn_img(gui_t *gui, const char *fname, img_scale_t scale)
{
	btn_val_t result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_img(gui, x, y, w, h, fname, scale);
	return result;
}

btn_val_t pgui_btn_pen(gui_t *gui, gui_pen_t pen)
{
	btn_val_t result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_btn_pen(gui, x, y, w, h, pen);
	return result;
}

b32 pgui_chk(gui_t *gui, const char *lbl, b32 *val)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	return gui_chk(gui, x, y, w, h, lbl, val);
}

b32 pgui_npt(gui_t *gui, char *lbl, u32 n, const char *hint, npt_flags_t flags)
{
	return pgui_npt_chars(gui, lbl, n, hint, flags, g_gui_npt_chars_print);
}

b32 pgui_npt_chars(gui_t *gui, char *lbl, u32 n, const char *hint,
                   npt_flags_t flags, const b32 chars[128])
{
	b32 result;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	result = gui_npt_chars(gui, x, y, w, h, lbl, n, hint, flags, chars);
	return result;
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

void pgui_dropdown_begin(gui_t *gui, u32 *val, u32 num_items)
{
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	gui_dropdown_begin(gui, x, y, w, h, val, num_items);
}

b32 pgui_dropdown_item(gui_t *gui, const char *txt)
{
	return gui_dropdown_item(gui, txt);
}

b32 pgui_slider_x(gui_t *gui, r32 *val)
{
	b32 ret;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	ret = gui_slider_x(gui, x, y, w, h, val);
	return ret;
}

b32 pgui_slider_y(gui_t *gui, r32 *val)
{
	b32 ret;
	s32 x, y, w, h;
	pgui__cell_consume(gui, &x, &y, &w, &h);
	ret = gui_slider_y(gui, x, y, w, h, val);
	return ret;
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


void pgui_subpanel_init(gui_t *gui, gui_panel_t *subpanel)
{
	pgui_panel_init(gui, subpanel, 0, 0, 0, 0, NULL, 0);
}

void pgui_subpanel(gui_t *gui, gui_panel_t *subpanel)
{
	assert(!subpanel->split.leaf);
	assert(!(subpanel->flags & GUI_PANEL_COLLAPSABLE));
	assert(!(subpanel->flags & GUI_PANEL_CLOSABLE));
	pgui__cell_consume(gui, &subpanel->x, &subpanel->y,
	                   &subpanel->width, &subpanel->height);
	subpanel->parent = gui->panel;
	check(pgui_panel(gui, subpanel));
}

void pgui_subpanel_finish(gui_t *gui, gui_panel_t *subpanel)
{
	pgui_panel_finish(gui, subpanel);
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
	return lhs->pri - rhs->pri;
}

void pgui_cell(const gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	v2i pos, dim;
	gui_panel_grid_strip_t *strip;

	assert(gui->panel);
	assert(gui->panel->grid.depth > 0);

	strip = &gui->panel->grid.strips[gui->panel->grid.depth - 1];

	pos = v2i_add(gui->panel->grid.pos, gui->panel->scroll);
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

u64 pgui_next_widget_id(const gui_t *gui)
{
	/* TODO(rgriege): base panel widget id off relative position to panel
	 * without scroll, so scrolling doesn't reset focus id */
	s32 x, y;
	assert(gui->panel);
	assert(gui->panel->grid.depth > 0);
	pgui_cell(gui, &x, &y, NULL, NULL);
	return gui_widget_id(gui, x, y);
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
