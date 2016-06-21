static const SCALAR g_d2r = PI / 180;

SCALAR MATH_(clamp)(SCALAR lo, SCALAR val, SCALAR hi)
{
	return max(lo, min(hi, val));
}

SCALAR MATH_(deg2rad)(SCALAR deg)
{
	return deg * g_d2r;
}

SCALAR MATH_(rad2deg)(SCALAR rad)
{
	return rad / g_d2r;
}
