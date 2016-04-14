#ifndef VIOLET_GUI_H
#define VIOLET_GUI_H

#include "violet/core/types.h"
#include "violet/gui/color.h"

typedef struct vlt_gui vlt_gui;

vlt_gui * vlt_gui_create();
void vlt_gui_destroy(vlt_gui * gui);

b8 vlt_gui_init_window(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h,
                       vlt_color c, const char * title);
void vlt_gui_destroy_window(vlt_gui * gui);

void vlt_gui_rect(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h, vlt_color c);
void vlt_gui_img(vlt_gui * gui, s32 x, s32 y, const char * img);
void vlt_gui_txt(vlt_gui * gui, s32 x, s32 y, s32 h, const char * txt,
                 vlt_color c);
b8 vlt_gui_btn(vlt_gui * gui, s32 x, s32 y, s32 w, s32 h, vlt_color c);

b8 vlt_gui_begin_frame(vlt_gui * gui);
void vlt_gui_end_frame(vlt_gui * gui);

#endif

