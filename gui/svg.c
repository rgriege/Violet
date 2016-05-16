#include <stdlib.h>
#include <string.h>

#include "ezxml/ezxml.h"

#include "violet/gui/color.h"
#include "violet/gui/gui.h"
#include "violet/gui/svg.h"
#include "violet/math/aabb.h"
#include "violet/math/v2.h"
#include "violet/utility/hash.h"
#include "violet/utility/log.h"
#include "violet/structures/array.h"
#include "violet/structures/array_map.h"


#define GUI_TEXT_BUF_SZ 20


typedef struct gui_line
{
	s32 x0, y0, x1, y1;
	vlt_color color;
} gui_line;


typedef struct gui_rect
{
	s32 x, y, w, h;
	vlt_color fill_color;
	vlt_color line_color;
} gui_rect;


typedef struct gui_btn
{
	gui_rect rect;
	char * hook;
	char * params;
} gui_btn;

void gui_btn_init(gui_btn * b)
{
	b->hook = malloc(GUI_TEXT_BUF_SZ);
	b->params = NULL;
}

void gui_btn_destroy(gui_btn * b)
{
	free(b->hook);
	if (b->params)
		free(b->params);
}


typedef enum gui_text_type
{
	GUI_TEXT_STATIC,
	GUI_TEXT_DYNAMIC,
} gui_text_type;

typedef struct gui_text
{
	s32 x, y, sz;
	gui_text_type type;
	char * txt;
	char * hook;
	char * params;
	vlt_color color;
	font_align align;
} gui_text;

void gui_text_init(gui_text * t)
{
	t->x = t->y = t->sz = 0;
	t->type = GUI_TEXT_STATIC;
	t->txt = malloc(20);
	t->hook = NULL;
	t->params = NULL;
	t->color = g_black;
	t->align = FONT_ALIGN_LEFT;
}

void gui_text_destroy(gui_text * t)
{
	free(t->txt);
	if (t->hook)
		free(t->hook);
	if (t->params)
		free(t->params);
}


typedef struct gui_symbol
{
	array lines;
	array rects;
	array texts;
	array btns;
} gui_symbol;

void gui_symbol_init(gui_symbol * s)
{
	array_init(&s->lines, sizeof(gui_line));
	array_init(&s->rects, sizeof(gui_rect));
	array_init(&s->texts, sizeof(gui_text));
	array_init(&s->btns, sizeof(gui_btn));
}

void gui_symbol_destroy(gui_symbol * s)
{
	array_destroy(&s->lines);
	array_destroy(&s->rects);
	array_destroy_each(&s->texts, (void(*)(void*))gui_text_destroy);
	array_destroy_each(&s->btns, (void(*)(void*))gui_btn_destroy);
}


typedef struct gui_symbol_ref
{
	s32 x, y;
	char * params;
	const gui_symbol * symbol;
} gui_symbol_ref;

void gui_symbol_ref_init(gui_symbol_ref * sref)
{
	sref->params = NULL;
}

void gui_symbol_ref_destroy(gui_symbol_ref * sref)
{
	if (sref->params)
		free(sref->params);
}


typedef struct gui_layer
{
	array lines;
	array rects;
	array texts;
	array btns;
	array symbol_refs;
} gui_layer;

void gui_layer_init(gui_layer * l)
{
	array_init(&l->lines, sizeof(gui_line));
	array_init(&l->rects, sizeof(gui_rect));
	array_init(&l->texts, sizeof(gui_text));
	array_init(&l->btns, sizeof(gui_btn));
	array_init(&l->symbol_refs, sizeof(gui_symbol_ref));
}

void gui_layer_destroy(gui_layer * l)
{
	array_destroy(&l->lines);
	array_destroy(&l->rects);
	array_destroy_each(&l->texts, (void(*)(void*))gui_text_destroy);
	array_destroy_each(&l->btns, (void(*)(void*))gui_btn_destroy);
	array_destroy_each(&l->symbol_refs, (void(*)(void*))gui_symbol_ref_destroy);
}


