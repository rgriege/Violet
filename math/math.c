#include "violet/math/math.h"

static const r32 g_d2r = PI / 180.f;

r32 clamp(r32 lo, r32 val, r32 hi)
{
	return max(lo, min(hi, val));
}

u32 usub_capped(u32 lhs, u32 rhs)
{
	return lhs > rhs ? lhs - rhs : 0;
}

r32 degrees_to_radians(r32 deg)
{
	return deg * g_d2r;
}

r32 radians_to_degrees(r32 rad)
{
	return rad / g_d2r;
}