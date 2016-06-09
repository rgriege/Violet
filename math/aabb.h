#ifndef VIOLET_AABB_H
#define VIOLET_AABB_H

#include "violet/math/m3.h"
#include "violet/math/v2f.h"

typedef struct aabb
{
	v2f top_left;
	v2f bottom_right;
} aabb;

void aabb_init_point(aabb * b, const v2f * p);
void aabb_from_line(aabb * b, const v2f * start, const v2f * end);
void aabb_from_center(aabb * b, const v2f * center, const v2f * half_dim);
void aabb_from_dims(aabb * b, r32 left, r32 top, r32 right, r32 bottom);

b8 aabb_empty(const aabb * b);

b8 aabb_contains_point(const aabb * b, const v2f * p);
b8 aabb_contains_aabb(const aabb * lhs, const aabb * rhs);

b8 aabb_overlaps(const aabb * lhs, const aabb * rhs);

void aabb_extend_point(aabb * b, const v2f * p);
void aabb_extend_aabb(aabb * b, const aabb * other);

void aabb_translate(aabb * b, const v2f * v);
void aabb_transform(aabb * b, const m3 mat);

void aabb_get_center(const aabb * b, v2f * center);
void aabb_get_half_dim(const aabb * b, v2f * half_dim);
void aabb_get_min(const aabb * b, v2f * min);
void aabb_get_max(const aabb * b, v2f * max);

#endif
