#include <math.h>

#include "violet/math/v3.h"

const v3 g_v3_x_Axis = { 1, 0, 0 };
const v3 g_v3_y_Axis = { 0, 1, 0 };
const v3 g_v3_z_Axis = { 0, 0, 1 };
const v3 g_v3_zero = { 0, 0, 0 };

r32 v3_mag(const v3 * v)
{
	return sqrt(v3_mag_squared(v));
}

r32 v3_mag_squared(const v3 * v)
{
	return v->x * v->x + v->y * v->y + v->z * v->z;
}

void v3_normalize(const v3 * v, v3 * res)
{
	r32 scale = 1.f / v3_mag_squared(v);
	v3_scale(v, scale, scale, scale, res);
}

b8 v3_is_unit(const v3 * v)
{
	return v3_mag_squared(v) == 1;
}

void v3_scale(const v3 * v, const r32 sx, const r32 sy, const r32 sz, v3 * res)
{
	res->x = v->x * sx;
	res->y = v->y * sy;
	res->z = v->z * sz;
}

void v3_add(const v3 * lhs, const v3 * rhs, v3 * res)
{
	res->x = lhs->x + rhs->x;
	res->y = lhs->y + rhs->y;
	res->z = lhs->z + rhs->z;
}

void v3_sub(const v3 * lhs, const v3 * rhs, v3 * res)
{
	res->x = lhs->x - rhs->x;
	res->y = lhs->y - rhs->y;
	res->z = lhs->z - rhs->z;
}

r32 v3_dot(const v3 * lhs, const v3 * rhs)
{
	return lhs->x * rhs->x + lhs->y * rhs->y + lhs->z * rhs->z;
}

void v3_cross(const v3 * lhs, const v3 * rhs, v3 * res)
{
	res->x = lhs->y * rhs->z - lhs->z * rhs->y;
	res->y = lhs->z * rhs->x - lhs->x * rhs->z;
	res->z = lhs->x * rhs->y - lhs->y * rhs->x;
}

void v3_proj(const v3 * v, const v3 * axis, v3 * res)
{
	v3 unit_axis;
	v3_normalize(axis, &unit_axis);
	r32 scale = v3_dot(v, axis);
	v3_scale(&unit_axis, scale, scale, scale, res);
}

void v3_inverse(const v3 * v, v3 * res)
{
	res->x = -v->x;
	res->y = -v->y;
	res->z = -v->z;
}

b8 v3_is_zero(const v3 * v)
{
	return v3_equal(v, &g_v3_zero);
}

b8 v3_equal(const v3 * lhs, const v3 * rhs)
{
	return lhs->x == rhs->x && lhs->y == rhs->y && lhs->z == rhs->z;
}

/*Serializer * vlt_operator<<(Serializer * serializer, const v3 * vec)
{
	auto segment = serializer.create_segment("vec");
	segment->write_r32("x", vec.x);
	segment->write_r32("y", vec.y);
	segment->write_r32("z", vec.z);
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer * vlt_operator>>(Deserializer * deserializer, v3 * vec)
{
	auto segment = deserializer.enter_segment("vec");
	vec.x = segment->get_r32("x");
	vec.y = segment->get_r32("y");
	vec.z = segment->get_r32("z");
	return deserializer;
}*/

