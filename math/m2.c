#include <math.h>
#include <string.h>

#include "violet/math/m2.h"

const m2 g_m2_identity = { 1, 0, 0, 1 };
const m2 g_m2_zero = { 0, 0, 0, 0 };

void m2_init_rot(m2 m, const r32 radians)
{
	m[0] = cos(radians);
	m[1] = -sin(radians);
	m[2] = sin(radians);
	m[3] = cos(radians);
}

b8 m2_inverse(const m2 m, m2 result)
{
	const r32 dt = m2_determinant(m);
	if (dt != 0)
	{
		const r32 a = m[0];
		result[0] = m[3];
		result[1] = -m[1];
		result[2] = -m[2];
		result[3] = a;
		const r32 s = 1/dt;
		m2_mul_s(result, s, result);
		return true;
	}
	return false;
}

r32 m2_determinant(const m2 m)
{
	return m[0] * m[3] - m[2] * m[1];
}

void m2_mul_s(const m2 m, r32 s, m2 result)
{
	result[0] = m[0] * s;
	result[1] = m[1] * s;
	result[2] = m[2] * s;
	result[3] = m[3] * s;
}

void m2_mul_v(const m2 m, const v2 * v, v2 * result)
{
	result->x = v->x * m[0] + v->y * m[1];
	result->y = v->x * m[2] + v->y * m[3];
}

b8 m2_equal(const m2 lhs, const m2 rhs)
{
	return memcmp(lhs, rhs, sizeof(m2));
}

