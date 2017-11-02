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
color_t color_from_hex(const char *hex);
b32     color_equal(color_t lhs, color_t rhs);


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
	GUI_ALIGN_TOP       = 0x08,
	GUI_ALIGN_MIDDLE    = 0x10,
	GUI_ALIGN_BOTTOM    = 0x20,
	GUI_ALIGN_MIDCENTER = GUI_ALIGN_CENTER | GUI_ALIGN_MIDDLE,
} gui_align_t;

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

typedef struct gui_t gui_t;

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
void   gui_destroy(gui_t *gui);
void   gui_move(const gui_t *gui, s32 dx, s32 dy);
void   gui_dim(const gui_t *gui, s32 *x, s32 *y);
void   gui_minimize(gui_t *gui);
void   gui_maximize(gui_t *gui);
void   gui_fullscreen(gui_t *gui);
b32    gui_begin_frame(gui_t *gui);
void   gui_end_frame(gui_t *gui);
void   gui_run(gui_t *gui, u32 fps, b32(*ufunc)(gui_t*, void*), void *udata);

timepoint_t gui_frame_start(gui_t *gui);
timepoint_t gui_last_input_time(gui_t *gui);


typedef enum mouse_button_t
{
	MB_LEFT = 1 << 0,
	MB_MIDDLE = 1 << 1,
	MB_RIGHT = 1 << 2,
	MB_X1 = 1 << 3,
	MB_X2 = 1 << 4,
	MB_WHEELUP = 1 << 5,
	MB_WHEELDOWN = 1 << 6
} mouse_button_t;

/* Input */

void mouse_pos(const gui_t *gui, s32 *x, s32 *y);
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



/* Primitives */

typedef enum img_scale
{
	IMG_SCALED,
	IMG_CENTERED,
} img_scale_t;

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c);
void gui_rect(gui_t *gui, s32 x, s32 y, s32 w, s32 h, color_t fill, color_t line);
void gui_circ(gui_t *gui, s32 x, s32 y, s32 r, color_t fill, color_t line);
void gui_poly(gui_t *gui, const v2i *v, u32 n, color_t fill, color_t line);
void gui_polyf(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t line);
void gui_img(gui_t *gui, s32 x, s32 y, const char *img);
void gui_img_ex(gui_t *gui, s32 x, s32 y, const img_t *img, r32 sx, r32 sy,
                r32 opacity);
void gui_img_boxed(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                   img_scale_t scale);
void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt, color_t c,
             gui_align_t align);
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


/* Widgets */

typedef enum npt_flags_t
{
	NPT_PASSWORD = 1 << 0,
	NPT_NUMERIC  = 1 << 1,
} npt_flags_t;

typedef enum btn_val_t
{
	BTN_NONE,
	BTN_PRESS,
	BTN_HOLD,
} btn_val_t;

b32       gui_npt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
                  const char *hint, npt_flags_t flags);
btn_val_t gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt);
btn_val_t gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                      img_scale_t scale);
btn_val_t gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen);
void      gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt,
                  b32 *val);
void      gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen,
                      b32 *val);
b32       gui_slider_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
b32       gui_slider_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
b32       gui_range_x(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                      r32 min, r32 max);
b32       gui_range_y(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val,
                      r32 min, r32 max);
void      gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, u32 *val, u32 opt);
void      gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                      const char *txt, u32 *val, u32 opt);
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
void      gui_widget_focus(gui_t *gui, u64 id); /* careful! */
b32       gui_widget_active(const gui_t *gui, u64 id);
b32       gui_widget_focused(const gui_t *gui, u64 id);
b32       gui_any_widget_active(const gui_t *gui);
b32       gui_any_widget_has_focus(const gui_t *gui);

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
	r32 sz;
	box2i box;
} gui_split_t;

void gui_split_init(gui_t *gui, gui_split_t *split,
                    gui_split_t *sp1, r32 sz, gui_split_t *sp2,
                    gui_split_flags_t flags);
void gui_vsplit_init(gui_t *gui, gui_split_t *split,
                     gui_split_t *sp1, r32 sz, gui_split_t *sp2,
                     gui_split_flags_t flags);


/* Panels */

#define GUI_ROW_REMAINING 0

#ifndef GUI_PANEL_MAX_COLS
#define GUI_PANEL_MAX_COLS 16
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
	GUI_PANEL_TITLEBAR  = 0x1,
	GUI_PANEL_MOVABLE   = 0x2,
	GUI_PANEL_RESIZABLE = 0x4,
	GUI_PANEL_DRAGGABLE = 0x8,
	GUI_PANEL_CLOSABLE  = 0x10,
	GUI_PANEL_FULL      = 0x1f,
} gui_panel_flags_t;

typedef struct gui_panel
{
	s32 x, y, width, height;
	const char *title;
	gui_panel_flags_t flags;
	u32 id;
	intptr userdata;
	struct
	{
		u32 *current_col;
		u32 cols[GUI_PANEL_MAX_COLS];
		u32 num_cols;
		u32 height;
	} row;
	v2i pos;
	v2i scroll;
	v2i padding;
	v2i required_dim;
	s32 body_height;
	s32 pri;
	struct gui_panel *parent;
	gui_split_t split;
	b32 hidden;
} gui_panel_t;

void pgui_panel_init(gui_t *gui, gui_panel_t *panel, s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_t flags);
void pgui_panel(gui_t *gui, gui_panel_t *panel);
void pgui_panel_finish(gui_t *gui, gui_panel_t *panel);

void pgui_row(gui_t *gui, u32 height, r32 width_ratio);
void pgui_row_cols(gui_t *gui, u32 height, const r32 *cols, u32 num_cols);
#define pgui_row_colsv(gui, height, cols) \
	pgui_row_cols(gui, height, cols, sizeof(cols) / sizeof(cols[0]))
void pgui_row_empty(gui_t *gui, u32 height);
void pgui_row_break(gui_t *gui, u32 top_y, u32 bottom_y, r32 width_ratio);

void      pgui_spacer(gui_t *gui);
void      pgui_txt(gui_t *gui, const char *str);
void      pgui_img(gui_t *gui, const char *fname, img_scale_t scale);
btn_val_t pgui_btn_txt(gui_t *gui, const char *lbl);
btn_val_t pgui_btn_img(gui_t *gui, const char *fname, img_scale_t scale);
btn_val_t pgui_btn_pen(gui_t *gui, gui_pen_t pen);
void      pgui_chk(gui_t *gui, const char *lbl, b32 *val);
b32       pgui_npt(gui_t *gui, char *lbl, u32 n, const char *hint,
                   npt_flags_t flags);
void      pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt);
void      pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt);
b32       pgui_slider_x(gui_t *gui, r32 *val);
b32       pgui_slider_y(gui_t *gui, r32 *val);
b32       pgui_range_x(gui_t *gui, r32 *val, r32 min, r32 max);
b32       pgui_range_y(gui_t *gui, r32 *val, r32 min, r32 max);

void pgui_subpanel_init(gui_t *gui, gui_panel_t *subpanel);
void pgui_subpanel(gui_t *gui, gui_panel_t *subpanel);
void pgui_subpanel_finish(gui_t *gui, gui_panel_t *subpanel);

void pgui_panel_to_front(gui_t *gui, gui_panel_t *panel);
int  pgui_panel_sort(const void *lhs, const void *rhs);

void pgui_row_xw(gui_t *gui, s32 *x, s32 *w);
void pgui_cell(gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h);

u64  pgui_next_widget_id(const gui_t *gui);

void gui_pen_panel_drag(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                        const gui_element_style_t *style);
void gui_pen_panel_minimize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            const gui_element_style_t *style);
void gui_pen_panel_maximize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
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
} gui_widget_style_t;

typedef struct gui_panel_style
{
	color_t bg_color;
	color_t border_color;
	gui_widget_style_t drag;
	gui_element_style_t titlebar;
	gui_widget_style_t close;
	color_t cell_bg_color;
	color_t cell_border_color;
	color_t break_color;
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
	struct
	{
		gui_widget_style_t handle;
		gui_widget_style_t track;
		b32 track_narrow;
	} slider;
	gui_widget_style_t select;
	gui_widget_style_t drag;
	gui_panel_style_t  panel;
} gui_style_t;

const gui_style_t g_gui_style_default;
const gui_style_t g_gui_style_invis;

gui_style_t *gui_style(gui_t *gui);
void         gui_style_set(gui_t *gui, const gui_style_t *style);

void gui_style_push_(gui_t *gui, const void *value, size_t offset, size_t size);
void gui_style_push_color_(gui_t *gui, size_t offset, color_t val);
void gui_style_push_u32_(gui_t *gui, size_t offset, u32 val);
void gui_style_push_r32_(gui_t *gui, size_t offset, r32 val);
void gui_style_push_pen_(gui_t *gui, size_t offset, gui_pen_t pen);
void gui_style_pop(gui_t *gui);

#define gui_style_push(gui, loc, val) \
	do { \
		assert(&gui_style(gui)->loc != &(val)); \
		gui_style_push_(gui, &(val), offsetof(gui_style_t, loc), sizeof(val)); \
	} while (0)
