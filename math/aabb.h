#ifndef VIOLET_AABB_H
#define VIOLET_AABB_H

#include "violet/math/m3.h"
#include "violet/math/v2.h"

typedef struct aabb
{
	v2 top_left;
	v2 bottom_right;
} aabb;

void aabb_init_point(aabb * b, const v2 * p);
void aabb_from_line(aabb * b, const v2 * start, const v2 * end);
void aabb_from_center(aabb * b, const v2 * center, const v2 * half_dim);
void aabb_from_dims(aabb * b, r32 left, r32 top, r32 right, r32 bottom);

b8 aabb_empty(const aabb * b);

b8 aabb_contains_point(const aabb * b, const v2 * p);
b8 aabb_contains_aabb(const aabb * lhs, const aabb * rhs);

b8 aabb_overlaps(const aabb * lhs, const aabb * rhs);

void aabb_extend_point(aabb * b, const v2 * p);
void aabb_extend_aabb(aabb * b, const aabb * other);

void aabb_translate(aabb * b, const v2 * v);
void aabb_transform(aabb * b, const m3 mat);

void aabb_get_center(const aabb * b, v2 * center);
void aabb_get_half_dim(const aabb * b, v2 * half_dim);
void aabb_get_min(const aabb * b, v2 * min);
void aabb_get_max(const aabb * b, v2 * max);

#endif
