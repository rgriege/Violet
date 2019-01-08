#ifndef VIOLET_COLOR_H
#define VIOLET_COLOR_H

typedef union
{
	struct { u8 r, g, b, a; };
	struct { u8 d[4]; };
	struct { u32 val; };
} color_t;

typedef union
{
	struct { r32 r, g, b, a; };
	struct { r32 d[4]; };
} colorf_t;

#define gi_black        { .r=0,    .g=0,    .b=0,    .a=0xff }
#define gi_grey77       { .r=0x4d, .g=0x4d, .b=0x4d, .a=0xff }
#define gi_grey128      { .r=0x80, .g=0x80, .b=0x80, .a=0xff }
#define gi_white        { .r=0xff, .g=0xff, .b=0xff, .a=0xff }

#define gi_blue         { .r=0,    .g=0,    .b=0xff, .a=0xff }
#define gi_fuchsia      { .r=0xff, .g=0,    .b=0xff, .a=0xff }
#define gi_green        { .r=0,    .g=0xff, .b=0,    .a=0xff }
#define gi_lightblue    { .r=0x3f, .g=0xa8, .b=0xf5, .a=0xff }
#define gi_medblue      { .r=0x3f, .g=0x68, .b=0xf5, .a=0xff }
#define gi_orange       { .r=0xff, .g=0x92, .b=0x1e, .a=0xff }
#define gi_red          { .r=0xff, .g=0,    .b=0,    .a=0xff }
#define gi_yellow       { .r=0xff, .g=0xff, .b=0,    .a=0xff }
#define gi_cyan         { .r=0x00, .g=0xff, .b=0xff, .a=0xff }

#define gi_nocolor      { .r=0,    .g=0,    .b=0,    .a=0 }


#define g_black     (color_t)gi_black
#define g_grey77    (color_t)gi_grey77
#define g_grey128   (color_t)gi_grey128
#define g_white     (color_t)gi_white

#define g_blue      (color_t)gi_blue
#define g_fuchsia   (color_t)gi_fuchsia
#define g_green     (color_t)gi_green
#define g_lightblue (color_t)gi_lightblue
#define g_medblue   (color_t)gi_medblue
#define g_orange    (color_t)gi_orange
#define g_red       (color_t)gi_red
#define g_yellow    (color_t)gi_yellow
#define g_cyan      (color_t)gi_cyan

#define g_nocolor   (color_t)gi_nocolor

colorf_t color_to_colorf(color_t c);
color_t  colorf_to_color(colorf_t cf);
void     color_to_hex(color_t c, char *hex, u32 n);
b32      color_from_hex(const char *hex, color_t *c);
b32      color_equal(color_t lhs, color_t rhs);
b32      colorf_equal(colorf_t lhs, colorf_t rhs);
color_t  color_blend(color_t src, color_t dst);

void rgb_to_hsv(r32 r, r32 g, r32 b, r32 *h, r32 *s, r32 *v);
void hsv_to_rgb(r32 h, r32 s, r32 v, r32 *r, r32 *g, r32 *b);
void color_to_hsv(colorf_t c, r32 *h, r32 *s, r32 *v);
void hsv_to_color(r32 h, r32 s, r32 v, colorf_t *c);
void color_to_hsv8(color_t c, u8 *h, u8 *s, u8 *v);
void hsv_to_color8(u8 h, u8 s, u8 v, color_t *c);

#endif // VIOLET_COLOR_H

/* Implementation */

#ifdef COLOR_IMPLEMENTATION

colorf_t color_to_colorf(color_t c)
{
	return (colorf_t) {
		.r = (r32)c.r / 255.f,
		.g = (r32)c.g / 255.f,
		.b = (r32)c.b / 255.f,
		.a = (r32)c.a / 255.f,
	};
}

color_t colorf_to_color(colorf_t cf)
{
	return (color_t) {
		.r = (u8)(cf.r * 255.f),
		.g = (u8)(cf.g * 255.f),
		.b = (u8)(cf.b * 255.f),
		.a = (u8)(cf.a * 255.f),
	};
}

void color_to_hex(color_t c, char *hex, u32 n)
{
	snprintf(hex, n, "#%.2x%.2x%.2x%.2x", c.r, c.g, c.b, c.a);
}