/* literals */
#define gui_style_push_color(gui, loc, val) \
	gui_style_push_color_(gui, offsetof(gui_style_t, loc), val)
#define gui_style_push_u32(gui, loc, val) \
	gui_style_push_u32_(gui, offsetof(gui_style_t, loc), val)
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

static void gui__split(gui_t *gui, gui_split_t *split);


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
		*x += w - 2;
	else /* default to GUI_ALIGN_LEFT */
		*x += 2;
	if (align & GUI_ALIGN_MIDDLE)
		*y += h / 2;
	else if (align & GUI_ALIGN_TOP)
		*y += h - 2;
	else /* default to GUI_ALIGN_BOTTOM */
		*y += 2;
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
		else if (*c == '\r')
			goto out;
		else
			log_warn("unknown character: '%u'", *c);
	}
out:
	return width;
}

static
s32 font__line_offset_x(font_t *f, const char *txt, gui_align_t align)
{
	const s32 width = font__line_width(f, txt);
	if (align & GUI_ALIGN_CENTER)
		return -width / 2;
	else if (align & GUI_ALIGN_RIGHT)
		return -width;
	else /* default to GUI_ALIGN_LEFT */
		return 0;
}

static
s32 font__offset_y(font_t *f, const char *txt, gui_align_t align)
{
	s32 height;
	if (align & GUI_ALIGN_MIDDLE) {
		height = 1;
		for (const char *c = txt; *c != '\0'; ++c)
			if (*c == '\r')
				++height;
		return   height % 2 == 0
		       ? -f->descent + f->line_gap + f->newline_dist * (height / 2 - 1)
		       :   -f->descent - (f->ascent - f->descent) / 2
		         + f->newline_dist * (height / 2);
	} else if (align & GUI_ALIGN_TOP) {
		return -f->ascent - f->line_gap / 2;
	} else /* default to GUI_ALIGN_BOTTOM */ {
		height = -f->descent + f->line_gap / 2;
		for (const char *c = txt; *c != '\0'; ++c)
			if (*c == '\r')
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

#define gi__gui_line_style_dark { \
	.thickness = 1.f, \
	.dash_len = 0.f, \
	.color = gi_black, \
}

#define gi__gui_text_style_default { \
	.size = 14, \
	.color = gi_white, \
	.align = GUI_ALIGN_LEFT | GUI_ALIGN_MIDDLE, \
}

#define gi__gui_text_style_dark { \
	.size = 14, \
	.color = gi_black, \
	.align = GUI_ALIGN_MIDCENTER, \
}

#define gi__gui_btn_text_style_default { \
	.size = 14, \
	.color = gi_white, \
	.align = GUI_ALIGN_MIDCENTER, \
}

#define gi__gui_btn_inactive_style_default { \
	.line = gi__gui_line_style_default, \
	.text = gi__gui_btn_text_style_default, \
	.bg_color = gi_grey77, \
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
}

const gui_style_t g_gui_style_default = {
	.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
	.line = gi__gui_line_style_default,
	.txt = gi__gui_text_style_default,
	.npt = gi__gui_btn_style_default,
	.btn = gi__gui_btn_style_default,
	.chk = gi__gui_chk_style_default,
	.slider = {
		.handle = gi__gui_chk_style_default,
		.track = {
			.pen = gui_pen_rect,
			.inactive = gi__gui_btn_inactive_style_default,
			.hot = gi__gui_btn_inactive_style_default,
			.active = gi__gui_btn_inactive_style_default,
		},
		.track_narrow = false,
	},
	.select = gi__gui_btn_style_default,
	.drag = gi__gui_chk_style_default,
	.panel = {
		.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xbf },
		.border_color = gi_grey128,
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
				.text = gi__gui_text_style_default,
				.bg_color = gi_orange,
				.outline_color = gi_nocolor,
			},
		},
		.titlebar = {
			.line = gi__gui_line_style_default,
			.text = gi__gui_text_style_default,
			.bg_color = { .r=0x66, .g=0x66, .b=0x66, .a=0xff },
			.outline_color = gi_nocolor,
		},
		.close = {
			.pen = gui_pen_rect,
			.inactive = {
				.line = {
					.thickness = 1.f,
					.dash_len = 0.f,
					.color = { .r=0xb0, .g=0xb0, .b=0xb0, .a=0xff },
				},
				.text = gi__gui_text_style_default,
				.bg_color = gi_nocolor,
				.outline_color = gi_nocolor,
			},
			.hot = {
				.line = gi__gui_line_style_default,
				.text = gi__gui_text_style_dark,
				.bg_color = gi_nocolor,
				.outline_color = gi_nocolor,
			},
			.active = {
				.line = {
					.thickness = 1.f,
					.dash_len = 0.f,
					.color = gi_orange,
				},
				.text = gi__gui_text_style_default,
				.bg_color = gi_nocolor,
				.outline_color = gi_nocolor,
			},
		},
		.cell_bg_color = gi_nocolor,
		.cell_border_color = gi_nocolor,
		.break_color =  { .r=0x33, .g=0x33, .b=0x33, .a=0xff },
		.padding = 10.f,
	},
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
}

