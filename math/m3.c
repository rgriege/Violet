#include <math.h>

const M3 M3G_(identity) = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

const M3 M3G_(zero) = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
};

void M3_(init_rot)(M3 m, r32 radians)
{
	m[0] = cos(radians);
	m[1] = -sin(radians);
	m[2] = 0;
	m[3] = sin(radians);
	m[4] = m[0];
	m[5] = 0;
	m[6] = 0;
	m[7] = 0;
	m[8] = 1;
}

void M3_(mul_m3)(const M3 lhs, const M3 rhs, M3 res)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int h = 0; h < 3; h++)
				res[3*i+j] += lhs[3*i+h] * rhs[3*h+j];
}

void M3_(mul_v2)(const M3 m, const V2 *v, V2 *res)
{
	res->x = m[0] * v->x + m[1] * v->y + m[2];
	res->y = m[3] * v->x + m[4] * v->y + m[5];
}

void M3_(mul_v3)(const M3 m, const V3 *v, V3 *res)
{
	res->x = m[0] * v->x + m[1] * v->y + m[2] * v->z;
	res->y = m[3] * v->x + m[4] * v->y + m[5] * v->z;
	res->z = m[6] * v->x + m[7] * v->y + m[8] * v->z;
}

b8 M3_(equal)(const M3 lhs, const M3 rhs)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (lhs[3*i+j] != rhs[3*i+j])
				return false;
	return true;
}

