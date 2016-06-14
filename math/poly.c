#include <assert.h>

#include "violet/math/aabb.h"
#include "violet/math/interval.h"
#include "violet/math/line.h"
#include "violet/math/m2.h"
#include "violet/math/poly.h"
#include "violet/math/v2f.h"

void poly_init(poly * p)
{
	array_init(&p->vertices, sizeof(v2f));
}

void poly_from_box(poly * p, const aabb * box)
{
	const v2f top_right = { box->bottom_right.x, box->top_left.y };
	const v2f bottom_left = { box->top_left.x, box->bottom_right.y };

	poly_init(p);
	array_reserve(&p->vertices, 4);
	array_append(&p->vertices, &box->bottom_right);
	array_append(&p->vertices, &top_right);
	array_append(&p->vertices, &box->top_left);
	array_append(&p->vertices, &bottom_left);
}

void poly_destroy(poly * p)
{
	array_destroy(&p->vertices);
}

b8 poly_contains(const poly * poly, const v2f * point)
{
	aabb box;
	poly_bounding_box(poly, &box);
	if (!aabb_contains_point(&box, point))
		return false;

	v2f outside_point;
	v2f_sub(&box.top_left, &box.bottom_right, &outside_point);
	v2f_add(&outside_point, &box.top_left, &outside_point);

	u32 intersections = 0;
	r32 t, u;
	for (u32 i = 0, n = array_size(&poly->vertices); i < n; ++i)
	{
		const v2f * a = array_get(&poly->vertices, i);
		const v2f * b = array_get(&poly->vertices, i < n - 1 ? i + 1 : 0);
		if (   line_intersect_coords(a, b, &outside_point, point, &t, &u)
		    && 0 < t && t <= 1 && 0 < u && u < 1)
			++intersections;
	}
	return intersections % 2 == 1;
}

void poly_bounding_box(const poly * p, aabb * box)
{
	const v2f center = poly_center(p);
	aabb_from_center(box, &center, &g_v2f_zero);
	for (u32 i = 0, n = array_size(&p->vertices); i < n; ++i)
		aabb_extend_point(box, array_get(&p->vertices, i));
}

void poly_translate(poly * p, const v2f * delta)
{
	for (u32 i = 0, n = array_size(&p->vertices); i < n; ++i)
	{
		v2f * vertex = array_get(&p->vertices, i);
		v2f_add(vertex, delta, vertex);
	}
}

interval poly_project(const poly * p, const v2f * axis)
{
	v2f unit_axis = *axis;
	if (!v2f_is_unit(&unit_axis))
		v2f_normalize(axis, &unit_axis);
	interval projection = { .l = 0,.r = 0 };
	for (u32 i = 0; i < p->vertices.size; ++i)
	{
		const v2f * vertex = array_get(&p->vertices, i);
		const r32 dp = v2f_dot(vertex, &unit_axis);
		if (dp < projection.l)
			projection.l = dp;
		else if (dp > projection.r)
			projection.r = dp;
	}
	return projection;
}

v2f poly_center(const poly * p)
{
	v2f center = { .x=0, .y=0 };
	u32 n = array_size(&p->vertices);
	for (u32 i = 0; i < n; ++i)
		v2f_add(&center, array_get(&p->vertices, i), &center);
	const r32 scale = 1.f / n;
	v2f_scale(&center, scale, scale, &center);
	return center;
}

r32 poly_area(const poly * p)
{
	const v2f center = poly_center(p);
	r32 area = 0.f;
	u32 n = array_size(&p->vertices);
	for (u32 i = 0; i < n; ++i)
	{
		const v2f * a = array_get(&p->vertices, i);
		const v2f * b = array_get(&p->vertices, i < n - 1 ? i + 1 : 0);
		v2f ab;
		v2f_sub(b, a, &ab);
		v2f ac;
		v2f_sub(&center, a, &ac);
		v2f height_axis;
		v2f_perp(&ab, false, &height_axis);
		v2f_normalize(&height_axis, &height_axis);
		v2f height;
		v2f_proj(&ac, &height_axis, &height);
		area += 0.5f * v2f_mag(&ab) * v2f_mag(&height);
	}
	return area;
}

b8 poly_is_cc(const poly * p)
{
	assert(array_size(&p->vertices) > 0);

	r32 sine_sum = 0;
	for (u32 i = 0, last = array_size(&p->vertices) - 1; i <= last; ++i)
	{
		const v2f * a = array_get(&p->vertices, i > 0 ? i - 1 : last);
		const v2f * b = array_get(&p->vertices, i);
		const v2f * c = array_get(&p->vertices, i < last ? i + 1 : 0);

		v2f ab, bc;
		v2f_sub(b, a, &ab);
		v2f_sub(c, b, &bc);

		sine_sum += v2f_cross(&ab, &bc) / v2f_mag(&ab) / v2f_mag(&bc);
	}
	return sine_sum > 0.f;
}

