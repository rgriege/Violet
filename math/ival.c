#include <math.h>
#include "violet/math/math.h"


const IVAL IVALG_(0_to_1) = { 0, 1 };


void IVAL_(slide)(IVAL *i, SCALAR d)
{
	i->l += d;
	i->r += d;
}

SCALAR IVAL_(length)(const IVAL *i)
{
	return i->r - i->l;
}

b8 IVAL_(contains_val)(const IVAL *i, SCALAR x)
{
	return x >= i->l && x <= i->r;
}

b8 IVAL_(contains_ival)(const IVAL *lhs, const IVAL *rhs)
{
	return IVAL_(contains_val)(lhs, rhs->l) && IVAL_(contains_val)(lhs, rhs->r);
}

b8 IVAL_(overlaps)(const IVAL *lhs, const IVAL *rhs)
{
	return IVAL_(contains_val)(lhs, rhs->l) || IVAL_(contains_val)(lhs, rhs->r);
}

SCALAR IVAL_(overlap)(const IVAL *lhs, const IVAL *rhs)
{
	SCALAR diff1 = lhs->r - rhs->l;
	SCALAR diff2 = rhs->r - lhs->l;
	return diff1 < 0 || diff2 < 0 ? 0 : min(min(diff1, diff2), min(IVAL_(length)(lhs), IVAL_(length)(rhs)));
}
