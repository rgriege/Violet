#include "violet/math/math.h"

r32 clamp(r32 lo, r32 val, r32 hi)
{
	return max(lo, min(hi, val));
}

u32 usub_capped(u32 lhs, u32 rhs)
{
	return lhs > rhs ? lhs - rhs : 0;
}