b8 svg_pos(ezxml_t node, s32 * x, s32 * y)
{
	const char * x_attr = ezxml_attr(node, "x");
	const char * y_attr = ezxml_attr(node, "y");
	if (x_attr && y_attr)
	{
		*x = atoi(x_attr);
		*y = atoi(y_attr);
		return true;
	}

	const char * transform_attr = ezxml_attr(node, "transform");
	if (transform_attr)
	{
		const char * p = transform_attr;
		while (*p != '(')
			++p;
		++p;

		char * end;
		for (u32 i = 0; i < 4; ++i)
		{
			strtof(p, &end);
			p = end;
		}

		*x = strtof(p, &end);
		p = end;
		*y = strtof(p, &end);
		return true;
	}

	return false;
}

b8 svg_line(gui_line * l, ezxml_t node)
{
	const char * x0_attr = ezxml_attr(node, "x0");
	const char * y0_attr = ezxml_attr(node, "y0");
	const char * x1_attr = ezxml_attr(node, "x1");
	const char * y1_attr = ezxml_attr(node, "y1");
	const char * stroke_attr = ezxml_attr(node, "stroke");
	l->color = stroke_attr ? vlt_color_from_hex(stroke_attr) : g_nocolor;
	if (x0_attr && y0_attr && x1_attr && y1_attr && l->color.a != 0)
	{
		l->x0 = atoi(x0_attr);
		l->y0 = atoi(y0_attr);
		l->x1 = atoi(x1_attr);
		l->y1 = atoi(y1_attr);
		return l->x0 != l->x1 || l->y0 != l->y1;
	}
	return false;
}

void svg_lines(array * lines, ezxml_t container)
{
	for (ezxml_t line = ezxml_child(container, "line"); line; line = line->next)
	{
		gui_line * l = array_append_null(lines);
		if (!svg_line(l, line))
		{
			log_write("invalid line");
			array_remove(lines, array_size(lines) - 1);
		}
	}
}

b8 svg_rect(gui_rect * r, ezxml_t node)
{
	if (!svg_pos(node, &r->x, &r->y))
		r->x = r->y = 0;
	const char * width_attr = ezxml_attr(node, "width");
	r->w = width_attr ? atoi(width_attr) : 0;
	const char * height_attr = ezxml_attr(node, "height");
	r->h = height_attr ? atoi(height_attr) : 0;
	const char * color_attr = ezxml_attr(node, "fill");
	r->fill_color = color_attr ? vlt_color_from_hex(color_attr) : g_nocolor;
	const char * stroke_attr = ezxml_attr(node, "stroke");
	r->line_color = stroke_attr ? vlt_color_from_hex(stroke_attr) : g_nocolor;
	const char * opacity_attr = ezxml_attr(node, "opacity");
	if (opacity_attr)
		r->fill_color.a = strtof(opacity_attr, NULL) * 255;

	return r->w != 0 && r->h != 0 && (r->fill_color.a != 0 || r->line_color.a != 0);
}

void svg_rects(array * rects, ezxml_t container)
{
	for (ezxml_t rect = ezxml_child(container, "rect"); rect; rect = rect->next)
	{
		gui_rect * r = array_append_null(rects);
		if (!svg_rect(r, rect))
		{
			log_write("invalid rect");
			array_remove(rects, array_size(rects) - 1);
		}
	}
}

b8 svg_btn(gui_btn * b, ezxml_t node)
{
	if (!svg_rect(&b->rect, node))
		return false;

	const char * hook_attr = ezxml_attr(node, "hook");
	if (!hook_attr)
		return false;

	const char * params_attr = ezxml_attr(node, "params");
	if (params_attr)
	{
		b->params = malloc(GUI_TEXT_BUF_SZ);
		strncpy(b->params, params_attr, GUI_TEXT_BUF_SZ);
	}

	strncpy(b->hook, hook_attr, GUI_TEXT_BUF_SZ);
	return true;
}

