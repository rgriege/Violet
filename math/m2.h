#ifndef VIOLET_M2_H
#define VIOLET_M2_H

#include "violet/math/v2.h"

typedef r32 m2[4];

extern const m2 g_m2_identity;
extern const m2 g_m2_zero;

void m2_init_rot(m2 m, r32 radians);

b8 m2_inverse(const m2 m, m2 result);
r32 m2_determinant(const m2 m);

void m2_scale(const m2 m, r32 s, m2 result);
void m2_mul_v2(const m2 m, const v2 * v, v2 * result);

b8 m2_equal(const m2 lhs, const m2 rhs);

#endif
