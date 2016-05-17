#include <stdlib.h>
#include <string.h>

#include "violet/gui/color.h"
#include "violet/utility/log.h"

const vlt_color g_black = { .r = 0, .g = 0, .b = 0, .a = 255 };
const vlt_color g_white = { .r = 255, .g = 255, .b = 255, .a = 255 };
const vlt_color g_red = { .r = 255, .g = 0, .b = 0, .a = 255 };
const vlt_color g_green = { .r = 0, .g = 255, .b = 0, .a = 255 };
const vlt_color g_blue = { .r = 0, .g = 0, .b = 255, .a = 255 };
const vlt_color g_grey128 = { .r = 128, .g = 128, .b = 128, .a = 255 };
const vlt_color g_nocolor = { .r = 0, .g = 0, .b = 0, .a = 0 };

void vlt_color_as_float_array(float * f, vlt_color c)
{
	f[0] = c.r / 255.f;
	f[1] = c.g / 255.f;
	f[2] = c.b / 255.f;
	f[3] = c.a / 255.f;
}

vlt_color vlt_color_from_hex(const char * hex)
{
	vlt_color c = g_nocolor;
	if (*hex == '#')
		++hex;

	const u32 val = strtoul(hex, NULL, 16);
	const u32 len = strlen(hex);
	switch (len)
	{
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
		log_write("invalid color string '%s'", hex);
		break;
	}

	return c;
}

b8 vlt_color_equal(vlt_color lhs, vlt_color rhs)
{
	return memcmp(&lhs, &rhs, sizeof(vlt_color)) == 0;
}
