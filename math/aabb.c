#include "violet/math/aabb.h"
#include "violet/math/interval.h"
#include "violet/math/math.h"
#include "violet/math/poly.h"

void aabb_init_point(aabb * b, const v2 * p)
{
	b->top_left = *p;
	b->bottom_right = *p;
}

void aabb_from_line(aabb * box, const v2 * start, const v2 * end)
{
	r32 xmin, xmax, ymin, ymax;

	if (start->x < end->x)
	{
		xmin = start->x;
		xmax = end->x;
	}
	else
	{
		xmin = end->x;
		xmax = start->x;
	}

	if (start->y < end->y)
	{
		ymin = start->y;
		ymax = end->y;
	}
	else
	{
		ymin = end->y;
		ymax = start->y;
	}

	aabb_from_dims(box, xmin, ymax, xmax, ymin);
}

void aabb_from_center(aabb * box, const v2 * center, const v2 * half_dim)
{
	v2_sub(center, half_dim, &box->top_left);
	v2_add(center, half_dim, &box->bottom_right);
}

void aabb_from_dims(aabb * box, r32 left, r32 top, r32 right, r32 bottom)
{
	box->top_left.x = left;
	box->top_left.y = top;
	box->bottom_right.x = right;
	box->bottom_right.y = bottom;
}

b8 aabb_empty(const aabb * box)
{
	return v2_equal(&box->top_left, &box->bottom_right);
}

b8 aabb_contains_point(const aabb * box, const v2 * point)
{
	return point->x >= box->top_left.x
		&& point->x <= box->bottom_right.x
		&& point->y <= box->top_left.y
		&& point->y >= box->bottom_right.y;
}

b8 aabb_contains_aabb(const aabb * lhs, const aabb * rhs)
{
	interval lhs_x = { lhs->top_left.x, lhs->bottom_right.x };
	interval rhs_x = { rhs->top_left.x, rhs->bottom_right.x };
	interval lhs_y = { lhs->top_left.y, lhs->bottom_right.y };
	interval rhs_y = { rhs->top_left.y, rhs->bottom_right.y };
	return interval_contains_interval(&lhs_x, &rhs_x)
		&& interval_contains_interval(&lhs_y, &rhs_y);
}

b8 aabb_overlaps(const aabb * lhs, const aabb * rhs)
{
	interval lhs_x = { lhs->top_left.x, lhs->bottom_right.x };
	interval rhs_x = { rhs->top_left.x, rhs->bottom_right.x };
	interval lhs_y = { lhs->top_left.y, lhs->bottom_right.y };
	interval rhs_y = { rhs->top_left.y, rhs->bottom_right.y };
	return interval_overlaps(&lhs_x, &rhs_x) && interval_overlaps(&lhs_y, &rhs_y);
}

void aabb_extend_point(aabb * box, const v2 * point)
{
	aabb_from_dims(box,
		min(box->top_left.x, point->x),
		max(box->top_left.y, point->y),
		max(box->bottom_right.x, point->x),
		min(box->bottom_right.y, point->y));
}

void aabb_extend_aabb(aabb * lhs, const aabb * rhs)
{
	aabb_extend_point(lhs, &rhs->top_left);
	aabb_extend_point(lhs, &rhs->bottom_right);
}

void aabb_translate(aabb * box, const v2 * off)
{
	v2_add(&box->top_left, off, &box->top_left);
	v2_add(&box->bottom_right, off, &box->bottom_right);
}

void aabb_transform(aabb * box, const m3 mat)
{
	poly p;
	const v2 top_right = { box->bottom_right.x, box->top_left.y };
	const v2 bottom_left = { box->top_left.x, box->bottom_right.y };

	m3_mul_v2(mat, &box->top_left, array_append_null(&p.vertices));
	m3_mul_v2(mat, &top_right, array_append_null(&p.vertices));
	m3_mul_v2(mat, &box->bottom_right, array_append_null(&p.vertices));
	m3_mul_v2(mat, &bottom_left, array_append_null(&p.vertices));

	poly_bounding_box(&p, box);
}

void aabb_get_center(const aabb * box, v2 * center)
{
	center->x = (box->bottom_right.x - box->top_left.x) / 2.f;
	center->y = (box->top_left.y - box->bottom_right.y) / 2.f;
}

void aabb_get_half_dim(const aabb * box, v2 * half_dim)
{
	v2 center;
	aabb_get_center(box, &center);
	v2_sub(&center, &box->top_left, half_dim);
}

void aabb_get_min(const aabb * box, v2 * min)
{
	min->x = box->top_left.x;
	min->y = box->bottom_right.y;
}

void aabb_get_max(const aabb * box, v2 * max)
{
	max->x = box->bottom_right.x;
	max->y = box->top_left.y;
}

