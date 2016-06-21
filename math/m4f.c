#include <assert.h>
#include <string.h>
#include <x86intrin.h>

#include "violet/math/m4f.h"

const m4f g_m4f_identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const m4f g_m4f_zero = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

void m4f_mul_m4(const m4f lhs, const m4f rhs, m4f res)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 4; ++k)
		{
			r32 r = lhs.v[4*i+k];
			__m128 r_vec = _mm_set_ps(r, r, r, r);

			__m128 rhs_vec = _mm_load_ps(&rhs.v[4*k]);
			__m128 res_vec = _mm_load_ps(&res.v[4*i]);
			__m128 mul = _mm_mul_ps(r_vec, rhs_vec);
			__m128 add = _mm_add_ps(res_vec, mul);
			_mm_store_ps(&res.v[4*i], add);
		}
	}
}

void m4f_mul_v3(const m4f lhs, const v3 *rhs, v3 *res)
{
	res->x = lhs.v[0] * rhs->x + lhs.v[1] * rhs->y + lhs.v[2] * rhs->z + lhs.v[3];
	res->y = lhs.v[4] * rhs->x + lhs.v[5] * rhs->y + lhs.v[6] * rhs->z + lhs.v[7];
	res->z = lhs.v[8] * rhs->x + lhs.v[9] * rhs->y + lhs.v[10] * rhs->z + lhs.v[11];
}

b8 m4f_equal(const m4f lhs, const m4f rhs)
{
	return memcmp(lhs.v, rhs.v, 16 * sizeof(r32)) == 0;
}

void m4f_from_m3(m4f dst, const m3 src)
{
	dst.v[0] = src[0];
	dst.v[1] = src[1];
	dst.v[2] = 0;
	dst.v[3] = src[2];
	dst.v[4] = src[3];
	dst.v[5] = src[4];
	dst.v[6] = 0;
	dst.v[7] = src[5];
	dst.v[8] = 0;
	dst.v[9] = 0;
	dst.v[10] = 1;
	dst.v[11] = 0;
	dst.v[12] = src[6];
	dst.v[13] = src[7];
	dst.v[14] = 0;
	dst.v[15] = src[8];
}

void m4f_to_m3(const m4f src, m3 dst)
{
	memcpy(dst, src.v, 3 * sizeof(r32));
	memcpy(&dst[3], &src.v[4], 3 * sizeof(r32));
	memcpy(&dst[6], &src.v[8], 3 * sizeof(r32));
}

