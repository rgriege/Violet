#ifndef VIOLET_INTERVAL_H
#define VIOLET_INTERVAL_H

#include "violet/core/types.h"


typedef struct
{
	r32 l, r;
} interval;


extern const interval g_0_to_1;

void slide_interval(interval * i, r32 d);
r32 interval_length(interval * i);

b8 interval_contains_r(interval * i, r32 x);
b8 interval_contains_i(interval * lhs, interval * rhs);
#define interval_contains(X, Y) _Generic((Y), \
	r32: interval_contains_r,                 \
    interval*: interval_contains_i        \
    )(X, Y)

b8 interval_overlaps(interval * lhs, interval * rhs);
r32 interval_overlap(interval * lhs, interval * rhs);

#endif
