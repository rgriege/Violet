#include <math.h>

#include "violet/math/m3.h"

const m3 g_m3_identity = {
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
};

const m3 g_m3_zero = {
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f
};

void m3_init_rot(m3 m, r32 radians)
{
	m[0] = cos(radians);
	m[1] = -sin(radians);
	m[2] = 0;
	m[3] = sin(radians);
	m[4] = cos(radians);
	m[5] = 0;
	m[6] = 0;
	m[7] = 0;
	m[8] = 0;
}

void m3_mul_m3(const m3 lhs, const m3 rhs, m3 res)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int h = 0; h < 3; h++)
				res[3*i+j] += lhs[3*i+h] * rhs[3*h+j];
}

void m3_mul_v2f(const m3 m, const v2f * v, v2f * res)
{
	res->x = m[0] * v->x + m[1] * v->y + m[2];
	res->y = m[3] * v->x + m[4] * v->y + m[5];
}

void m3_mul_v3(const m3 m, const v3 * v, v3 * res)
{
	res->x = m[0] * v->x + m[1] * v->y + m[2] * v->z;
	res->y = m[3] * v->x + m[4] * v->y + m[5] * v->z;
	res->z = m[6] * v->x + m[7] * v->y + m[8] * v->z;
}

b8 m3_equal(const m3 lhs, const m3 rhs)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (lhs[3*i+j] != rhs[3*i+j])
				return false;
	return true;
}

/*Serializer * vlt_operator<<(Serializer * serializer, const m3 * mat)
{
	auto segment = serializer.create_segment("mat");
	segment->write_r32("a", mat[0][0]);
	segment->write_r32("b", mat[0][1]);
	segment->write_r32("c", mat[0][2]);
	segment->write_r32("d", mat[1][0]);
	segment->write_r32("e", mat[1][1]);
	segment->write_r32("f", mat[1][2]);
	segment->write_r32("g", mat[2][0]);
	segment->write_r32("h", mat[2][1]);
	segment->write_r32("i", mat[2][2]);
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer * vlt_operator>>(Deserializer * deserializer, m3 * mat)
{
	auto segment = deserializer.enter_segment("mat");
	mat[0][0] = segment->get_r32("a");
	mat[0][1] = segment->get_r32("b");
	mat[0][2] = segment->get_r32("c");
	mat[1][0] = segment->get_r32("d");
	mat[1][1] = segment->get_r32("e");
	mat[1][2] = segment->get_r32("f");
	mat[2][0] = segment->get_r32("g");
	mat[2][1] = segment->get_r32("h");
	mat[2][2] = segment->get_r32("i");
	return deserializer;
}*/

