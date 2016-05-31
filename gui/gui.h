#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core/types.h"
#include "violet/gui/color.h"
#include "violet/gui/font.h"
#include "violet/gui/mesh.h"

typedef struct vlt_img vlt_img;
typedef struct vlt_gui vlt_gui;

vlt_gui * vlt_gui_create();
void vlt_gui_destroy(vlt_gui * gui);

b8 vlt_gui_init_window(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h,
                       vlt_color c, const char * title);
void vlt_gui_destroy_window(vlt_gui * gui);

void vlt_gui_mouse_pos(vlt_gui * gui, s32 * x, s32 * y);
void vlt_gui_mouse_scroll(vlt_gui * gui, s32 * dir);

b8 vlt_gui_begin_frame(vlt_gui * gui);
void vlt_gui_end_frame(vlt_gui * gui);


/* Retained Mode API */

typedef struct vlt_rmgui_poly
{
	vlt_mesh mesh;
	u32 vao;
	vlt_color fill_color;
	vlt_color line_color;
} vlt_rmgui_poly;

void vlt_rmgui_poly_init(vlt_gui * gui, array * vertices,
                         vlt_mesh * mesh, u32 * vao);
void vlt_rmgui_line_init(vlt_gui * gui, s32 x0, s32 y0, s32 x1, s32 y1, u32 sz,
                         vlt_color c, vlt_rmgui_poly * line);
void vlt_rmgui_rect_init(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h,
                         vlt_color fill, vlt_color line, vlt_rmgui_poly * rect);
void vlt_rmgui_poly_draw(vlt_gui * gui, const vlt_rmgui_poly * poly, s32 x, s32 y);
void vlt_rmgui_poly_destroy(vlt_rmgui_poly * poly);

void vlt_rmgui_img_draw(vlt_gui * gui, vlt_img * img, s32 x, s32 y);


/* Immediate Mode API */

void vlt_gui_line(vlt_gui * gui, s32 x0, s32 y0, s32 x1, s32 y1, u32 sz,
                  vlt_color c);
void vlt_gui_rect(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h, vlt_color c,
                  vlt_color lc);
void vlt_gui_img(vlt_gui * gui, s32 x, s32 y, const char * img);
void vlt_gui_txt(vlt_gui * gui, s32 x, s32 y, s32 sz, const char * txt,
                 vlt_color c, font_align align);
void vlt_gui_npt(vlt_gui * gui, s32 x, s32 y, s32 sz, char * txt, u32 n,
                 vlt_color c, font_align align);
b8 vlt_gui_btn(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h, vlt_color c,
               vlt_color lc);
b8 vlt_gui_btn_invis(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h);

#endif

