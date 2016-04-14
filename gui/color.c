#include <stdlib.h>
#include <stdio.h>

#include "violet/gui/color.h"

const vlt_color g_black = { .r = 0, .g = 0, .b = 0, .a = 255 };
const vlt_color g_white = { .r = 255, .g = 255, .b = 255, .a = 255 };

void vlt_color_as_float_array(float * f, vlt_color c)
{
	f[0] = c.r / 255.f;
	f[1] = c.g / 255.f;
	f[2] = c.b / 255.f;
	f[3] = c.a / 255.f;
}

void vlt_color_from_hex(vlt_color * c, const char * hex)
{
	if (*hex == '#')
		++hex;

	const u32 val = strtoul(hex, NULL, 16);
	printf("hex val = %x\n", val);
	if (val <= 0xffffff)
	{
		// no alpha
		printf("no alpha\n");
		c->r = (val >> 16) & 0xff;
		c->g = (val >> 8) & 0xff;
		c->b = val & 0xff;
		c->a = 255;
	}
	else
	{
		// with alpha
		c->r = (val >> 24) & 0xff;
		c->g = (val >> 16) & 0xff;
		c->b = (val >> 8) & 0xff;
		c->a = val & 0xff;
	}
}

