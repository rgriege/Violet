#ifndef VIOLET_M3_H
#define VIOLET_M3_H

#include "violet/math/v2f.h"
#include "violet/math/v3.h"

typedef r32 m3[9];

extern const m3 g_m3_identity;
extern const m3 g_m3_zero;

void m3_init_rot(m3 m, r32 radians);

void m3_mul_m3(const m3 lhs, const m3 rhs, m3 res);
void m3_mul_v2f(const m3 m, const v2f * v, v2f * res);
void m3_mul_v3(const m3 m, const v3 * v, v3 * res);

b8 m3_equal(const m3 lhs, const m3 rhs);
void m3_read(reader * r, m3 m);
void m3_write(writer * w, const m3 m);

#endif
