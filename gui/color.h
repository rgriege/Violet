#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

#include "violet/core/types.h"

typedef struct vlt_color
{
	u8 r, g, b, a;
} vlt_color;

#define gi_black        { .r=0,    .g=0,    .b=0,    .a=0xff }
#define gi_grey77       { .r=0x4d, .g=0x4d, .b=0x4d, .a=0xff }
#define gi_grey128      { .r=0x80, .g=0x80, .b=0x80, .a=0xff }
#define gi_white        { .r=0xff, .g=0xff, .b=0xff, .a=0xff }

#define gi_red          { .r=0xff, .g=0,    .b=0,    .a=0xff }
#define gi_orange       { .r=0xff, .g=0x92, .b=0x1e, .a=0xff }
#define gi_green        { .r=0,    .g=0xff, .b=0,    .a=0xff }
#define gi_lightblue    { .r=0x3f, .g=0xa8, .b=0xf5, .a=0xff }
#define gi_blue         { .r=0,    .g=0,    .b=0xff, .a=0xff }
#define gi_fuchsia      { .r=0xff, .g=0,    .b=0xff, .a=0xff }

#define gi_nocolor      { .r=0,    .g=0,    .b=0,    .a=0 }


#define g_black (vlt_color)gi_black
#define g_grey77 (vlt_color)gi_grey77
#define g_grey128 (vlt_color)gi_grey128
#define g_white (vlt_color)gi_white

#define g_red (vlt_color)gi_red
#define g_orange (vlt_color)gi_orange
#define g_green (vlt_color)gi_green
#define g_lightblue (vlt_color)gi_lightblue
#define g_blue (vlt_color)gi_blue
#define g_fuchsia (vlt_color)gi_fuchsia

#define g_nocolor (vlt_color)gi_nocolor

void vlt_color_as_float_array(float *f, vlt_color c);
vlt_color vlt_color_from_hex(const char *hex);

b8 vlt_color_equal(vlt_color lhs, vlt_color rhs);

#endif

