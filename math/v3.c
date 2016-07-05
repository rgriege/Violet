#include <math.h>

const V3 V3G_(x_axis) = { 1, 0, 0 };
const V3 V3G_(y_axis) = { 0, 1, 0 };
const V3 V3G_(z_axis) = { 0, 0, 1 };
const V3 V3G_(zero) = { 0, 0, 0 };

SCALAR V3_(mag)(const V3 *v)
{
	return sqrt(V3_(mag_sq)(v));
}

SCALAR V3_(mag_sq)(const V3 *v)
{
	return v->x * v->x + v->y * v->y + v->z * v->z;
}

void V3_(normalize)(const V3 *v, V3 *res)
{
	SCALAR scale = 1.f / V3_(mag_sq)(v);
	V3_(scale)(v, scale, scale, scale, res);
}

b8 V3_(is_unit)(const V3 *v)
{
	return V3_(mag_sq)(v) == 1;
}

void V3_(scale)(const V3 *v, SCALAR sx, SCALAR sy, SCALAR sz, V3 *res)
{
	res->x = v->x * sx;
	res->y = v->y * sy;
	res->z = v->z * sz;
}

void V3_(add)(const V3 *lhs, const V3 *rhs, V3 *res)
{
	res->x = lhs->x + rhs->x;
	res->y = lhs->y + rhs->y;
	res->z = lhs->z + rhs->z;
}

void V3_(sub)(const V3 *lhs, const V3 *rhs, V3 *res)
{
	res->x = lhs->x - rhs->x;
	res->y = lhs->y - rhs->y;
	res->z = lhs->z - rhs->z;
}

SCALAR V3_(dot)(const V3 *lhs, const V3 *rhs)
{
	return lhs->x * rhs->x + lhs->y * rhs->y + lhs->z * rhs->z;
}

void V3_(cross)(const V3 *lhs, const V3 *rhs, V3 *res)
{
	res->x = lhs->y * rhs->z - lhs->z * rhs->y;
	res->y = lhs->z * rhs->x - lhs->x * rhs->z;
	res->z = lhs->x * rhs->y - lhs->y * rhs->x;
}

void V3_(proj)(const V3 *v, const V3 *axis, V3 *res)
{
	V3 unit_axis;
	V3_(normalize)(axis, &unit_axis);
	SCALAR scale = V3_(dot)(v, axis);
	V3_(scale)(&unit_axis, scale, scale, scale, res);
}

void V3_(inverse)(const V3 *v, V3 *res)
{
	res->x = -v->x;
	res->y = -v->y;
	res->z = -v->z;
}

b8 V3_(is_zero)(const V3 *v)
{
	return V3_(equal)(v, &V3G_(zero));
}

b8 V3_(equal)(const V3 *lhs, const V3 *rhs)
{
	return lhs->x == rhs->x && lhs->y == rhs->y && lhs->z == rhs->z;
}