void svg_btns(array * btns, ezxml_t container)
{
	for (ezxml_t node = ezxml_child(container, "btn"); node; node = node->next)
	{
		gui_btn * b = array_append_null(btns);
		gui_btn_init(b);
		if (!svg_btn(b, node))
		{
			log_write("invalid btn");
			gui_btn_destroy(b);
			array_remove(btns, array_size(btns) - 1);
		}
	}
}

b8 svg_text(gui_text * t, ezxml_t node)
{
	if (!svg_pos(node, &t->x, &t->y))
		return false;

	const char * color_attr = ezxml_attr(node, "fill");
	t->color = color_attr ? vlt_color_from_hex(color_attr) : g_nocolor;
	const char * opacity_attr = ezxml_attr(node, "opacity");
	if (opacity_attr)
		t->color.a = strtof(opacity_attr, NULL) * 255;

	const char * hook_attr = ezxml_attr(node, "hook");
	if (hook_attr)
	{
		t->type = GUI_TEXT_DYNAMIC;
		t->hook = malloc(GUI_TEXT_BUF_SZ);
		strncpy(t->hook, hook_attr, GUI_TEXT_BUF_SZ);

		const char * params_attr = ezxml_attr(node, "params");
		if (params_attr)
		{
			t->params = malloc(GUI_TEXT_BUF_SZ);
			strncpy(t->params, params_attr, GUI_TEXT_BUF_SZ);
		}
	}
	else
	{
		t->type = GUI_TEXT_STATIC;
		strncpy(t->txt, node->txt, GUI_TEXT_BUF_SZ);
	}

	const char * align_attr = ezxml_attr(node, "font-align");
	if (align_attr)
	{
		if (strcmp(align_attr, "right") == 0)
			t->align = FONT_ALIGN_RIGHT;
		else if (strcmp(align_attr, "center") == 0)
			t->align = FONT_ALIGN_CENTER;
	}

	const char * font_sz_attr = ezxml_attr(node, "font-size");
	if (font_sz_attr)
		t->sz = atoi(font_sz_attr);

	return    t->color.a != 0
	       && t->sz != 0
	       && (t->type == GUI_TEXT_STATIC ? strlen(t->txt) > 0 : strlen(t->hook) > 0);
}

void svg_texts(array * texts, ezxml_t container)
{
	for (ezxml_t node = ezxml_child(container, "text"); node; node = node->next)
	{
		gui_text * text = array_append_null(texts);
		gui_text_init(text);
		if (!svg_text(text, node))
		{
			log_write("invalid text");
			gui_text_destroy(text);
			array_remove(texts, array_size(texts) - 1);
		}
		else
			log_write("text [%u %u '%s']", text->x, text->y, text->type == GUI_TEXT_STATIC ? text->txt : text->hook);
	}
}

b8 svg_view_box(aabb * dims, ezxml_t node)
{
	const char * view_attr = ezxml_attr(node, "viewBox");
	if (!view_attr)
		return false;

	char * end;
	dims->top_left.x = strtof(view_attr, &end);
	view_attr = end;
	dims->top_left.y = strtof(view_attr, &end);
	view_attr = end;
	dims->bottom_right.x = strtof(view_attr, &end);
	view_attr = end;
	dims->bottom_right.y = strtof(view_attr, &end);
	view_attr = end;

	return true;
}


typedef struct vlt_svg
{
	aabb window;
	array_map symbols;
	array layers;
} vlt_svg;

