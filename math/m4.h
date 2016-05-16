#ifndef VIOLET_M4_H
#define VIOLET_M4_H

#include "violet/math/m3.h"
#include "violet/math/v3.h"

typedef struct m4
{
	_Alignas(16) r32 v[16];
} m4;

extern const m4 g_m4_identity;
extern const m4 g_m4_zero;

void m4_mul_m4(const m4 lhs, const m4 rhs, m4 res);
void m4_mul_v3(const m4 lhs, const v3 * rhs, v3 * res);

b8 m4_equal(const m4 lhs, const m4 rhs);

void m4_from_m3(m4 dst, const m3 src);
void m4_to_m3(const m4 src, m3 dst);

void m4_read(reader * r, m4 m);
void m4_write(writer * w, const m4 m);


typedef r32 unaligned_m4[16];

#endif
