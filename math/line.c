#include "violet/math/math.h"

void MATH_(line_extrapolate)(const V2 *a, const V2 *b, SCALAR t, V2 *p)
{
	V2 ab;
	V2_(sub)(b, a, &ab);
	V2_(scale)(&ab, t, t, p);
	V2_(add)(p, a, p);
}

b8 MATH_(ray_intersect_coords)(const V2 *a, const V2 *adir,
                               const V2 *b, const V2 *bdir,
                               SCALAR *t, SCALAR *u)
{
	/*
	 * (uppercase = vector, lowercase = scalar)
	 * ray intersection equation
	 * A + tR = B + uS
	 *
	 * (A + tR) x S = (B + uS) x S
	 * t = (B - A) X S / (R x S)
	 * ...
	 * u = (B - A) X R / (R X S)
	 */

	V2 ab;
	V2_(sub)(b, a, &ab);
	const SCALAR rxs = V2_(cross)(adir, bdir);

	if (rxs == 0)
	{
		// parallel or collinear
		// treating collinear lines as not intersecting
		return false;
	}
	else
	{
		*t = V2_(cross)(&ab, bdir) / rxs;
		*u = V2_(cross)(&ab, adir) / rxs;
		return true;
	}
}

b8 MATH_(line_intersect_coords)(const V2 *a0, const V2 *a1,
                                const V2 *b0, const V2 *b1,
                                SCALAR *t, SCALAR *u)
{
	V2 a;
	V2_(sub)(a1, a0, &a);
	V2 b;
	V2_(sub)(b1, b0, &b);
	return MATH_(ray_intersect_coords)(a0, &a, b0, &b, t, u);
}

b8 MATH_(line_intersect)(const V2 *a0, const V2 *a1,
                         const V2 *b0, const V2 *b1, V2 *isec)
{
	SCALAR t, u;
	const b8 result = MATH_(line_intersect_coords)(a0, a1, b0, b1, &t, &u);
	MATH_(line_extrapolate)(a0, a1, t, isec);
	return result;
}

b8 MATH_(segment_intersect)(const V2 *a0, const V2 *a1,
                            const V2 *b0, const V2 *b1, V2 *isec)
{
	SCALAR t, u;
	const b8 result = MATH_(line_intersect_coords)(a0, a1, b0, b1, &t, &u)
	    && 0 < u && u < 1 && 0 < t && t < 1;
	MATH_(line_extrapolate)(a0, a1, t, isec);
	return result;
}

void MATH_(nearest_point_on_segment)(const V2 *a, const V2 *b, const V2 *p,
                                     V2 *result)
{
	V2 ab, ap, proj;
	V2_(sub)(b, a, &ab);
	V2_(sub)(p, a, &ap);
	const SCALAR t = MATH_(clamp)(0, V2_(dot)(&ap, &ab) / V2_(mag_squared)(&ab), 1);
	V2_(scale)(&ab, t, t, &proj);
	V2_(add)(a, &proj, result);
}

void MATH_(nearest_point_on_line)(const V2 *a, const V2 *b, const V2 *p,
                                  V2 *result)
{
	V2 ab, ap, proj;
	V2_(sub)(b, a, &ab);
	V2_(sub)(p, a, &ap);
	const SCALAR t = V2_(dot)(&ap, &ab) / V2_(mag_squared)(&ab);
	V2_(scale)(&ab, t, t, &proj);
	V2_(add)(a, &proj, result);
}

SCALAR MATH_(point_to_segment_dist)(const V2 *a, const V2 *b, const V2 *p)
{
	V2 nearest_point;
	MATH_(nearest_point_on_segment)(a, b, p, &nearest_point);
	return V2_(dist)(p, &nearest_point);
}

SCALAR MATH_(point_to_line_dist)(const V2 *a, const V2 *b, const V2 *p)
{
	V2 nearest_point;
	MATH_(nearest_point_on_line)(a, b, p, &nearest_point);
	return V2_(dist)(p, &nearest_point);
}