vlt_svg * vlt_svg_create(const char * filename)
{
	vlt_svg * g = NULL;
	ezxml_t doc = ezxml_parse_file(filename);
	if (!doc)
	{
		log_write("failed to open svg '%s'", filename);
		goto err_file;
	}

	aabb window;
	if (!svg_view_box(&window, doc))
	{
		log_write("failed to find svg viewBox attr");
		goto out;
	}

	g = malloc(sizeof(vlt_svg));
	g->window = window;
	array_map_init(&g->symbols, sizeof(u32), sizeof(gui_symbol));
	array_init(&g->layers, sizeof(gui_layer));


	for (ezxml_t node = ezxml_child(doc, "symbol"); node; node = node->next)
	{
		log_write("symbol '%s'", ezxml_attr(node, "id"));
		const u32 id = vlt_hash(ezxml_attr(node, "id"));
		gui_symbol * symbol = array_map_insert_null(&g->symbols, &id);
		gui_symbol_init(symbol);
		svg_lines(&symbol->lines, node);
		svg_rects(&symbol->rects, node);
		svg_texts(&symbol->texts, node);
		svg_btns(&symbol->btns, node);

		aabb view = {0};
		if (svg_view_box(&view, node) && (view.top_left.x != 0 || view.top_left.y != 0))
		{
			log_write("view box offset: %d %d", view.top_left.x, view.top_left.y);

			for (u32 i = 0, end = array_size(&symbol->rects); i < end; ++i)
			{
				gui_rect * r = array_get(&symbol->rects, i);
				r->x -= view.top_left.x;
				r->y -= view.top_left.y;
			}
			for (u32 i = 0, end = array_size(&symbol->texts); i < end; ++i)
			{
				gui_text * t = array_get(&symbol->texts, i);
				t->x -= view.top_left.x;
				t->y -= view.top_left.y;
			}
			for (u32 i = 0, end = array_size(&symbol->btns); i < end; ++i)
			{
				gui_btn * b = array_get(&symbol->btns, i);
				b->rect.x -= view.top_left.x;
				b->rect.y -= view.top_left.y;
			}
		}
	}


	for (ezxml_t node = ezxml_child(doc, "g"); node; node = node->next)
	{
		log_write("layer '%s'", ezxml_attr(node, "id"));
		gui_layer * layer = array_append_null(&g->layers);
		gui_layer_init(layer);
		svg_lines(&layer->lines, node);
		svg_rects(&layer->rects, node);
		svg_texts(&layer->texts, node);
		svg_btns(&layer->btns, node);
		for (ezxml_t sym_node = ezxml_child(node, "use"); sym_node; sym_node = sym_node->next)
		{
			const char * ref = ezxml_attr(sym_node, "xlink:href");
			if (!ref)
			{
				log_write("invalid symbol ref");
				continue;
			}

			++ref;
			const u32 id = vlt_hash(ref);
			const gui_symbol * symbol = array_map_get(&g->symbols, &id);
			if (!symbol)
			{
				log_write("unknown symbol '%s'", ref);
				continue;
			}

			s32 x, y;
			if (!svg_pos(sym_node, &x, &y))
			{
				log_write("symbol ref '%s' has no position data", ref);
				continue;
			}

			gui_symbol_ref * sref = array_append_null(&layer->symbol_refs);
			gui_symbol_ref_init(sref);
			sref->symbol = symbol;
			sref->x = x;
			sref->y = y;

			const char * params_attr = ezxml_attr(sym_node, "params");
			if (params_attr)
			{
				sref->params = malloc(GUI_TEXT_BUF_SZ);
				strncpy(sref->params, params_attr, GUI_TEXT_BUF_SZ);
			}
		}
	}

out:
	ezxml_free(doc);
err_file:
	return g;
}

void vlt_svg_free(vlt_svg * g)
{
	array_destroy_each(&g->layers, (void(*)(void*))gui_layer_destroy);
	array_map_destroy_each(&g->symbols, NULL, (void(*)(void*))gui_symbol_destroy);
	free(g);
}


const aabb * vlt_svg_window(vlt_svg * s)
{
	return &s->window;
}

void text_get(gui_text * t, void * state, const char * addl_params, array_map * hooks)
{
	char params[GUI_TEXT_BUF_SZ] = "";
	if (addl_params)
		strncpy(params, addl_params, GUI_TEXT_BUF_SZ);
	if (t->params)
	{
		strncat(params, " ", GUI_TEXT_BUF_SZ - strlen(params) - 1);
		strncat(params, t->params, GUI_TEXT_BUF_SZ - strlen(params) - 1);
	}
	const u32 id = vlt_hash(t->hook);
	void ** fn = array_map_get(hooks, &id);
	if (fn)
		((void(*)(void*,const char*,char*))*fn)(state, params, t->txt);
	else
		strncpy(t->txt, t->hook, GUI_TEXT_BUF_SZ);
}

