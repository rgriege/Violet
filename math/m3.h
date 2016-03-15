#ifndef VIOLET_M3_H
#define VIOLET_M3_H

#include "violet/math/v2.h"

typedef struct
{
	r32 cols[3];
} _m3_row;

typedef struct
{
	union
	{
		_m3_row rows[3];

		struct
		{
			r32 a, b, c,
				d, e, f,
				g, h, i;
		};
	};
} m3;

extern const g_m3_identity;
static const g_m3_zero;

m3 m3_create_rot(r32 radians);

m3 m3_mul_m(const m3 * lhs, const m3 * rhs);
v2 m3_mul_v(const m3 * m, const v2 * v);
#define m3_mul(X, Y) _Generic(Y) \
	m3*: m3_mul_m,               \
	v2*: m3_mul_v                \
	)(X, Y)

b8 m3_equal(const m3 * lhs, const m3 * rhs);
void m3_read(reader * r, m3 * m);
void m3_write(writer * w, const m3 * m);

#endif
