#ifndef VIOLET_COMMON_MATH_H
#define VIOLET_COMMON_MATH_H

#include "violet/core/types.h"

#define max(X, Y) (((X) < (Y)) ? (Y) : (X))
#define min(X, Y) (((X) > (Y)) ? (Y) : (X))

u32 usub_capped(u32 lhs, u32 rhs);

#define PI 3.14159265359

#endif
