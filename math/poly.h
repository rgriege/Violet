#ifndef VIOLET_POLYGON_H
#define VIOLET_POLYGON_H

#include "violet/math/interval.h"
#include "violet/math/v2.h"
#include "violet/structures/array.h"

typedef struct aabb aabb;


typedef struct poly
{
	array vertices;
} poly;


void poly_init(poly * p);
void poly_from_box(poly * p, const aabb * box);
void poly_destroy(poly * p);

b8 poly_contains(const poly * p, const v2 * point);
void poly_bounding_box(const poly * p, aabb * box);

void poly_translate(poly * p, const v2 * delta);

interval poly_project(const poly * p, const v2 * axis);
v2 poly_center(const poly * p);
r32 poly_area(const poly * p);

#endif
