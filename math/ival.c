#include <math.h>

#include "violet/math/ival.h"


const ival g_0_to_1 = { 0, 1 };


void ival_slide(ival * i, s32 d)
{
	i->l += d;
	i->r += d;
}

s32 ival_length(const ival * i)
{
	return i->r - i->l;
}

b8 ival_contains_s(const ival * i, s32 x)
{
	return x >= i->l && x <= i->r;
}

b8 ival_contains_i(const ival * lhs, const ival * rhs)
{
	return ival_contains_r(lhs, rhs->l) && ival_contains_r(lhs, rhs->r);
}

b8 ival_overlaps(const ival * lhs, const ival * rhs)
{
	return ival_contains_r(lhs, rhs->l) || ival_contains_r(lhs, rhs->r);
}

s32 ival_overlap(const ival * lhs, const ival * rhs)
{
	s32 diff1 = lhs->r - rhs->l;
	s32 diff2 = rhs->r - lhs->l;
	return diff1 < 0 || diff2 < 0 ? 0 : min(min(diff1, diff2), min(ival_length(lhs), ival_length(rhs)));
}
