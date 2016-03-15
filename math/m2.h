#ifndef VIOLET_M2_H
#define VIOLET_M2_H

#include "violet/math/v2.h"

/*typedef struct
{
	r32 cols[3];
} _m2_row;*/

typedef r32 m2[4];

/*struct m2
{
	union
	{
		_m2_row rows[3];

		struct
		{
			r32 a, b,
				c, d;
		};
	};
};*/

extern const m2 g_m2_identity;
extern const m2 g_m2_zero;

void m2_init_rot(m2 m, r32 radians);

b8 m2_inverse(const m2 m, m2 result);
r32 m2_determinant(const m2 m);

void m2_mul_s(const m2 m, r32 s, m2 result);
void m2_mul_v(const m2 m, const v2 * v, v2 * result);
#define m2_mul(X, Y, R) _Generic((Y) \
    r32: m2_mul_s,                   \
	v2*: m2_mul_v                    \
	)(X, Y, R)

b8 m2_equal(const m2 lhs, const m2 rhs);

#endif

