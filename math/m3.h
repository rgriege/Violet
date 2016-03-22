#ifndef VIOLET_M3_H
#define VIOLET_M3_H

#include "violet/math/v2.h"
#include "violet/math/v3.h"

typedef r32 m3[9];

extern const m3 g_m3_identity;
extern const m3 g_m3_zero;

void m3_init_rot(m3 m, r32 radians);

void m3_mul_m(const m3 lhs, const m3 rhs, m3 res);
void m3_mul_v2(const m3 m, const v2 * v, v2 * res);
void m3_mul_v3(const m3 m, const v3 * v, v3 * res);
#define m3_mul(X, Y, Z) _Generic((Y), \
	m3:        m3_mul_m,              \
	const m3:  m3_mul_m,              \
	v2*:       m3_mul_v2,             \
	const v2*: m3_mul_v2,             \
	v3*:       m3_mul_v3,             \
	const v3*: m3_mul_v3              \
	)(X, Y, Z)

b8 m3_equal(const m3 lhs, const m3 rhs);
void m3_read(reader * r, m3 m);
void m3_write(writer * w, const m3 m);

#endif
