#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

#include "violet/core/types.h"

typedef struct vlt_color
{
	u8 r, g, b, a;
} vlt_color;

extern const vlt_color g_black;
extern const vlt_color g_white;

void vlt_color_as_float_array(float * f, vlt_color c);
void vlt_color_from_hex(vlt_color * c, const char * hex);

#endif