const gui_style_t g_gui_style_invis = {
	.bg_color = { .r=0x22, .g=0x1f, .b=0x1f, .a=0xff },
	.line = gi__gui_line_style_invis,
	.txt = gi__gui_text_style_invis,
	.npt = gi__gui_widget_style_invis,
	.btn = gi__gui_widget_style_invis,
	.chk = gi__gui_widget_style_invis,
	.slider = {
		.handle = gi__gui_widget_style_invis,
		.track = gi__gui_widget_style_invis,
		.track_narrow = false,
	},
	.select = gi__gui_widget_style_invis,
	.drag = gi__gui_widget_style_invis,
	.panel = {
		.bg_color = gi_nocolor,
		.border_color = gi_nocolor,
		.drag = gi__gui_widget_style_invis,
		.titlebar = gi__gui_element_style_invis,
		.close = gi__gui_widget_style_invis,
		.cell_bg_color = gi_nocolor,
		.cell_border_color = gi_nocolor,
		.break_color = gi_nocolor,
		.padding = 10.f,
	},
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

typedef struct gui_t
{
	timepoint_t creation_time;
	timepoint_t frame_start_time;
	timepoint_t last_input_time;
	u32 frame_time_milli;
	SDL_Window *window;
	SDL_GLContext gl_context;
	SDL_Window *parent_window;
	SDL_GLContext parent_gl_context;

	u32 vao, vbo[VBO_COUNT];
	shader_prog_t shader;
	texture_t texture_white;
	texture_t texture_white_dotted;
	v2f verts[GUI_MAX_VERTS];
	color_t vert_colors[GUI_MAX_VERTS];
	v2f vert_tex_coords[GUI_MAX_VERTS];
	u32 vert_cnt;
	draw_call_t draw_calls[GUI_MAX_DRAW_CALLS];
	u32 draw_call_cnt;
	gui__scissor_t scissors[GUI_MAX_SCISSORS];
	u32 scissor_cnt;

	v2i win_halfdim;
	v2i mouse_pos;
	u32 mouse_btn;
	u32 mouse_btn_diff;
	b32 mouse_covered_by_panel;
	b32 mouse_debug;

	u8 prev_keys[KB_COUNT];
	const u8 *keys;
	gui__key_toggle_state_t key_toggles[KBT_COUNT];
	u32 repeat_delay;
	u32 repeat_interval;
	u32 repeat_timer;
	u32 npt_prev_key_idx;
	u32 npt_cursor_pos;

	SDL_Cursor *cursors[GUI__CURSOR_COUNT];
	b32 use_default_cursor;
	font_t *fonts;
	cached_img_t *imgs;
	gui_style_t style;
	u8 style_stack[GUI_STYLE_STACK_LIMIT];
	u32 style_stack_sz;

	u64 hot_id;    /* hover */
	u64 active_id; /* mouse down */
	u64 focus_id;  /* widgets like input boxes change state post-click */
	u64 active_id_at_frame_start;
	b32 hot_id_found_this_frame;
	b32 active_id_found_this_frame;
	b32 focus_id_found_this_frame;

	v2i drag_offset;
	char *pw_buf;
	v2f *vert_buf;

	gui_split_t *root_split;

	gui_panel_t *panel;
	u32 next_panel_id;
	s32 next_panel_pri, min_panel_pri;
} gui_t;

static u32 g_gui_cnt = 0;

gui_t *gui_create(s32 x, s32 y, s32 w, s32 h, const char *title,
                  gui_flags_t flags)
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

	// Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
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
	GL_CHECK(glGenBuffers, 3, gui->vbo);

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

	gui->cursors[GUI__CURSOR_DEFAULT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	gui->cursors[GUI__CURSOR_RESIZE_NS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	gui->cursors[GUI__CURSOR_RESIZE_EW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	for (u32 i = 0; i < GUI__CURSOR_COUNT; ++i)
		if (!gui->cursors[i])
			goto err_cursor;
	gui->fonts = array_create();
	gui->imgs = array_create();

	SDL_Event evt;
	while (SDL_PollEvent(&evt) == 1); /* must be run before SDL_GetWindowSize */
	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x, &gui->win_halfdim.y);
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div_eq(&gui->win_halfdim, g_v2i_2);

	gui->creation_time = time_current();
	gui->frame_start_time = gui->creation_time;
	gui->last_input_time = gui->creation_time;
	gui->frame_time_milli = 0;

	gui->repeat_delay = 500;
	gui->repeat_interval = 100;
	gui->repeat_timer = gui->repeat_delay;
	gui->npt_cursor_pos = 0;

	gui_style_set(gui, &g_gui_style_default);

	gui->style_stack_sz = 0;

	gui->mouse_debug = false;

	memset(gui->prev_keys, 0, KB_COUNT);
	memset(gui->key_toggles, 0, KBT_COUNT * sizeof(gui__key_toggle_state_t));

	gui->hot_id = 0;
	gui->active_id = 0;
	gui->focus_id = 0;
	gui->active_id_at_frame_start = 0;

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

b32 gui_begin_frame(gui_t *gui)
{
	s32 key_cnt;
	b32 quit = false;
	float bg_color[4];
	timepoint_t now = time_current();
	gui->frame_time_milli = time_diff_milli(gui->frame_start_time, now);
	gui->frame_start_time = now;

	SDL_GL_MakeCurrent(gui->window, gui->gl_context);

	SDL_Event evt;
	const u32 last_mouse_btn = gui->mouse_btn;
	gui->mouse_btn = 0;
	while (SDL_PollEvent(&evt) == 1) {
		switch (evt.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEWHEEL:
			gui->mouse_btn |= (evt.wheel.y > 0 ? MB_WHEELUP : MB_WHEELDOWN);
			gui->last_input_time = now;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_WINDOWEVENT:
			gui->last_input_time = now;
			break;
		}
	}

	gui->mouse_btn |= SDL_GetMouseState(&gui->mouse_pos.x, &gui->mouse_pos.y);
	gui->mouse_btn_diff = gui->mouse_btn ^ last_mouse_btn;
	SDL_GetWindowSize(gui->window, &gui->win_halfdim.x, &gui->win_halfdim.y);
	gui->mouse_pos.y = gui->win_halfdim.y - gui->mouse_pos.y;
	static const v2i g_v2i_2 = { .x=2, .y=2 };
	v2i_div_eq(&gui->win_halfdim, g_v2i_2);
	gui->mouse_covered_by_panel = false;

	gui->keys = SDL_GetKeyboardState(&key_cnt);
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
	}
	gui->focus_id_found_this_frame = false;

	gui->active_id_at_frame_start = gui->active_id;

	gui->vert_cnt = 0;
	gui->draw_call_cnt = 0;
	gui->scissor_cnt = 0;
	gui_unmask(gui);

	GL_CHECK(glViewport, 0, 0, 2*gui->win_halfdim.x, 2*gui->win_halfdim.y);

	/* NOTE(rgriege): reset the scissor for glClear */
	GL_CHECK(glScissor, 0, 0, gui->win_halfdim.x * 2, gui->win_halfdim.y * 2);

	color_as_float_array(bg_color, gui->style.bg_color);
	GL_CHECK(glClearColor, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
	GL_CHECK(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gui->use_default_cursor = true;
	if (gui->root_split) {
		box2i_from_center(&gui->root_split->box, gui->win_halfdim, gui->win_halfdim);
		gui__split(gui, gui->root_split);
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
	box2i_from_xywh(box, gui->scissors[gui->scissor_cnt-1].x,
	                gui->scissors[gui->scissor_cnt-1].y,
	                gui->scissors[gui->scissor_cnt-1].w,
	                gui->scissors[gui->scissor_cnt-1].h);
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
void gui__poly(gui_t *gui, const v2f *v, u32 n, color_t fill, color_t stroke)
{
	r32 dist;
	v2f last;
	box2f mask, bbox;

	gui__current_maskf(gui, &mask);
	polyf_bounding_box(v, n, &bbox);
	if (!box2f_overlaps(mask, bbox))
		return;

	if (!color_equal(fill, g_nocolor)) {
		assert(gui->vert_cnt + n <= GUI_MAX_VERTS);
		assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

		for (u32 i = 0; i < n; ++i) {
			gui->verts[gui->vert_cnt+i] = v[i];
			gui->vert_colors[gui->vert_cnt+i] = fill;
			gui->vert_tex_coords[gui->vert_cnt+i] = g_v2f_zero;
		}
		gui->draw_calls[gui->draw_call_cnt].idx = gui->vert_cnt;
		gui->draw_calls[gui->draw_call_cnt].cnt = n;
		gui->draw_calls[gui->draw_call_cnt].type = DRAW_TRIANGLE_FAN;
		gui->draw_calls[gui->draw_call_cnt].tex = gui->texture_white.handle;
		++gui->draw_call_cnt;
		gui->vert_cnt += n;
	}
	if (!color_equal(stroke, g_nocolor)) {
		assert(gui->vert_cnt + n <= GUI_MAX_VERTS);
		assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

		if (gui->style.line.dash_len != 0.f) {
			dist = 0;
			last = v[n-1];
			for (u32 i = 0; i < n; ++i) {
				gui->verts[gui->vert_cnt+i] = v[i];
				gui->vert_colors[gui->vert_cnt+i] = stroke;
				gui->vert_tex_coords[gui->vert_cnt+i].x = dist / gui->style.line.dash_len;
				gui->vert_tex_coords[gui->vert_cnt+i].y = 0;
				dist += v2f_dist(last, v[i]);
			}
			gui->draw_calls[gui->draw_call_cnt].tex = gui->texture_white_dotted.handle;
		} else {
			for (u32 i = 0; i < n; ++i) {
				gui->verts[gui->vert_cnt+i] = v[i];
				gui->vert_colors[gui->vert_cnt+i] = stroke;
				gui->vert_tex_coords[gui->vert_cnt+i] = g_v2f_zero;
			}
			gui->draw_calls[gui->draw_call_cnt].tex = gui->texture_white.handle;
		}
		gui->draw_calls[gui->draw_call_cnt].idx = gui->vert_cnt;
		gui->draw_calls[gui->draw_call_cnt].cnt = n;
		gui->draw_calls[gui->draw_call_cnt].type = DRAW_LINE_LOOP;
		++gui->draw_call_cnt;
		gui->vert_cnt += n;
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
void text__render(gui_t *gui, const texture_t *texture, r32 yb, r32 x0, r32 y0,
                  r32 x1, r32 y1, r32 s0, r32 t0, r32 s1, r32 t1, color_t color)
{
	r32 dy;
	box2f mask, bbox;

	assert(gui->vert_cnt + 4 < GUI_MAX_VERTS);
	assert(gui->draw_call_cnt < GUI_MAX_DRAW_CALLS);

	/* TODO(rgriege): figure out why these come in upside-down */
	dy = y1 - y0;
	y0 = yb + (yb - y1);
	y1 = y0 + dy;

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
	if (gui->scissor_cnt > 0) {
		if (gui->draw_call_cnt == gui->scissors[gui->scissor_cnt-1].draw_call_idx)
			--gui->scissor_cnt;
		else
			gui->scissors[gui->scissor_cnt-1].draw_call_cnt
				= gui->draw_call_cnt - gui->scissors[gui->scissor_cnt-1].draw_call_idx;
	}
}

void gui_end_frame(gui_t *gui)
{
	gui__complete_scissor(gui);

	GL_CHECK(glDisable, GL_DEPTH_TEST);
	GL_CHECK(glBindVertexArray, gui->vao);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, gui->vbo[VBO_VERT]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, gui->vert_cnt * sizeof(v2f),
	             gui->verts, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, VBO_VERT, 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, VBO_VERT);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, gui->vbo[VBO_COLOR]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, gui->vert_cnt * sizeof(color_t),
	             gui->vert_colors, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, VBO_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, VBO_COLOR);

	GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, gui->vbo[VBO_TEX]);
	GL_CHECK(glBufferData, GL_ARRAY_BUFFER, gui->vert_cnt * sizeof(v2f),
	             gui->vert_tex_coords, GL_STREAM_DRAW);
	GL_CHECK(glVertexAttribPointer, VBO_TEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK(glEnableVertexAttribArray, VBO_TEX);

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
			GL_CHECK(glBindTexture, GL_TEXTURE_2D, draw_call->tex);
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

void gui_run(gui_t *gui, u32 fps, b32(*ufunc)(gui_t *gui, void *udata),
             void *udata)
{
	const u32 target_frame_milli = 1000/fps;
	timepoint_t start, end;
	b32 quit = false;
	while(gui_begin_frame(gui) && !quit) {
		vlt_mem_advance_gen();
		start = time_current();
		quit = ufunc(gui, udata);
		gui_end_frame(gui);
		end = time_current();
		const u32 frame_milli = time_diff_milli(start, end);
		if (frame_milli < target_frame_milli)
			time_sleep_milli(target_frame_milli - frame_milli);
		else
			log_warn("long frame: %ums", frame_milli);
	}
}

timepoint_t gui_frame_start(gui_t *gui)
{
	return gui->frame_start_time;
}

timepoint_t gui_last_input_time(gui_t *gui)
{
	return gui->last_input_time;
}

/* Input */

void mouse_pos(const gui_t *gui, s32 *x, s32 *y)
{
	*x = gui->mouse_pos.x;
	*y = gui->mouse_pos.y;
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


/* Immediate Mode API */

void gui_line(gui_t *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w, color_t c)
{
	assert(w >= 1);
	if (w == 1) {
		v2f poly[2] = {
			{ x0, y0 },
			{ x1, y1 }
		};
		gui__poly(gui, poly, 2, g_nocolor, c);
	} else {
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
	if (font_load(font, "Roboto.ttf", sz)) {
		return font;
	} else {
		array_pop(gui->fonts);
		return NULL;
	}
}

static
void gui__txt_char_pos(gui_t *gui, s32 *ix, s32 *iy, s32 w, s32 h, s32 sz,
                       const char *txt, u32 pos, gui_align_t align)
{
	font_t *font;
	s32 ix_ = *ix, iy_ = *iy;
	r32 x, y;
	stbtt_aligned_quad q;

	font = gui__get_font(gui, sz);
	assert(font);

	font__align_anchor(&ix_, &iy_, w, h, align);
	x = ix_ + font__line_offset_x(font, txt, align);
	y = iy_ + font__offset_y(font, txt, align);

	if (pos == 0)
		goto out;

	for (u32 i = 0; i < pos; ++i) {
		const int ch = *(const u8*)&txt[i];
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
		} else if (txt[i] == '\r') {
			y -= font->newline_dist;
			x = ix_ + font__line_offset_x(font, &txt[i+1], align);
		}
	}

out:
	*ix = x;
	*iy = y;
}

static
void gui__txt(gui_t *gui, s32 *ix, s32 *iy, s32 sz, const char *txt,
              color_t color, gui_align_t align)
{
	font_t *font;
	r32 x = *ix, y = *iy;
	stbtt_aligned_quad q;

	font = gui__get_font(gui, sz);
	assert(font);

	x += font__line_offset_x(font, txt, align);
	y += font__offset_y(font, txt, align);

	for (const char *c = txt; *c != '\0'; ++c) {
		const int ch = *(const u8*)c;
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
			text__render(gui, &font->texture, y, q.x0, q.y0, q.x1, q.y1, q.s0, q.t0,
			             q.s1, q.t1, color);
		} else if (*c == '\r') {
			y -= font->newline_dist;
			x = *ix + font__line_offset_x(font, c + 1, align);
		}
	}
	*ix = x;
	*iy = y;
}

static
u32 gui__txt_mouse_pos(gui_t *gui, s32 xi_, s32 yi_, s32 w, s32 h, s32 sz,
                       const char *txt, v2i mouse, gui_align_t align)
{
	font_t *font;
	s32 xi = xi_, yi = yi_;
	r32 x, y;
	stbtt_aligned_quad q;
	u32 closest_pos;
	s32 closest_dist, dist;
	v2i p;

	font = gui__get_font(gui, sz);
	assert(font);

	font__align_anchor(&xi, &yi, w, h, align);
	x = xi + font__line_offset_x(font, txt, align);
	y = yi + font__offset_y(font, txt, align);

	v2i_set(&p, x, y + font->ascent / 2);
	closest_pos = 0;
	closest_dist = v2i_dist_sq(p, mouse);

	for (const char *c = txt; *c != '\0'; ++c) {
		const int ch = *(const u8*)c;
		if (ch >= GUI__FONT_MIN_CHAR && ch <= GUI__FONT_MAX_CHAR) {
			stbtt_GetPackedQuad(font->char_info, font->texture.width,
			                    font->texture.height, ch - GUI__FONT_MIN_CHAR,
			                    &x, &y, &q, 1);
		} else if (*c == '\r') {
			y -= font->newline_dist;
			x = xi + font__line_offset_x(font, c + 1, align);
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
	return closest_pos;
}

void gui_txt(gui_t *gui, s32 x, s32 y, s32 sz, const char *txt,
             color_t c, gui_align_t align)
{
	gui__txt(gui, &x, &y, sz, txt, c, align);
}

s32 gui_txt_width(gui_t *gui, const char *txt, u32 sz)
{
	s32 width = 0;
	const char *line = txt;
	font_t *font = gui__get_font(gui, sz);
	assert(font);
	while (*line != '\0') {
		const s32 line_width = font__line_width(font, txt);
		width = max(width, line_width);
		while (*line != '\0' && *line != '\r')
			++line;
	}
	return width;
}

void gui_mask(gui_t *gui, s32 x, s32 y, s32 w, s32 h)
{
	gui__complete_scissor(gui);

	assert(gui->scissor_cnt < GUI_MAX_SCISSORS);

	gui->scissors[gui->scissor_cnt].draw_call_idx = gui->draw_call_cnt;
	gui->scissors[gui->scissor_cnt].draw_call_cnt = 0;
	gui->scissors[gui->scissor_cnt].x = x;
	gui->scissors[gui->scissor_cnt].y = y;
	gui->scissors[gui->scissor_cnt].w = w;
	gui->scissors[gui->scissor_cnt].h = h;

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
	font__align_anchor(&x, &y, w, h, style->align);
	gui_txt(gui, x, y, style->size, txt, style->color, style->align);
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

typedef enum gui__widget_status
{
	GUI__WIDGET_INACTIVE,
	GUI__WIDGET_HOT,
	GUI__WIDGET_ACTIVE,
} gui__widget_status_t;

static
gui__widget_status_t gui__widget_status(const gui_t *gui, u64 id)
{
	if (gui->active_id == id || gui->focus_id == id)
		return GUI__WIDGET_ACTIVE;
	else if (gui->hot_id == id)
		return GUI__WIDGET_HOT;
	else
		return GUI__WIDGET_INACTIVE;
}

static
const gui_element_style_t *gui__element_style(gui__widget_status_t status,
                                              const gui_widget_style_t *style)
{
	switch (status) {
	case GUI__WIDGET_ACTIVE:
		return &style->active;
	case GUI__WIDGET_HOT:
		return &style->hot;
	case GUI__WIDGET_INACTIVE:
		return &style->inactive;
	}
	assert(false);
	return NULL;
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

static
b32 gui__box_visible(const gui_t *gui, box2i box)
{
	box2i mask, clipped;
	gui__current_mask(gui, &mask);
	clipped = box2i_intersection(mask, box);
	return    clipped.max.x - clipped.min.x >= (box.max.x - box.min.x) / 2
	       && clipped.max.y - clipped.min.y >= (box.max.y - box.min.y) / 2;
}

static
b32 gui__can_repeat(gui_t *gui)
{
	if (gui->repeat_timer <= gui->frame_time_milli) {
		gui->repeat_timer =   gui->repeat_interval
		                    - (gui->frame_time_milli - gui->repeat_timer);
		return true;
	} else {
		gui->repeat_timer -= gui->frame_time_milli;
		return false;
	}
}

static inline
b32 gui__convert_key_to_char(const gui_t *gui, gui_key_t key, char *c)
{
	static const char gui_key_chars[KB_COUNT] = {
		0, 0, 0, 0,
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		0, 0, 0, '\t', ' ',
		'-', '=', '[', ']', '\\',
		0,
		';', '\'', '`', ',', '.', '/',
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* KB_CAPSLOCK - KB_F12 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* KB_PRINTSCREEN - KB_UP */
		0,
		'/', '*', '-', '+', 0,
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		'.', '=',
	};

	static const char caps[128] = {
			0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
		 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
		 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
		 30,  31,  32,  33,  34,  35,  36,  37,  38,  34,
		 40,  41,  42,  43,  60,  95,  62,  63,  41,  33,
		 64,  35,  36,  37,  94,  38,  42,  40,  58,  58,
		 60,  43,  62,  63,  64,  65,  66,  67,  68,  69,
		 70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
		 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
		 90, 123, 124, 125,  94,  95, 126,  65,  66,  67,
		 68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
		 78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
		 88,  89,  90, 123, 124, 125, 126, 127
	};

	*c = gui_key_chars[key];
	if (key_mod(gui, KBM_SHIFT) != key_toggled(gui, KBT_CAPS))
		*c = caps[(u8)*c];
	return *c != 0;
}

b32 gui_npt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, char *txt, u32 n,
            const char *hint, npt_flags_t flags)
{
	const u64 id = gui_widget_id(gui, x, y);
	box2i box;
	box2i_from_dims(&box, x, y+h, x+w, y);
	const b32 contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                           && gui__box_visible(gui, box);
	b32 complete = false;
	const char *displayed_txt;

	if (gui->focus_id == id) {
		if (contains_mouse && mouse_pressed(gui, MB_LEFT)) {
			const u32 size = gui->style.npt.active.text.size;
			const gui_align_t align = gui->style.npt.active.text.align;
			gui->npt_cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, size, txt,
			                                         gui->mouse_pos, align);
		}
		u32 key_idx;
		for (key_idx = 0; key_idx < KB_COUNT; ++key_idx)
			if (gui->keys[key_idx])
				break;
		if (key_idx != KB_COUNT) {
			b32 modify = false;
			if (key_idx != gui->npt_prev_key_idx) {
				gui->repeat_timer = gui->repeat_delay;
				gui->npt_prev_key_idx = key_idx;
				modify = true;
			} else if (gui__can_repeat(gui)) {
				modify = true;
			}
			if (modify) {
				u32 len = strlen(txt);
				char key_char = 0;
				if (key_idx == KB_BACKSPACE) {
					if (gui->npt_cursor_pos > 0) {
						for (u32 i = gui->npt_cursor_pos - 1; i < len; ++i)
							txt[i] = txt[i+1];
						--gui->npt_cursor_pos;
					}
				} else if (key_idx == KB_DELETE) {
					for (u32 i = gui->npt_cursor_pos; i < len; ++i)
						txt[i] = txt[i+1];
				} else if (key_idx == KB_RETURN) {
					gui->focus_id = 0;
					complete = true;
				} else if (key_idx == KB_ESCAPE) {
					gui->focus_id = 0;
				} else if (key_idx == KB_V && key_mod(gui, KBM_CTRL)) {
					char *clipboard;
					u32 cnt;
					if (   SDL_HasClipboardText()
					    && (clipboard = SDL_GetClipboardText())
					    && (cnt = min(n - 1 - len, strlen(clipboard))) > 0) {
						memmove(&txt[gui->npt_cursor_pos + cnt],
								&txt[gui->npt_cursor_pos],
								len - gui->npt_cursor_pos + 1);
						memcpy(&txt[gui->npt_cursor_pos], clipboard, cnt);
						gui->npt_cursor_pos += cnt;
						SDL_free(clipboard);
					}
				} else if (key_idx == KB_LEFT) {
					if (gui->npt_cursor_pos > 0)
						--gui->npt_cursor_pos;
				} else if (key_idx == KB_RIGHT) {
					if (gui->npt_cursor_pos < len)
						++gui->npt_cursor_pos;
				} else if (key_idx == KB_HOME) {
					gui->npt_cursor_pos = 0;
				} else if (key_idx == KB_END) {
					gui->npt_cursor_pos = len;
				} else if (   gui__convert_key_to_char(gui, key_idx, &key_char)
				           && len < n-1
				           && (  (flags & NPT_NUMERIC) == 0
				               ? (isgraph(key_char) || key_char == ' ')
				               : isdigit(key_char))) {
					for (u32 i = len + 1; i > gui->npt_cursor_pos; --i)
						txt[i] = txt[i-1];
					txt[gui->npt_cursor_pos++] = key_char;
				}
			}
		} else {
			gui->repeat_timer = gui->repeat_delay;
			gui->npt_prev_key_idx = KB_COUNT;
		}
		if (mouse_pressed(gui, MB_LEFT) && !contains_mouse)
			gui->focus_id = 0;
	} else if (gui->active_id == id) {
		if (mouse_released(gui, MB_LEFT)) {
			if (contains_mouse)
				gui->focus_id = id;
			gui->active_id = 0;
			gui->repeat_timer = gui->repeat_delay;
		} else {
			gui->active_id_found_this_frame = true;
		}
	} else if (gui->hot_id == id) {
		if (!contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, MB_LEFT)) {
			const u32 size = gui->style.npt.active.text.size;
			const gui_align_t align = gui->style.npt.active.text.align;
			gui->active_id = id;
			gui->npt_cursor_pos = gui__txt_mouse_pos(gui, x, y, w, h, size, txt,
			                                         gui->mouse_pos, align);
			gui->npt_prev_key_idx = 0;
			gui->hot_id = 0;
			gui->active_id_found_this_frame = true;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui__allow_new_interaction(gui) && contains_mouse) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}


	const gui_element_style_t *style;
	if (gui->active_id == id || gui->focus_id == id)
		style = &gui->style.npt.active;
	else if (gui->hot_id == id)
		style = &gui->style.npt.hot;
	else
		style = &gui->style.npt.inactive;
	gui->style.npt.pen(gui, x, y, w, h, style);

	if (flags & NPT_PASSWORD) {
		const u32 sz = strlen(txt);
		array_reserve(gui->pw_buf, sz+1);
		for (u32 i = 0; i < sz; ++i)
			gui->pw_buf[i] = '*';
		gui->pw_buf[sz] = '\0';
		gui_txt_styled(gui, x, y, w, h, gui->pw_buf, &style->text);
		displayed_txt = gui->pw_buf;
	} else {
		gui_txt_styled(gui, x, y, w, h, txt, &style->text);
		displayed_txt = txt;
	}
	if (gui->focus_id == id) {
		gui->focus_id_found_this_frame = true;
		if (time_diff_milli(gui->creation_time, gui->frame_start_time) % 1000 < 500) {
			const u32 size = gui->style.npt.active.text.size;
			const gui_align_t align = gui->style.npt.active.text.align;
			const color_t color = gui->style.npt.active.text.color;
			const font_t *font = gui__get_font(gui, size);
			gui__txt_char_pos(gui, &x, &y, w, h, size, displayed_txt,
			                  gui->npt_cursor_pos, align);
			gui_line(gui, x + 1, y + font->descent, x + 1, y + font->ascent, 1, color);
		}
	} else if (hint && strlen(txt) == 0) {
		gui_txt_styled(gui, x, y, w, h, hint, &style->text);
	}
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
	                  && gui__box_visible(gui, box);
	if (gui->hot_id == id) {
		if (!*contains_mouse || gui->mouse_covered_by_panel) {
			gui->hot_id = 0;
		} else if (mouse_pressed(gui, MB_LEFT)) {
			gui->hot_id = 0;
			gui->active_id = id;
			gui->active_id_found_this_frame = true;
			gui->repeat_timer = gui->repeat_delay;
		} else {
			gui->hot_id_found_this_frame = true;
		}
	} else if (gui->active_id == id) {
		if (mouse_released(gui, MB_LEFT)) {
			if (*contains_mouse)
				retval = BTN_PRESS;
			gui->active_id = 0;
			gui->repeat_timer = gui->repeat_delay;
		} else {
			gui->active_id_found_this_frame = true;
			if (!*contains_mouse) {
				gui->repeat_timer = gui->repeat_delay;
			} else if (gui__can_repeat(gui)) {
				retval = BTN_HOLD;
			}
		}
	} else if (gui__allow_new_interaction(gui) && *contains_mouse) {
		gui->hot_id = id;
		gui->hot_id_found_this_frame = true;
	}
	return retval;
}

static
gui__widget_status_t gui__btn_status(const gui_t *gui, u64 id, b32 contains_mouse)
{
	if (gui->active_id == id)
		return contains_mouse ? GUI__WIDGET_ACTIVE : GUI__WIDGET_HOT;
	else if (gui->hot_id == id)
		return GUI__WIDGET_HOT;
	else
		return GUI__WIDGET_INACTIVE;
}

static
void gui__btn_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                     const char *txt, gui__widget_status_t status,
                     const gui_widget_style_t *style_)
{
	const gui_element_style_t *style = gui__element_style(status, style_);
	style_->pen(gui, x, y, w, h, style);
	gui_txt_styled(gui, x, y, w, h, txt, &style->text);
}

btn_val_t gui_btn_txt(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, id, &contains_mouse);
	const gui__widget_status_t status = gui__btn_status(gui, id, contains_mouse);
	gui__btn_render(gui, x, y, w, h, txt, status, &gui->style.btn);
	return ret;
}

btn_val_t gui_btn_img(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *fname,
                      img_scale_t scale)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, id, &contains_mouse);
	const gui__widget_status_t status = gui__btn_status(gui, id, contains_mouse);
	const gui_element_style_t *style = gui__element_style(status, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, style);
	gui_img_boxed(gui, x, y, w, h, fname, scale);
	return ret;
}

btn_val_t gui_btn_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	const btn_val_t ret = gui__btn_logic(gui, x, y, w, h, id, &contains_mouse);
	const gui__widget_status_t status = gui__btn_status(gui, id, contains_mouse);
	const gui_element_style_t *style = gui__element_style(status, &gui->style.btn);
	gui->style.btn.pen(gui, x, y, w, h, style);
	pen(gui, x, y, w, h, style);
	return ret;
}

void gui_chk(gui_t *gui, s32 x, s32 y, s32 w, s32 h, const char *txt, b32 *val)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	gui__widget_status_t status = GUI__WIDGET_INACTIVE;
	if (gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS)
		*val = !*val;

	if (gui->active_id == id)
		status = contains_mouse ? GUI__WIDGET_ACTIVE : GUI__WIDGET_HOT;
	else if (gui->hot_id == id)
		status = GUI__WIDGET_HOT;
	else if (*val)
		status = GUI__WIDGET_ACTIVE;
	gui__btn_render(gui, x, y, w, h, txt, status, &gui->style.chk);
}

void gui_chk_pen(gui_t *gui, s32 x, s32 y, s32 w, s32 h, gui_pen_t pen, b32 *val)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	gui__widget_status_t status = GUI__WIDGET_INACTIVE;
	const gui_element_style_t *style;

	if (gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS)
		*val = !*val;

	if (gui->active_id == id)
		status = contains_mouse ? GUI__WIDGET_ACTIVE : GUI__WIDGET_HOT;
	else if (gui->hot_id == id)
		status = GUI__WIDGET_HOT;
	else if (*val)
		status = GUI__WIDGET_ACTIVE;
	style = gui__element_style(status, &gui->style.chk);
	gui->style.chk.pen(gui, x, y, w, h, style);
	pen(gui, x, y, w, h, style);
}

typedef enum gui__slider_orientation
{
	GUI__SLIDER_X,
	GUI__SLIDER_Y,
} gui__slider_orientation_t;

b32 gui__slider(gui_t *gui, s32 x, s32 y, s32 w, s32 h, r32 *val, s32 hnd_len,
                gui__slider_orientation_t orientation)
{
	assert(*val >= 0.f && *val <= 1.f);

	const u64 id = gui_widget_id(gui, x, y);
	box2i box;
	if (orientation == GUI__SLIDER_Y) {
		box.min = (v2i){ .x=x, .y=y+(h-hnd_len)**val };
		box.max = (v2i){ .x=x+w, .y=y+hnd_len+(h-hnd_len)**val };
	} else {
		box.min = (v2i){ .x=x+(w-hnd_len)**val, .y=y };
		box.max = (v2i){ .x=x+hnd_len+(w-hnd_len)**val, .y=y+h };
	}
	const b32 contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                           && gui__box_visible(gui, box);

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

	const gui__widget_status_t status = gui__widget_status(gui, id);
	const gui_element_style_t *style_track =
		gui__element_style(status, &gui->style.slider.track);
	const gui_element_style_t *style_handle =
		gui__element_style(status, &gui->style.slider.handle);
	if (orientation == GUI__SLIDER_Y) {
		if (gui->style.slider.track_narrow)
			gui->style.slider.track.pen(gui, x, y + hnd_len / 2, w, h - hnd_len,
			                            style_track);
		else
			gui->style.slider.track.pen(gui, x, y, w, h, style_track);
		gui->style.slider.handle.pen(gui, x, y + (h - hnd_len) * *val, w, hnd_len,
		                             style_handle);
	} else {
		if (gui->style.slider.track_narrow)
			gui->style.slider.track.pen(gui, x + hnd_len / 2, y, w - hnd_len, h,
			                            style_track);
		else
			gui->style.slider.track.pen(gui, x, y, w, h, style_track);
		gui->style.slider.handle.pen(gui, x + (w - hnd_len) * *val, y, hnd_len, h,
		                             style_handle);
	}
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

void gui_select(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                const char *txt, u32 *val, u32 opt)
{
	const u64 id = gui_widget_id(gui, x, y);
	const b32 selected = *val == opt;
	gui__widget_status_t status = GUI__WIDGET_INACTIVE;
	b32 contains_mouse;
	if (   gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS
	    && !selected) {
		*val = opt;
	}

	if (gui->active_id == id)
		status = contains_mouse ? GUI__WIDGET_ACTIVE : GUI__WIDGET_HOT;
	else if (gui->hot_id == id)
		status = GUI__WIDGET_HOT;
	else if (*val == opt)
		status = GUI__WIDGET_ACTIVE;
	gui__btn_render(gui, x, y, w, h, txt, status, &gui->style.select);
}

void gui_mselect(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                 const char *txt, u32 *val, u32 opt)
{
	const u64 id = gui_widget_id(gui, x, y);
	b32 contains_mouse;
	gui__widget_status_t status = GUI__WIDGET_INACTIVE;
	if (gui__btn_logic(gui, x, y, w, h, id, &contains_mouse) == BTN_PRESS) {
	  if (!(*val & opt))
			*val |= opt;
		else if (*val & ~opt)
			*val &= ~opt;
	}

	if (gui->active_id == id)
		status = contains_mouse ? GUI__WIDGET_ACTIVE : GUI__WIDGET_HOT;
	else if (gui->hot_id == id)
		status = GUI__WIDGET_HOT;
	else if (*val & opt)
		status = GUI__WIDGET_ACTIVE;
	gui__btn_render(gui, x, y, w, h, txt, status, &gui->style.select);
}

static
b32 gui__drag(gui_t *gui, s32 *x, s32 *y, b32 contains_mouse, mouse_button_t mb,
              u64 *id, gui_drag_callback_t cb, void *udata)
{
	b32 retval = false;
	*id = gui_widget_id(gui, *x, *y);
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
	return retval;
}

static
void gui__drag_render(gui_t *gui, s32 x, s32 y, s32 w, s32 h, u64 id,
                      const gui_widget_style_t *style_)
{
	const gui__widget_status_t status = gui__widget_status(gui, id);
	const gui_element_style_t *style = gui__element_style(status, style_);
	style_->pen(gui, x, y, w, h, style);
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
	                 && gui__box_visible(gui, box);
	return gui__drag(gui, x, y, contains_mouse, mb, id, cb, udata);
}

b32 gui_dragx(gui_t *gui, s32 *x, s32 *y, u32 w, u32 h, mouse_button_t mb,
              gui_drag_callback_t cb, void *udata)
{
	u64 id;
	const b32 ret = gui__drag_logic(gui, &id, x, y, w, h, mb, cb, udata);
	gui__drag_render(gui, *x, *y, w, h, id, &gui->style.drag);
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
	                           && gui__box_visible(gui, box);
	const b32 ret = gui__drag(gui, x, y, contains_mouse, mb, &id, cb, udata);
	gui_style_push_pen(gui, drag.pen, gui_pen_circ);
	gui__drag_render(gui, *x - r, *y - r, 2 * r, 2 * r, id, &gui->style.drag);
	gui_style_pop(gui);
	return ret;
}

#define GUI__DEFAULT_PANEL_ID UINT_MAX

u64 gui_widget_id(const gui_t *gui, s32 x, s32 y)
{
	u32 panel_id = gui->panel ? gui->panel->id : GUI__DEFAULT_PANEL_ID;
	return (((u64)x) << 48) | (((u64)y) << 32) | panel_id;
}

void gui_widget_focus(gui_t *gui, u64 id)
{
	gui->focus_id = id;
}

b32 gui_widget_active(const gui_t *gui, u64 id)
{
	return gui->active_id == id;
}

b32 gui_widget_focused(const gui_t *gui, u64 id)
{
	return gui->focus_id == id;
}

b32 gui_any_widget_active(const gui_t *gui)
{
	return gui->active_id != 0;
}

b32 gui_any_widget_has_focus(const gui_t *gui)
{
	return gui->focus_id != 0;
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
	split->flags = flags;
	split->vert = vert;
	split->leaf = false;
	split->sp1 = sp1;
	split->sp2 = sp2;
	split->sz = sz;
	sp1->parent = split;
	sp2->parent = split;
	sp1->leaf = true;
	sp2->leaf = true;
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
void gui__split(gui_t *gui, gui_split_t *split)
{
	s32 x, y, w, h, sz1;

	assert(split);
	assert(!split->leaf);

	box2i_to_xywh(split->box, &x, &y, &w, &h);

	/* checking use_default_cursor ensures only 1 resize happens per frame */
	if (split->flags & GUI_SPLIT_RESIZABLE && gui->use_default_cursor) {
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
			}
		}
		gui_style_pop(gui);
		gui_style_pop(gui);
	}

	sz1 = gui__split_dim(split->sz, split->vert ? w : h);
	split->sp1->box = split->box;
	split->sp2->box = split->box;
	if (split->vert) {
		split->sp1->box.max.x = split->box.min.x + sz1;
		split->sp2->box.min.x = split->box.min.x + sz1;
	} else {
		split->sp1->box.min.y = split->box.max.y - sz1;
		split->sp2->box.max.y = split->box.max.y - sz1;
	}

	if (!split->sp1->leaf)
		gui__split(gui, split->sp1);
	if (!split->sp2->leaf)
		gui__split(gui, split->sp2);
}


void pgui_panel_init(gui_t *gui, gui_panel_t *panel, s32 x, s32 y, s32 w, s32 h,
                     const char *title, gui_panel_flags_t flags)
{
	panel->x = x;
	panel->y = y;
	panel->width = w;
	panel->height = h;
	panel->id = ++gui->next_panel_id;
	assert(gui->next_panel_id != GUI__DEFAULT_PANEL_ID);

	panel->title = title;
	panel->flags = flags;
	assert(!(flags & GUI_PANEL_TITLEBAR) || title);

	panel->scroll = g_v2i_zero;
	panel->required_dim = g_v2i_zero;

	panel->parent = NULL;
	panel->split.leaf = false;

	panel->hidden = false;

	pgui_panel_to_front(gui, panel);
}

void pgui_panel(gui_t *gui, gui_panel_t *panel)
{
	v2i resize;
	s32 resize_delta;
	b32 dragging, contains_mouse;
	box2i box, box2;

	assert(panel->parent == gui->panel);

	gui->panel = panel;

	panel->hidden = false;

	if (panel->split.leaf)
		box2i_to_xywh(panel->split.box, &panel->x, &panel->y, &panel->width, &panel->height);

	if (gui->style.panel.padding >= 1.f) {
		panel->padding.x = gui->style.panel.padding;
		panel->padding.y = gui->style.panel.padding;
	} else {
	  panel->padding.x = panel->width * gui->style.panel.padding;
	  panel->padding.y = panel->height * gui->style.panel.padding;
	}

	/* resizing */

	if (panel->flags & GUI_PANEL_RESIZABLE
	    && !panel->split.leaf
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

	/* titlebar / dragging */

	if (panel->flags & GUI_PANEL_TITLEBAR) {
		const s32 dim = GUI_PANEL_TITLEBAR_HEIGHT;
		panel->pos.y = panel->y + panel->height - dim;
		u64 drag_id = ~0;

		if (panel->flags & GUI_PANEL_DRAGGABLE && !panel->split.leaf) {
			dragging = gui__drag_logic(gui, &drag_id, &panel->x, &panel->pos.y,
			                           dim, dim, MB_LEFT,
			                           gui__drag_default_callback, NULL);
			if (dragging)
				panel->y = panel->pos.y - panel->height + dim;
		} else {
			dragging = false;
		}

		// TODO(rgriege): active_color if at front?
		gui_rect(gui, panel->x, panel->pos.y, panel->width,
		         dim, gui->style.panel.titlebar.bg_color,
		         gui->style.panel.titlebar.outline_color);

		gui__drag_render(gui, panel->x, panel->pos.y, dim, dim, drag_id,
		                 &gui->style.panel.drag);

		gui_txt_styled(gui, panel->x + dim, panel->pos.y, panel->width, dim,
		               panel->title, &gui->style.panel.titlebar.text);

		if ((panel->flags & GUI_PANEL_CLOSABLE) && !panel->split.leaf) {
			const s32 outer_dim = GUI_PANEL_TITLEBAR_HEIGHT;
			const s32 inner_dim = outer_dim;
			const s32 x = panel->x + panel->width - outer_dim;
			const s32 y = panel->pos.y;
			const s32 w = inner_dim;
			const s32 h = inner_dim;
			gui_style_push(gui, btn, gui->style.panel.close);
			if (gui_btn_pen(gui, x, y, w, h, gui_pen_panel_close) == BTN_PRESS)
				panel->hidden = true;
			gui_style_pop(gui);
		}

		panel->pos.y -= panel->padding.y;
		panel->body_height = panel->height - dim;
	} else {
		dragging = false;
		panel->pos.y = panel->y + panel->height - panel->padding.y;
		panel->body_height = panel->height;
	}

	/* background outline display */
	gui_rect(gui, panel->x, panel->y, panel->width, panel->height,
	         g_nocolor, gui->style.panel.border_color);

	box2i_from_xywh(&box, panel->x, panel->y, panel->width, panel->height);
	contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                 && gui__box_visible(gui, box);
	if (   dragging
	    || (   gui__allow_new_panel_interaction(gui)
	        && mouse_pressed(gui, MB_LEFT)
	        && contains_mouse))
		pgui_panel_to_front(gui, panel);
	else
		panel->pri = ++gui->next_panel_pri;

	panel->pos.x = panel->x;
	panel->row.height = 0;
	panel->row.current_col = panel->row.cols;
	panel->row.num_cols = 0;

	panel->required_dim = v2i_scale(panel->padding, 2);

	panel->pos.y += panel->scroll.y;

	assert(gui->scissor_cnt > 0);
	gui__current_mask(gui, &box);
	box2i_from_xywh(&box2, panel->x + panel->padding.x,
	                panel->y + panel->padding.y,
	                panel->width - panel->padding.x * 2,
	                panel->body_height - panel->padding.y * 2);
	box2i_clamp_point(box, &box2.min);
	box2i_clamp_point(box, &box2.max);
	gui_mask(gui, box2.min.x, box2.min.y, box2.max.x - box2.min.x, box2.max.y - box2.min.y);
}

void pgui_panel_finish(gui_t *gui, gui_panel_t *panel)
{
	box2i box;
	v2i needed;
	s32 scroll, scroll_handle_sz;
	r32 slider_val;
	b32 contains_mouse;

	assert(gui->panel == panel);

	/* scrolling */

	box2i_from_xywh(&box, panel->x, panel->y, panel->width, panel->height);
	contains_mouse =    box2i_contains_point(box, gui->mouse_pos)
	                 && gui__box_visible(gui, box);
	if (panel->body_height < panel->required_dim.y) {
		needed.y = panel->required_dim.y - panel->body_height;
		if (   !key_mod(gui, KBM_SHIFT)
		    && !gui->mouse_covered_by_panel
		    && contains_mouse) {
			mouse_scroll(gui, &scroll);
			scroll *= -GUI_SCROLL_RATE;
			panel->scroll.y = clamp(0, panel->scroll.y + scroll, needed.y);
		} else {
			panel->scroll.y = clamp(0, panel->scroll.y, needed.y);
		}
		scroll_handle_sz = max(panel->padding.x / 2,   panel->body_height * panel->body_height
		                                             / panel->required_dim.y);
		slider_val = 1.f - ((r32)panel->scroll.y / needed.y);
		gui__slider(gui, panel->x, panel->y + panel->padding.y / 2, panel->padding.x / 2,
		            panel->body_height - panel->padding.y / 2, &slider_val,
		            scroll_handle_sz, GUI__SLIDER_Y);
		panel->scroll.y = -(slider_val - 1.f) * needed.y;
	} else {
		panel->scroll.y = 0;
	}

	if (panel->width < panel->required_dim.x) {
		needed.x = panel->required_dim.x - panel->width;
		if (   key_mod(gui, KBM_SHIFT)
		    && !gui->mouse_covered_by_panel
		    && contains_mouse) {
			mouse_scroll(gui, &scroll);
			scroll *= GUI_SCROLL_RATE;
			panel->scroll.x = clamp(0, panel->scroll.x + scroll, needed.x);
		} else {
			panel->scroll.x = clamp(0, panel->scroll.x, needed.x);
		}
		scroll_handle_sz = max(panel->padding.y / 2,   panel->width * panel->width
		                                             / panel->required_dim.x);
		slider_val = (r32)panel->scroll.x / needed.x;
		gui__slider(gui, panel->x + panel->padding.x / 2, panel->y,
		            panel->width - panel->padding.x / 2, panel->padding.y / 2, &slider_val,
		            scroll_handle_sz, GUI__SLIDER_X);
		panel->scroll.x = slider_val * needed.x;
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

	/* background display */
	/* NOTE(rgriege): would be great to avoid the additional mask here */
	gui_rect(gui, panel->x, panel->y, panel->width, panel->height,
	         gui->style.panel.bg_color, g_nocolor);
	gui_unmask(gui);

	if (panel->flags & GUI_PANEL_TITLEBAR)
		panel->body_height = panel->height - GUI_PANEL_TITLEBAR_HEIGHT;
	else
		panel->body_height = panel->height;

	if (contains_mouse)
		gui->mouse_covered_by_panel = true;

	gui->panel = panel->parent;
}

#ifdef DEBUG
static
b32 pgui__row_complete(const gui_panel_t *panel)
{
	return panel->row.current_col == panel->row.cols + panel->row.num_cols;
}
#endif

void pgui_row(gui_t *gui, u32 height, r32 width_ratio)
{
	pgui_row_cols(gui, height, &width_ratio, 1);
}

void pgui_row_cols(gui_t *gui, u32 height, const r32 *cols, u32 num_cols)
{
	u32 unspecified_width_col_cnt, unspecified_col_width;
	s32 row_width, inner_width;

	assert(gui->panel);
	assert(pgui__row_complete(gui->panel));
	assert(num_cols <= GUI_PANEL_MAX_COLS);

	pgui_row_xw(gui, &gui->panel->pos.x, &inner_width);
	gui->panel->pos.y -= height;
	gui->panel->row.height = height;

	gui->panel->row.current_col = gui->panel->row.cols;
	gui->panel->row.num_cols = num_cols;

	row_width = 0;
	unspecified_width_col_cnt = 0;
	for (u32 i = 0; i < num_cols; ++i) {
		if (cols[i] > 1.f) {
			gui->panel->row.cols[i] = cols[i];
			row_width += cols[i];
		} else if (cols[i] > 0.f) {
			gui->panel->row.cols[i] = cols[i] * inner_width;
			row_width += gui->panel->row.cols[i];
		} else {
			assert(cols[i] == 0.f);
			gui->panel->row.cols[i] = 0.f;
			++unspecified_width_col_cnt;
		}
	}
	if (unspecified_width_col_cnt) {
		unspecified_col_width =   max((inner_width - row_width), 0)
		                        / unspecified_width_col_cnt;
		for (u32 i = 0; i < num_cols; ++i)
			if (gui->panel->row.cols[i] == 0.f)
				gui->panel->row.cols[i] = unspecified_col_width;
		row_width += unspecified_width_col_cnt * unspecified_col_width;
	}

	gui->panel->required_dim.x = max(gui->panel->required_dim.x, row_width);
	gui->panel->required_dim.y += height;
}

static
void pgui__col_advance(gui_panel_t *panel)
{
	panel->pos.x += *panel->row.current_col;
	++panel->row.current_col;
}

void pgui_row_empty(gui_t *gui, u32 height)
{
	pgui_row(gui, height, 1.f);
	pgui__col_advance(gui->panel);
}

void pgui_row_break(gui_t *gui, u32 top_y, u32 bottom_y, r32 width_ratio)
{
	const s32 dx = gui->panel->width * (1.f - width_ratio) * 0.5f;

	pgui_row_empty(gui, top_y + bottom_y);

	gui_line(gui, gui->panel->x + dx, gui->panel->pos.y + bottom_y,
	         gui->panel->x + gui->panel->width - dx,
	         gui->panel->pos.y + bottom_y, 1, gui->style.panel.break_color);
}

void pgui_spacer(gui_t *gui)
{
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	gui_rect(gui, gui->panel->pos.x, gui->panel->pos.y,
	         *gui->panel->row.current_col, gui->panel->row.height,
	         gui->style.panel.cell_bg_color, gui->style.panel.cell_border_color);

	pgui__col_advance(gui->panel);
}

void pgui_txt(gui_t *gui, const char *str)
{
	const s32 x = gui->panel->pos.x;
	const s32 y = gui->panel->pos.y;
	const s32 w = *gui->panel->row.current_col;
	const s32 h = gui->panel->row.height;
	const gui_panel_style_t *style = &gui->style.panel;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	gui_rect(gui, x, y, w, h, style->cell_bg_color, style->cell_border_color);
	gui_txt_styled(gui, x, y, w, h, str, &gui->style.txt);
	pgui__col_advance(gui->panel);
}

void pgui_img(gui_t *gui, const char *fname, img_scale_t scale)
{
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	gui_img_boxed(gui, gui->panel->pos.x, gui->panel->pos.y,
	              *gui->panel->row.current_col, gui->panel->row.height,
	              fname, scale);
	pgui__col_advance(gui->panel);
}

btn_val_t pgui_btn_txt(gui_t *gui, const char *lbl)
{
	btn_val_t result;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	result = gui_btn_txt(gui, gui->panel->pos.x, gui->panel->pos.y,
	                     *gui->panel->row.current_col, gui->panel->row.height, lbl);
	pgui__col_advance(gui->panel);
	return result;
}

btn_val_t pgui_btn_img(gui_t *gui, const char *fname, img_scale_t scale)
{
	btn_val_t result;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	result = gui_btn_img(gui, gui->panel->pos.x, gui->panel->pos.y,
	                     *gui->panel->row.current_col, gui->panel->row.height,
	                     fname, scale);
	pgui__col_advance(gui->panel);
	return result;
}

btn_val_t pgui_btn_pen(gui_t *gui, gui_pen_t pen)
{
	btn_val_t result;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	result = gui_btn_pen(gui, gui->panel->pos.x, gui->panel->pos.y,
	                     *gui->panel->row.current_col, gui->panel->row.height, pen);
	pgui__col_advance(gui->panel);
	return result;
}

void pgui_chk(gui_t *gui, const char *lbl, b32 *val)
{
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	gui_chk(gui, gui->panel->pos.x, gui->panel->pos.y,
	        *gui->panel->row.current_col, gui->panel->row.height, lbl, val);
	pgui__col_advance(gui->panel);
}

b32 pgui_npt(gui_t *gui, char *lbl, u32 n, const char *hint, npt_flags_t flags)
{
	b32 result;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	result = gui_npt(gui, gui->panel->pos.x, gui->panel->pos.y,
	                 *gui->panel->row.current_col, gui->panel->row.height,
	                 lbl, n, hint, flags);
	pgui__col_advance(gui->panel);
	return result;
}

void pgui_select(gui_t *gui, const char *lbl, u32 *val, u32 opt)
{
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	gui_select(gui, gui->panel->pos.x, gui->panel->pos.y,
	            *gui->panel->row.current_col, gui->panel->row.height,
	            lbl, val, opt);
	pgui__col_advance(gui->panel);
}

void pgui_mselect(gui_t *gui, const char *txt, u32 *val, u32 opt)
{
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	gui_mselect(gui, gui->panel->pos.x, gui->panel->pos.y,
	             *gui->panel->row.current_col, gui->panel->row.height,
	             txt, val, opt);
	pgui__col_advance(gui->panel);
}

b32 pgui_slider_x(gui_t *gui, r32 *val)
{
	b32 ret;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	ret = gui_slider_x(gui, gui->panel->pos.x, gui->panel->pos.y,
	                   *gui->panel->row.current_col, gui->panel->row.height, val);
	pgui__col_advance(gui->panel);
	return ret;
}

b32 pgui_slider_y(gui_t *gui, r32 *val)
{
	b32 ret;

	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));

	ret = gui_slider_y(gui, gui->panel->pos.x, gui->panel->pos.y,
	                   *gui->panel->row.current_col, gui->panel->row.height, val);
	pgui__col_advance(gui->panel);
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
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));
	assert(!subpanel->split.leaf);

	subpanel->x = gui->panel->pos.x;
	subpanel->y = gui->panel->pos.y;
	subpanel->width = *gui->panel->row.current_col;
	subpanel->height = gui->panel->row.height;
	subpanel->parent = gui->panel;

	pgui_panel(gui, subpanel);
}

