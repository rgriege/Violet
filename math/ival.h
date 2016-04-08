#ifndef VIOLET_IVAL_H
#define VIOLET_IVAL_H

#include "violet/core/types.h"


typedef struct ival
{
	s32 l, r;
} ival;


void ival_slide(ival * i, s32 d);
s32 ival_length(const ival * i);

b8 ival_contains_s(const ival * i, s32 x);
b8 ival_contains_i(const ival * lhs, const ival * rhs);
#define ival_contains(X, Y) _Generic((Y), \
	s32:             ival_contains_s,     \
    ival*:           ival_contains_i,     \
    const ival*:     ival_contains_i      \
    )(X, Y)

b8 ival_overlaps(const ival * lhs, const ival * rhs);
s32 ival_overlap(const ival * lhs, const ival * rhs);

#endif
