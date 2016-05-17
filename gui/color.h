#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

#include "violet/core/types.h"

typedef struct vlt_color
{
	u8 r, g, b, a;
} vlt_color;

extern const vlt_color g_black;
extern const vlt_color g_white;
extern const vlt_color g_red;
extern const vlt_color g_green;
extern const vlt_color g_blue;
extern const vlt_color g_grey128;
extern const vlt_color g_nocolor;

void vlt_color_as_float_array(float * f, vlt_color c);
vlt_color vlt_color_from_hex(const char * hex);

b8 vlt_color_equal(vlt_color lhs, vlt_color rhs);

#endif

