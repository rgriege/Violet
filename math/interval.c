#include <math.h>

#include "violet/math/interval.h"


const interval g_0_to_1 = { 0, 1 };


void slide_interval(interval * i, r32 d)
{
	i->l += d;
	i->r += d;
}

r32 interval_length(interval * i)
{
	return i->r - i->l;
}

b8 interval_contains_r(interval * i, r32 x)
{
	return x >= i->l && x <= i->r;
}

b8 interval_contains_i(interval * lhs, interval * rhs)
{
	return interval_contains_r(lhs, rhs->l) && interval_contains_r(lhs, rhs->r);
}

b8 interval_overlaps(interval * lhs, interval * rhs)
{
	return interval_contains_r(lhs, rhs->l) || interval_contains_r(lhs, rhs->r);
}

r32 interval_overlap(interval * lhs, interval * rhs)
{
	r32 diff1 = lhs->r - rhs->l;
	r32 diff2 = rhs->r - lhs->l;
	return diff1 < 0 || diff2 < 0 ? 0 : fminf(fminf(diff1, diff2), fminf(interval_length(lhs), interval_length(rhs)));
}
