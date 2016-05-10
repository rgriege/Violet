#ifndef VIOLET_SVG_H
#define VIOLET_SVG_H

#include "violet/core/types.h"

typedef struct aabb aabb;
typedef struct array_map array_map;
typedef struct vlt_gui vlt_gui;


typedef struct vlt_svg vlt_svg;

vlt_svg * vlt_svg_create(const char * filename);
void vlt_svg_free(vlt_svg * g);

const aabb * vlt_svg_window(vlt_svg * s);
void vlt_svg_render(vlt_gui * gui, vlt_svg * s, void * state,
                    array_map * text_hooks, array_map * btn_hooks);

#endif

