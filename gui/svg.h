#ifndef VIOLET_SVG_H
#define VIOLET_SVG_H

#include "violet/core/types.h"
#include "violet/gui/color.h"
#include "violet/math/aabb.h"
#include "violet/structures/array.h"
#include "violet/structures/array_map.h"

typedef struct vlt_gui vlt_gui;


#define SVG_TEXT_BUF_SZ 32


typedef struct vlt_str
{
	char *str;
	size_t sz;
} vlt_str;

void vlt_str_realloc(vlt_str *s, size_t n);


typedef struct svg_line
{
	s32 x0, y0, x1, y1;
	vlt_color color;
} svg_line;


typedef struct svg_rect
{
	s32 x, y, w, h;
	vlt_color fill_color;
	vlt_color line_color;
} svg_rect;


typedef struct svg_btn
{
	svg_rect rect;
	char * hook;
	char * params;
} svg_btn;

void svg_btn_init(svg_btn * b);
void svg_btn_destroy(svg_btn * b);


typedef struct svg_img
{
	s32 x, y;
	char * src;
} svg_img;

void svg_img_init(svg_img * i);
void svg_img_destroy(svg_img * i);


typedef enum svg_text_type
{
	SVG_TEXT_STATIC,
	SVG_TEXT_DYNAMIC,
} svg_text_type;

typedef struct svg_text
{
	s32 x, y, sz;
	svg_text_type type;
	char * txt;
	char * hook;
	char * params;
	vlt_color color;
	font_align align;
} svg_text;

void svg_text_init(svg_text * t);
void svg_text_destroy(svg_text * t);


typedef struct svg_symbol
{
	array lines;
	array rects;
	array texts;
	array btns;
	array imgs;
} svg_symbol;

void svg_symbol_init(svg_symbol * s);
void svg_symbol_destroy(svg_symbol * s);


typedef struct svg_symbol_ref
{
	s32 x, y;
	char * params;
	u32 symbol_id;
	const svg_symbol * symbol;
} svg_symbol_ref;

void svg_symbol_ref_init(svg_symbol_ref * sref);
void svg_symbol_ref_destroy(svg_symbol_ref * sref);


typedef struct svg_layer
{
	svg_symbol global_elems;
	array symbol_refs;
} svg_layer;

void svg_layer_init(svg_layer * l);
void svg_layer_destroy(svg_layer * l);

typedef struct vlt_svg
{
	aabb window;
	array_map symbols;
	array layers;
	vlt_str _str_buffer;
} vlt_svg;

void vlt_svg_init(vlt_svg * g);
b8 vlt_svg_init_from_file(vlt_svg * g, const char * filename);
void vlt_svg_destroy(vlt_svg * g);

void vlt_svg_render(vlt_gui * gui, vlt_svg * s, void * state,
                    array_map * text_hooks, array_map * btn_hooks);


/* Use the Retained-Mode gui API */

typedef struct vlt_rsvg vlt_rsvg;

vlt_rsvg * vlt_svg_retain(vlt_svg * svg, vlt_gui * gui);
void vlt_rsvg_destroy(vlt_rsvg * rsvg);
void vlt_rsvg_render(vlt_gui * gui, vlt_rsvg * s, void * state,
                     array_map * text_hooks, array_map * btn_hooks);

#endif