b32 color_from_hex(const char *hex, color_t *c)
{
	u32 val;

	if (*hex == '#')
		++hex;

	val = strtoul(hex, NULL, 16);
	switch (strlen(hex)) {
	case 6:
		// no alpha
		c->r = (val >> 16) & 0xff;
		c->g = (val >> 8) & 0xff;
		c->b = val & 0xff;
		c->a = 255;
		return true;
	case 8:
		// with alpha
		c->r = (val >> 24) & 0xff;
		c->g = (val >> 16) & 0xff;
		c->b = (val >> 8) & 0xff;
		c->a = val & 0xff;
		return true;
	default:
		*c = g_nocolor;
		return false;
	}
}

b32 color_equal(color_t lhs, color_t rhs)
{
	return memcmp(&lhs, &rhs, sizeof(color_t)) == 0;
}

b32 colorf_equal(colorf_t lhs, colorf_t rhs)
{
	return memcmp(&lhs, &rhs, sizeof(colorf_t)) == 0;
}

color_t color_blend(color_t src, color_t dst)
{
	return (color_t) {
		.r = src.r / 2 + dst.r / 2,
		.g = src.g / 2 + dst.g / 2,
		.b = src.b / 2 + dst.b / 2,
		.a = src.a / 2 + dst.a / 2,
	};
}

void rgb_to_hsv(r32 r, r32 g, r32 b, r32 *h, r32 *s, r32 *v)
{
	const r32 rgb_min = fminf(r, fminf(g, b));
	const r32 rgb_max = fmaxf(r, fmaxf(g, b));
	const r32 rgb_del = rgb_max - rgb_min;

	*v = max(r, max(g, b));

	if (r == g && r == b) {
		/* gray, no chroma */
		*h = 0.f;
		*s = 0.f;
	} else {
		const r32 r_del = (((rgb_max - r) / 6.f) + (rgb_del / 2.f)) / rgb_del;
		const r32 g_del = (((rgb_max - g) / 6.f) + (rgb_del / 2.f)) / rgb_del;
		const r32 b_del = (((rgb_max - b) / 6.f) + (rgb_del / 2.f)) / rgb_del;

		*s = rgb_del / rgb_max;

		if (r == rgb_max)
			*h = 0.f/3.f + b_del - g_del;
		else if (g == rgb_max)
			*h = 1.f/3.f + r_del - b_del;
		else if (b == rgb_max)
			*h = 2.f/3.f + g_del - r_del;
	}
}

void hsv_to_rgb(r32 h, r32 s, r32 v, r32 *r, r32 *g, r32 *b)
{
	if (s == 0.f) {
		*r = v;
		*g = v;
		*b = v;
	} else {
		const r32 var_h = (h >= 1.f) ? 0.f : h * 6.f;
		const r32 var_i = floorf(var_h);
		const r32 var_1 = v * (1.f - s);
		const r32 var_2 = v * (1.f - s * (var_h - var_i));
		const r32 var_3 = v * (1.f - s * (1.f - (var_h - var_i)));

		switch ((int)var_i) {
		case 0:
			*r = v;
			*g = var_3;
			*b = var_1;
		break;
		case 1:
			*r = var_2;
			*g = v;
			*b = var_1;
		break;
		case 2:
			*r = var_1;
			*g = v;
			*b = var_3;
		break;
		case 3:
			*r = var_1;
			*g = var_2;
			*b = v;
		break;
		case 4:
			*r = var_3;
			*g = var_1;
			*b = v;
		break;
		default:
			*r = v;
			*g = var_1;
			*b = var_2;
		break;
		}
	}
}

void color_to_hsv(colorf_t c, r32 *h, r32 *s, r32 *v)
{
	rgb_to_hsv(c.r, c.g, c.b, h, s, v);
}

void hsv_to_color(r32 h, r32 s, r32 v, colorf_t *c)
{
	hsv_to_rgb(h, s, v, &c->r, &c->g, &c->b);
}

void color_to_hsv8(color_t c, u8 *h, u8 *s, u8 *v)
{
	r32 hf, sf, vf;
	color_to_hsv(color_to_colorf(c), &hf, &sf, &vf);
	*h = (u8)(255.f*hf);
	*s = (u8)(255.f*sf);
	*v = (u8)(255.f*vf);
}

void hsv_to_color8(u8 h, u8 s, u8 v, color_t *c)
{
	const r32 hf = (r32)h/255.f;
	const r32 sf = (r32)s/255.f;
	const r32 vf = (r32)v/255.f;
	colorf_t cf = { .a=(r32)c->a/255.f };
	hsv_to_color(hf, sf, vf, &cf);
	*c = colorf_to_color(cf);
}

#undef COLOR_IMPLEMENTATION
#endif // COLOR_IMPLEMENTATION
