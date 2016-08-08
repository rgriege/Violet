#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

#include "violet/core/types.h"

typedef struct vlt_color
{
	u8 r, g, b, a;
} vlt_color;

#define g_black (vlt_color)     { .r=0,    .g=0,    .b=0,    .a=0xff }
#define g_grey128 (vlt_color)   { .r=0x80, .g=0x80, .b=0x80, .a=0xff }
#define g_white (vlt_color)     { .r=0xff, .g=0xff, .b=0xff, .a=0xff }

#define g_red (vlt_color)       { .r=0xff, .g=0,    .b=0,    .a=0xff }
#define g_orange (vlt_color)    { .r=0xff, .g=0x92, .b=0x1e, .a=0xff }
#define g_green (vlt_color)     { .r=0,    .g=0xff, .b=0,    .a=0xff }
#define g_lightblue (vlt_color) { .r=0x3f, .g=0xa8, .b=0xf5, .a=0xff }
#define g_blue (vlt_color)      { .r=0,    .g=0,    .b=0xff, .a=0xff }
#define g_fuchsia (vlt_color)   { .r=0xff, .g=0,    .b=0xff, .a=0xff }

#define g_nocolor (vlt_color)   { .r=0,    .g=0,    .b=0,    .a=0 }

void vlt_color_as_float_array(float *f, vlt_color c);
vlt_color vlt_color_from_hex(const char *hex);

b8 vlt_color_equal(vlt_color lhs, vlt_color rhs);

#endif

