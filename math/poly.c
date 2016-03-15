#include "violet/math/interval.h"
#include "violet/math/m2.h"
#include "violet/math/poly.h"
#include "violet/serialization/serialization_utilities.h"

void poly_from_box(poly * p, aabb * box)
{
	const v2 top_right = { box->bottom_right.x, box->top_left.y };
	const v2 bottom_left = { box->top_left.x, box->bottom_right.y };

	array_reserve(&p->vertices, 4);
	array_append(&p->vertices, &box->bottom_right);
	array_append(&p->vertices, &top_right);
	array_append(&p->vertices, &box->top_left);
	array_append(&p->vertices, &bottom_left);
}

static b8 _poly_side_barycentric_contains(const poly * p, const v2 * point, u32 start_idx, u32 end_idx)
{
	const v2 * p1 = array_get(&p->vertices, start_idx);
	const v2 * p2 = array_get(&p->vertices, end_idx);
	const m2 mat = { p1->x, p2->x, p1->y, p2->y };
	m2_inverse(mat, mat);
	v2 barycentric_coords;
	m2_mul(mat, point, &barycentric_coords);
	return    interval_contains(&g_0_to_1, barycentric_coords.x)
	       && interval_contains(&g_0_to_1, barycentric_coords.y)
		   && barycentric_coords.x + barycentric_coords.y <= 1;

}

b8 poly_contains(const poly * p, const v2 * point) const
{
	u32 const n = array_size(&p->vertices);
	b8 inside = _poly_side_barycentric_contains(p, point, n - 1, 0);

	u32 i = 1;
	while (!inside && i < n)
	{
		inside = _poly_side_barycentric_contains(p, point, i, i - 1);
		++i;
	}

	return inside;
}

aabb poly_bounding_box(const poly * p)
{
	aabb result = { poly_center(p), g_v2_zero };
	for (u32 i = 0, n = array_size(&p->vertices); i < n; ++i)
		aabb_extend(&result, (v2*) array_get(&p->vertices, i));
	return result;
}

void poly_translate(const poly * p, const v2 * delta)
{
	for (u32 i = 0, n = array_size(&p->vertices); i < n; ++i)
	{
		v2 * vertex = array_get(&p->vertices, i);
		v2_add(vertex, delta, vertex);
	}
}

interval poly_project(const poly * p, const v2 * axis)
{
	const v2 & unitAxis = axis.is_unit() ? axis : axis.get_unit();
	Interval projection;
	for (const auto & vertex : vertices)
	{
		const r32 dp = vertex.dot(unitAxis);
		if (dp < projection.left)
			projection.left = dp;
		else if (dp > projection.right)
			projection.right = dp;
	}
	return projection;
}

v2 poly_center(const poly * p)
{
	v2 center;
	u32 n = array_size(&p->vertices);
	for (u32 i = 0; i < n; ++i)
		v2_add(&center, array_get(&p->vertices, i), &center);
	v2_mul(&center, 1.f / n);
	return center;
}

/*Serializer & vlt_operator<<(Serializer & serializer, const poly & poly)
{
	auto segment = serializer.create_segment(ms_segmentLabel);
	serialize_vector(*segment, poly.vertices);
	return serializer;
}

Deserializer & vlt_operator>>(Deserializer & deserializer, poly & poly)
{
	auto segment = deserializer.enter_segment(ms_segmentLabel);
	poly.vertices = deserialize_vector<v2>(*segment);
	return deserializer;
}*/

