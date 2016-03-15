#include <math.h>

#include "violet/math/v2.h"
/*#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"*/

const v2 g_v2_x_axis = { 1, 0 };
const v2 g_v2_y_axis = { 0, 1 };
const v2 g_v2_zero = { 0, 0 };

r32 v2_mag(const v2 * v)
{
	sqrt(v2_mag_squared(v));
}

r32 v2_mag_squared(const v2 * v)
{
	return v->x * v->x + v->y * v->y;
}

void v2_normalize(const v2 * v, v2 * result)
{
	const r32 mag = v2_mag(v);
	v2_scale(v, mag, mag, result);
}

b8 v2_is_unit(const v2 * v)
{
	return v2_mag_squared(v) == 1;
}

void v2_scale(const v2 * v, r32 sx, r32 sy, v2 * result)
{
	result->x = v->x * sx;
	result->y = v->y * sy;
}

void v2_add(const v2 * lhs, const v2 * rhs, v2 * result)
{
	result->x = lhs->x + rhs->x;
	result->y = lhs->y + rhs->y;
}

void v2_sub(const v2 * lhs, const v2 * rhs, v2 * result)
{
	result->x = lhs->x - rhs->x;
	result->y = lhs->y - rhs->y;
}

void v2_rot(const v2 * v, r32 radians, v2 * result)
{
	r32 x = v->x;
	const r32 c = cos(radians);
	const r32 s = sin(radians);
	result->x = x * c - v->y * s;
	result->y = x * s + v->y * c;
}

r32 v2_dot(const v2 * lhs, const v2 * rhs)
{
	return lhs->x * rhs->x + lhs->y * rhs->y;
}

r32 v2_cross(const v2 * lhs, const v2 * rhs)
{
	return lhs->x * rhs->y - lhs->y * rhs->x;
}

void v2_proj(const v2 * v, const v2 * axis, v2 * result)
{
	v2 dir;
	v2_normalize(axis, &dir);
	r32 scale = v2_dot(v, axis);
	v2_scale(&dir, scale, scale, result);
}

void v2_perp(const v2 * v, b8 left, v2 * result)
{
	if (left)
	{
		result->x = v->y;
		result->y = -v->x;
	}
	else
	{
		result->x = -v->y;
		result->y = v->x;
	}
}

void v2_inverse(const v2 * v, v2 * result)
{
	result->x = -v->x;
	result->y = -v->y;
}

b8 v2_is_zero(const v2 * v)
{
	return v->x == 0 && v->y == 0;
}

b8 v2_equal(const v2 * lhs, const v2 * rhs)
{
	return lhs->x == rhs->x && lhs->y == rhs->y;
}

b8 v2_share_quadrant(const v2 * lhs, const v2 * rhs)
{
	return v2_dot(lhs, rhs) >= 0;
}

void v2_read(reader * r, v2 * v)
{
	/*auto segment = deserializer.enter_segment("vec");
	vec.x = segment->get_r32("x");
	vec.y = segment->get_r32("y");
	return deserializer;*/
}

void v2_write(writer * w, const v2 * v)
{
	/*auto segment = serializer.create_segment("vec");
	segment->write_r32("x", vec.x);
	segment->write_r32("y", vec.y);
	return serializer;*/
}