void pgui_subpanel_finish(gui_t *gui, gui_panel_t *subpanel)
{
	pgui_panel_finish(gui, subpanel);
	pgui__col_advance(gui->panel);
}

void pgui_panel_to_front(gui_t *gui, gui_panel_t *panel)
{
	panel->pri = --gui->min_panel_pri;
}

int pgui_panel_sort(const void *lhs_, const void *rhs_)
{
	const gui_panel_t *lhs = lhs_, *rhs = rhs_;
	return lhs->pri - rhs->pri;
}

void pgui_row_xw(gui_t *gui, s32 *x, s32 *w)
{
	assert(gui->panel);
	const s32 padding =   gui->style.panel.padding >= 1.f
	                    ? gui->style.panel.padding
	                    : gui->panel->width * gui->style.panel.padding;
	if (x)
		*x = gui->panel->x - gui->panel->scroll.x + padding;
	if (w)
		*w = gui->panel->width - 2 * padding;
}

void pgui_cell(gui_t *gui, s32 *x, s32 *y, s32 *w, s32 *h)
{
	assert(gui->panel);
	if (x)
		*x = gui->panel->pos.x;
	if (y)
		*y = gui->panel->pos.y;
	if (w)
		*w = (s32)*gui->panel->row.current_col;
	if (h)
		*h = (s32)gui->panel->row.height;
}

