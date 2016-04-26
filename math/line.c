#include "violet/math/line.h"

b8 line_intersect_coords(const v2 * a0, const v2 * a1,
                         const v2 * b0, const v2 * b1, r32 * t, r32 * u)
{
	/*
	 * (uppercase = vector, lowercase = scalar)
	 * 2 lines PR and QS intersect if there exists scalars t & u such that
	 * P + tR = Q + uS
	 *
	 * (P + tR) x S = (Q + uS) x S
	 * t = (Q - P) X S / (R x S)
	 * u = (Q - P) X R / (R X S)
	 */

	v2 r;
	v2_sub(a1, a0, &r);
	v2 s;
	v2_sub(b1, b0, &s);

	v2 qmp;
	v2_sub(b0, a0, &qmp);
	const r32 rxs = v2_cross(&r, &s);

	if (rxs == 0)
	{
		// parallel or collinear
		// treating collinear lines as not intersecting
		return false;
	}
	else
	{
		*t = v2_cross(&qmp, &s) / rxs;
		*u = v2_cross(&qmp, &r) / rxs;
		return true;
	}
}
static void _extrapolate(const v2 * a, const v2 * b, r32 t, v2 * p)
{
	v2 ab;
	v2_sub(b, a, &ab);
	v2_scale(&ab, t, t, p);
	v2_add(p, a, p);
}

b8 line_intersect(const v2 * a0, const v2 * a1,
                  const v2 * b0, const v2 * b1, v2 * isec)
{
	r32 t, u;
	const b8 result = line_intersect_coords(a0, a1, b0, b1, &t, &u);
	_extrapolate(a0, a1, t, isec);
	return result;
}

b8 segment_intersect(const v2 * a0, const v2 * a1,
                     const v2 * b0, const v2 * b1, v2 * isec)
{
	r32 t, u;
	const b8 result = line_intersect_coords(a0, a1, b0, b1, &t, &u)
	    && 0 < u && u < 1 && 0 < t && t < 1;
	_extrapolate(a0, a1, t, isec);
	return result;
}

