#include <assert.h>

#include "violet/math/line.h"

void POLY_(init)(array *p)
{
	array_init(p, sizeof(V2));
}

void POLY_(from_box)(array *p, const BOX2 *box)
{
	const V2 top_right = { box->bottom_right.x, box->top_left.y };
	const V2 bottom_left = { box->top_left.x, box->bottom_right.y };

	POLY_(init)(p);
	array_reserve(p, 4);
	array_append(p, &box->bottom_right);
	array_append(p, &top_right);
	array_append(p, &box->top_left);
	array_append(p, &bottom_left);
}

void POLY_(destroy)(array *p)
{
	array_destroy(p);
}

b8 POLY_(contains)(const array *poly, const V2 *point)
{
	BOX2 box;
	POLY_(bounding_box)(poly, &box);
	if (!BOX2_(contains_point)(&box, point))
		return false;

	V2 outside_point;
	V2_(sub)(&box.top_left, &box.bottom_right, &outside_point);
	V2_(add)(&outside_point, &box.top_left, &outside_point);

	u32 intersections = 0;
	SCALAR t, u;
	for (u32 i = 0, n = array_size(poly); i < n; ++i)
	{
		const V2 *a = array_get(poly, i);
		const V2 *b = array_get(poly, i < n - 1 ? i + 1 : 0);
		if (   MATH_(line_intersect_coords)(a, b, &outside_point, point, &t, &u)
		    && 0 < t && t <= 1 && 0 < u && u < 1)
			++intersections;
	}
	return intersections % 2 == 1;
}

void POLY_(bounding_box)(const array *p, BOX2 *box)
{
	const V2 center = POLY_(center)(p);
	BOX2_(from_center)(box, &center, &V2G_(zero));
	for (u32 i = 0, n = array_size(p); i < n; ++i)
		BOX2_(extend_point)(box, array_get(p, i));
}

void POLY_(translate)(array *p, const V2 *delta)
{
	for (u32 i = 0, n = array_size(p); i < n; ++i)
	{
		V2 *vertex = array_get(p, i);
		V2_(add)(vertex, delta, vertex);
	}
}

IVAL POLY_(project)(const array *p, const V2 *axis)
{
	V2 unit_axis = *axis;
	if (!V2_(is_unit)(&unit_axis))
		V2_(normalize)(axis, &unit_axis);

	const SCALAR v0_proj = V2_(dot)(array_get(p, 0), &unit_axis);
	IVAL projection = { .l = v0_proj, .r = v0_proj };

	for (u32 i = 1; i < p->size; ++i)
	{
		const V2 *vertex = array_get(p, i);
		const SCALAR dp = V2_(dot)(vertex, &unit_axis);
		if (dp < projection.l)
			projection.l = dp;
		else if (dp > projection.r)
			projection.r = dp;
	}
	return projection;
}

V2 POLY_(center)(const array *p)
{
	V2 center = { .x=0, .y=0 };
	const u32 n = array_size(p);
	for (u32 i = 0; i < n; ++i)
		V2_(add)(&center, array_get(p, i), &center);
	center.x /= n;
	center.y /= n;
	return center;
}

SCALAR POLY_(area)(const array *p)
{
	const V2 center = POLY_(center)(p);
	SCALAR area = 0;
	const u32 n = array_size(p);
	for (u32 i = 0; i < n; ++i)
	{
		const V2 *a = array_get(p, i);
		const V2 *b = array_get(p, i < n - 1 ? i + 1 : 0);
		V2 ab;
		V2_(sub)(b, a, &ab);
		V2 ac;
		V2_(sub)(&center, a, &ac);
		V2 height_axis;
		V2_(perp)(&ab, false, &height_axis);
		V2_(normalize)(&height_axis, &height_axis);
		V2 height;
		V2_(proj)(&ac, &height_axis, &height);
		area += 0.5 * V2_(mag)(&ab) * V2_(mag)(&height);
	}
	return area;
}

b8 POLY_(is_cc)(const array *p)
{
	assert(array_size(p) > 0);

	SCALAR sine_sum = 0;
	for (u32 i = 0, last = array_size(p) - 1; i <= last; ++i)
	{
		const V2 *a = array_get(p, i > 0 ? i - 1 : last);
		const V2 *b = array_get(p, i);
		const V2 *c = array_get(p, i < last ? i + 1 : 0);

		V2 ab, bc;
		V2_(sub)(b, a, &ab);
		V2_(sub)(c, b, &bc);

		sine_sum += V2_(cross)(&ab, &bc) / V2_(mag)(&ab) / V2_(mag)(&bc);
	}
	return sine_sum > 0;
}

b8 POLY_(segment_intersect)(const array *p, const V2 *v0, const V2 *v1)
{
	for (u32 i = 0, n = p->size; i < n; ++i)
	{
		const V2 *a = array_get(p, i);
		const V2 *b = array_get(p, (i+1)%n);
		V2 isec;
		if (MATH_(segment_intersect)(a, b, v0, v1, &isec))
			return true;
	}
	return false;
}

