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

/* Unfortunately, for MSVC compliance, we need 2 variants for color constants.
 * 'gi_' stands for global initializer, and should only be used when
 * statically initializing a color_t struct member at compile time. */

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


extern const color_t g_black;
extern const color_t g_grey77;
extern const color_t g_grey128;
extern const color_t g_white;

extern const color_t g_blue;
extern const color_t g_fuchsia;
extern const color_t g_green;
extern const color_t g_lightblue;
extern const color_t g_medblue;
extern const color_t g_orange;
extern const color_t g_red;
extern const color_t g_yellow;
extern const color_t g_cyan;

extern const color_t g_nocolor;

colorf_t color_to_colorf(color_t c);
color_t  colorf_to_color(colorf_t cf);
void     color_to_hex(color_t c, char *hex, u32 n);
b32      color_from_hex(const char *hex, color_t *c);
b32      color_equal(color_t lhs, color_t rhs);
b32      colorf_equal(colorf_t lhs, colorf_t rhs);
color_t  color_blend(color_t src, color_t dst);
colorf_t colorf_mix(colorf_t lhs, colorf_t rhs, r32 t);
colorf_t colorf_darken(colorf_t c, r32 factor);
color_t  color_darken(color_t c, r32 factor);
r32      colorf_brightness(colorf_t c);
r32      colorf_luminance(colorf_t c);
color_t  color_fade(color_t c, u8 alpha);

void rgb_to_hsv(r32 r, r32 g, r32 b, r32 *h, r32 *s, r32 *v);
void hsv_to_rgb(r32 h, r32 s, r32 v, r32 *r, r32 *g, r32 *b);
void color_to_hsv(colorf_t c, r32 *h, r32 *s, r32 *v);
void hsv_to_color(r32 h, r32 s, r32 v, colorf_t *c);
void color_to_hsv8(color_t c, u8 *h, u8 *s, u8 *v);
void hsv_to_color8(u8 h, u8 s, u8 v, color_t *c);

#endif // VIOLET_COLOR_H

/* Implementation */

#ifdef COLOR_IMPLEMENTATION

const color_t g_black     = gi_black;
const color_t g_grey77    = gi_grey77;
const color_t g_grey128   = gi_grey128;
const color_t g_white     = gi_white;

const color_t g_blue      = gi_blue;
const color_t g_fuchsia   = gi_fuchsia;
const color_t g_green     = gi_green;
const color_t g_lightblue = gi_lightblue;
const color_t g_medblue   = gi_medblue;
const color_t g_orange    = gi_orange;
const color_t g_red       = gi_red;
const color_t g_yellow    = gi_yellow;
const color_t g_cyan      = gi_cyan;

const color_t g_nocolor   = gi_nocolor;

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

	val = (u32)strtoul(hex, NULL, 16);
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

colorf_t colorf_mix(colorf_t lhs, colorf_t rhs, r32 t)
{
	if (t == 1.f)
		return lhs;
	else if (t == 0.f)
		return rhs;
	else {
		const r32 ti = 1.f - t;
		return (colorf_t) {
			.r = lhs.r * t + rhs.r * ti,
			.g = lhs.g * t + rhs.g * ti,
			.b = lhs.b * t + rhs.b * ti,
			.a = lhs.a * t + rhs.a * ti,
		};
	}
}

colorf_t colorf_darken(colorf_t c, r32 factor)
{
	return (colorf_t) {
		.r = c.r * factor,
		.g = c.g * factor,
		.b = c.b * factor,
		.a = c.a,
	};
}

color_t color_darken(color_t c, r32 factor)
{
	return colorf_to_color(colorf_darken(color_to_colorf(c), factor));
}

r32 colorf_brightness(colorf_t c)
{
	return (c.r + c.g + c.b) / 3.f;
}

/* https://en.wikipedia.org/wiki/SRGB#The_reverse_transformation */
r32 srgb_gamma_corrected(r32 c)
{
	if (c <= 0.03928f)
		return c / 12.92f;
	else
		return powf((c + 0.055f) / 1.055f, 2.4f);
}

/* https://www.w3.org/TR/WCAG20/#relativeluminancedef
 * (the Y channel of CEI XYZ color space) */
r32 colorf_luminance(colorf_t c)
{
	return 0.2126f * srgb_gamma_corrected(c.r)
	     + 0.7152f * srgb_gamma_corrected(c.g)
	     + 0.0722f * srgb_gamma_corrected(c.b);
}

color_t color_fade(color_t c, u8 alpha)
{
	return (color_t){ .r = c.r, .g = c.g, .b = c.b, .a = alpha };
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
		else
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