u64 pgui_next_widget_id(const gui_t *gui)
{
	assert(gui->panel);
	assert(!pgui__row_complete(gui->panel));
	return gui_widget_id(gui, gui->panel->pos.x, gui->panel->pos.y);
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

void gui_pen_panel_minimize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            const gui_element_style_t *style)
{
	gui_line_styled(gui, x + w / 4, y + 2 * h / 5, x + 3 * w / 4, y + 2 * h / 5,
	                &style->line);
}

void gui_pen_panel_maximize(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                            const gui_element_style_t *style)
{
	gui_rect(gui, x + w / 4, y + h / 4, w / 2, h / 2, g_nocolor, style->line.color);
}

void gui_pen_panel_close(gui_t *gui, s32 x, s32 y, s32 w, s32 h,
                         const gui_element_style_t *style)
{
	const s32 ix = x + w / 4;
	const s32 iy = y + h / 4;
	const s32 iw = w / 2;
	const s32 ih = h / 2;
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

void gui_style_push_(gui_t *gui, const void *value, size_t offset, size_t size)
{
	if (gui->style_stack_sz + 2 * sizeof(size_t) + size < GUI_STYLE_STACK_LIMIT) {
		const void *loc = ((u8*)&gui->style) + offset;
		memcpy(&gui->style_stack[gui->style_stack_sz], loc, size);
		gui->style_stack_sz += size;
		memcpy(&gui->style_stack[gui->style_stack_sz], &size, sizeof(size_t));
		gui->style_stack_sz += sizeof(size_t);
		memcpy(&gui->style_stack[gui->style_stack_sz], &offset, sizeof(size_t));
		gui->style_stack_sz += sizeof(size_t);
		memcpy(((u8*)&gui->style) + offset, value, size);
	} else {
		error("style stack limit exceeded");
	}
}

void gui_style_pop(gui_t *gui)
{
	if (gui->style_stack_sz > sizeof(size_t) + sizeof(size_t)) {
		size_t offset, size;
		gui->style_stack_sz -= sizeof(size_t);
		memcpy(&offset, &gui->style_stack[gui->style_stack_sz], sizeof(size_t));
		gui->style_stack_sz -= sizeof(size_t);
		memcpy(&size, &gui->style_stack[gui->style_stack_sz], sizeof(size_t));
		gui->style_stack_sz -= size;
		memcpy(((u8*)&gui->style) + offset,
		       &gui->style_stack[gui->style_stack_sz], size);
	} else {
		error("style stack empty");
	}
}

#define GUI_STYLE_STACK_PUSH_LITERAL(name, type) \
void gui_style_push_##name##_(gui_t *gui, size_t offset, type val) \
{ \
	gui_style_push_(gui, &val, offset, sizeof(val)); \
}

GUI_STYLE_STACK_PUSH_LITERAL(color, color_t);
GUI_STYLE_STACK_PUSH_LITERAL(u32, u32);
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

#undef GUI_IMPLEMENTATION
#endif // GUI_IMPLEMENTATION
