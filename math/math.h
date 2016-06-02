#ifndef VIOLET_MATH_H
#define VIOLET_MATH_H

#include "violet/core/types.h"

#define max(X, Y) (((X) < (Y)) ? (Y) : (X))
#define min(X, Y) (((X) > (Y)) ? (Y) : (X))

r32 clamp(r32 lo, r32 val, r32 hi);

u32 usub_capped(u32 lhs, u32 rhs);

r32 radians_to_degrees(r32 rad);
r32 degrees_to_radians(r32 deg);

#define PI 3.14159265359

#endif
