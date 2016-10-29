#include <stdlib.h>

#ifdef HAS_DECIMAL
static const SCALAR g_d2r = PI / 180;
#endif

SCALAR MATH_(clamp)(SCALAR lo, SCALAR val, SCALAR hi)
{
	return max(lo, min(hi, val));
}

#ifdef HAS_DECIMAL
SCALAR MATH_(deg2rad)(SCALAR deg)
{
	return deg * g_d2r;
}

SCALAR MATH_(rad2deg)(SCALAR rad)
{
	return rad / g_d2r;
}
#endif

b8 MATH_(eq)(SCALAR a, SCALAR b, SCALAR err)
{
	return ABS(a-b) <= err;
}

