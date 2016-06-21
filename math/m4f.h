#ifndef VIOLET_M4F_H
#define VIOLET_M4F_H

#include "violet/math/m3.h"
#include "violet/math/v3.h"

typedef struct m4f
{
	_Alignas(16) r32 v[16];
} m4f;

extern const m4f g_m4f_identity;
extern const m4f g_m4f_zero;

void m4f_mul_m4(const m4f lhs, const m4f rhs, m4f res);
void m4f_mul_v3(const m4f lhs, const v3 *rhs, v3 *res);

b8 m4f_equal(const m4f lhs, const m4f rhs);

void m4f_from_m3(m4f dst, const m3 src);
void m4f_to_m3(const m4f src, m3 dst);

#endif
