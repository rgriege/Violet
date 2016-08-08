#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core/types.h"
#include "violet/gui/color.h"
#include "violet/gui/font.h"
#include "violet/gui/mesh.h"

typedef struct vlt_img vlt_img;
typedef struct vlt_gui vlt_gui;

vlt_gui *vlt_gui_create(s32 x, s32 y, s32 w, s32 h, const char *title);
void vlt_gui_destroy(vlt_gui *gui);

typedef enum vlt_mb
{
	VLT_MB_LEFT = 1 << 0,
	VLT_MB_MIDDLE = 1 << 1,
	VLT_MB_RIGHT = 1 << 2,
	VLT_MB_X1 = 1 << 3,
	VLT_MB_X2 = 1 << 4,
	VLT_MB_WHEELUP = 1 << 5,
	VLT_MB_WHEELDOWN = 1 << 6
} vlt_mb;

void vlt_gui_mouse_pos(const vlt_gui *gui, s32 *x, s32 *y);
b8 vlt_gui_mouse_press(const vlt_gui *gui, u32 mask);
b8 vlt_gui_mouse_down(const vlt_gui *gui, u32 mask);
b8 vlt_gui_mouse_release(const vlt_gui *gui, u32 mask);
b8 vlt_gui_mouse_release_bg(const vlt_gui *gui, u32 mask);
void vlt_gui_mouse_scroll(const vlt_gui *gui, s32 *dir);

char vlt_gui_key(const vlt_gui *gui);

b8 vlt_gui_begin_frame(vlt_gui *gui);
void vlt_gui_end_frame(vlt_gui *gui);


/* Retained Mode API */

typedef struct vlt_rmgui_poly
{
	vlt_mesh mesh;
	u32 vao;
	vlt_color fill_color;
	vlt_color line_color;
} vlt_rmgui_poly;

void vlt_rmgui_poly_init(vlt_gui *gui, const v2f *v, u32 n,
                         vlt_mesh *mesh, u32 *vao);
void vlt_rmgui_line_init(vlt_gui *gui, s32 x0, s32 y0, s32 x1, s32 y1,
                         s32 w, vlt_color c, vlt_rmgui_poly *line);
void vlt_rmgui_rect_init(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                         vlt_color fill, vlt_color line, vlt_rmgui_poly *rect);
void vlt_rmgui_circ_init(vlt_gui *gui, s32 x, s32 y, s32 r, vlt_color fill,
                         vlt_color line, vlt_rmgui_poly *circ);
void vlt_rmgui_poly_draw(vlt_gui *gui, const vlt_rmgui_poly *poly,
                         s32 x, s32 y);
void vlt_rmgui_poly_destroy(vlt_rmgui_poly *poly);

void vlt_rmgui_img_draw(vlt_gui *gui, vlt_img *img, s32 x, s32 y);


/* Immediate Mode API */

void vlt_gui_line(vlt_gui *gui, s32 x0, s32 y0, s32 x1, s32 y1, s32 w,
                  vlt_color c);
void vlt_gui_rect(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                  vlt_color fill, vlt_color line);
void vlt_gui_circ(vlt_gui *gui, s32 x, s32 y, s32 r,
                  vlt_color fill, vlt_color line);
void vlt_gui_poly(vlt_gui *gui, const v2f *v, u32 n,
                  vlt_color fill, vlt_color line);
void vlt_gui_img(vlt_gui *gui, s32 x, s32 y, const char *img);
void vlt_gui_txt(vlt_gui *gui, s32 x, s32 y, s32 sz, const char *txt,
                 font_align align);
void vlt_gui_mask(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h);
void vlt_gui_unmask(vlt_gui *gui);
void vlt_gui_npt(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                 char *txt, u32 n, font_align align);
b8 vlt_gui_btn(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, const char *txt);
void vlt_gui_chk(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, const char *txt,
                 b8 *val);
void vlt_gui_slider(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h, r32 *val);
void vlt_gui_select(vlt_gui *gui, s32 x, s32 y, s32 w, s32 h,
                    const char *txt, u32 *val, u32 opt);


typedef struct vlt_style
{
	vlt_color bg_color;
	vlt_color fill_color;
	vlt_color hot_color;
	vlt_color active_color;
	vlt_color outline_color;
	vlt_color hot_line_color;
	vlt_color active_line_color;
	vlt_color text_color;
	vlt_color hot_text_color;
	vlt_color active_text_color;
	r32 font_ratio;
} vlt_style;

vlt_style *vlt_gui_get_style(vlt_gui *gui);
void vlt_gui_style(vlt_gui *gui, const vlt_style *style);
void vlt_gui_style_default(vlt_gui *gui);

#endif

