#ifndef VIOLET_INTERVAL_H
#define VIOLET_INTERVAL_H

#include "violet/core/types.h"


typedef struct interval
{
	r32 l, r;
} interval;


extern const interval g_0_to_1;

void slide_interval(interval * i, r32 d);
r32 interval_length(const interval * i);

b8 interval_contains_r(const interval * i, r32 x);
b8 interval_contains_i(const interval * lhs, const interval * rhs);
#define interval_contains(X, Y) _Generic((Y), \
	r32:             interval_contains_r,     \
    interval*:       interval_contains_i,     \
    const interval*: interval_contains_i      \
    )(X, Y)

b8 interval_overlaps(const interval * lhs, const interval * rhs);
r32 interval_overlap(const interval * lhs, const interval * rhs);

#endif
