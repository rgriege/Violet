#ifndef VIOLET_FONT_H
#define VIOLET_FONT_H

#include "violet/core/types.h"

typedef enum font_align
{
	FONT_ALIGN_LEFT,
	FONT_ALIGN_CENTER,
	FONT_ALIGN_RIGHT
} font_align;

typedef struct vlt_font vlt_font;
typedef struct vlt_shader_program vlt_shader_program;

b8 vlt_font_install();
b8 vlt_font_uninstall();

vlt_font * vlt_font_create();
void vlt_font_free(vlt_font * f);

b8 vlt_font_load(vlt_font * f, const char * filename, u32 sz);
void vlt_font_destroy(vlt_font * f);

void vlt_font_render(vlt_font * f, const char * txt, s32 x, s32 y,
                     vlt_shader_program * p, font_align align);

#endif