void btn_press(const gui_btn * b, void * state, const char * addl_params, array_map * hooks)
{
	char params[GUI_TEXT_BUF_SZ] = "";
	if (addl_params)
		strncpy(params, addl_params, GUI_TEXT_BUF_SZ);
	if (b->params)
	{
		strncat(params, b->params, GUI_TEXT_BUF_SZ - strlen(params) - 1);
		strncat(params, " ", GUI_TEXT_BUF_SZ - strlen(params) - 1);
	}
	const u32 id = vlt_hash(b->hook);
	void ** fn = array_map_get(hooks, &id);
	if (fn)
		((void(*)(void*,char*))*fn)(state, params);
}

void vlt_svg_render(vlt_gui * gui, vlt_svg * s, void * state,
                    array_map * text_hooks, array_map * btn_hooks)
{
	for (u32 layer_idx = 0, layer_end = array_size(&s->layers);
		 layer_idx < layer_end;
		 ++layer_idx)
	{
		const gui_layer * layer = array_get(&s->layers, layer_idx);
		for (u32 i = 0, end = array_size(&layer->rects); i < end; ++i)
		{
			const gui_rect * r = array_get(&layer->rects, i);
			vlt_gui_rect(gui, r->x, r->y, r->w, r->h, r->fill_color, r->line_color);
		}
		for (u32 i = 0, end = array_size(&layer->btns); i < end; ++i)
		{
			const gui_btn * b = array_get(&layer->btns, i);
			if(vlt_gui_btn(gui, b->rect.x, b->rect.y, b->rect.w, b->rect.h, b->rect.fill_color, b->rect.line_color))
				btn_press(b, state, NULL, btn_hooks);
		}
		for (u32 i = 0, end = array_size(&layer->texts); i < end; ++i)
		{
			gui_text * t = array_get(&layer->texts, i);
			if (t->type == GUI_TEXT_DYNAMIC)
				text_get(t, state, NULL, text_hooks);
			vlt_gui_txt(gui, t->x, t->y, t->sz, t->txt, t->color, t->align);
		}
		for (u32 i = 0, end = array_size(&layer->lines); i < end; ++i)
		{
			const gui_line * l = array_get(&layer->lines, i);
			vlt_gui_line(gui, l->x0, l->y0, l->x1, l->y1, 1, l->color);
		}
		for (u32 i = 0, end = array_size(&layer->symbol_refs); i < end; ++i)
		{
			const gui_symbol_ref * sref = array_get(&layer->symbol_refs, i);
			const gui_symbol * s = sref->symbol;
			for (u32 i = 0, end = array_size(&s->rects); i < end; ++i)
			{
				const gui_rect * r = array_get(&s->rects, i);
				vlt_gui_rect(gui, r->x + sref->x, r->y + sref->y, r->w, r->h, r->fill_color, r->line_color);
			}
			for (u32 i = 0, end = array_size(&s->btns); i < end; ++i)
			{
				const gui_btn * b = array_get(&s->btns, i);
				if(vlt_gui_btn(gui, b->rect.x + sref->x, b->rect.y + sref->y, b->rect.w, b->rect.h, b->rect.fill_color, b->rect.line_color))
					btn_press(b, state, sref->params, btn_hooks);
			}
			for (u32 i = 0, end = array_size(&s->texts); i < end; ++i)
			{
				gui_text * t = array_get(&s->texts, i);
				if (t->type == GUI_TEXT_DYNAMIC)
					text_get(t, state, sref->params, text_hooks);
				vlt_gui_txt(gui, t->x + sref->x, t->y + sref->y, t->sz, t->txt, t->color, t->align);
			}
			for (u32 i = 0, end = array_size(&s->lines); i < end; ++i)
			{
				const gui_line * l = array_get(&s->lines, i);
				vlt_gui_line(gui, sref->x + l->x0, sref->y + l->y0, sref->x + l->x1, sref->y + l->y1, 1, l->color);
			}
		}
	}
}

