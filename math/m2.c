#include <math.h>
#include <string.h>

const M2 M2G_(identity) = { 1, 0, 0, 1 };
const M2 M2G_(zero) = { 0, 0, 0, 0 };

void M2_(init_rot)(M2 m, SCALAR radians)
{
	m[0] = cos(radians);
	m[1] = -sin(radians);
	m[2] = sin(radians);
	m[3] = cos(radians);
}

b8 M2_(inverse)(const M2 m, M2 result)
{
	const SCALAR dt = M2_(determinant)(m);
	if (dt != 0)
	{
		const SCALAR a = m[0];
		result[0] = m[3];
		result[1] = -m[1];
		result[2] = -m[2];
		result[3] = a;
		const SCALAR s = 1/dt;
		M2_(scale)(result, s, result);
		return true;
	}
	return false;
}

SCALAR M2_(determinant)(const M2 m)
{
	return m[0] * m[3] - m[2] * m[1];
}

void M2_(scale)(const M2 m, SCALAR s, M2 result)
{
	result[0] = m[0] * s;
	result[1] = m[1] * s;
	result[2] = m[2] * s;
	result[3] = m[3] * s;
}

void M2_(mul_v2)(const M2 m, const V2 *v, V2 *result)
{
	result->x = v->x * m[0] + v->y * m[1];
	result->y = v->x * m[2] + v->y * m[3];
}

b8 M2_(equal)(const M2 lhs, const M2 rhs)
{
	return memcmp(lhs, rhs, sizeof(M2)) == 0;
}

