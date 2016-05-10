#include "violet/math/line.h"
#include "violet/math/math.h"

void line_extrapolate(const v2 * a, const v2 * b, r32 t, v2 * p)
{
	v2 ab;
	v2_sub(b, a, &ab);
	v2_scale(&ab, t, t, p);
	v2_add(p, a, p);
}

b8 ray_intersect_coords(const v2 * a, const v2 * adir,
                        const v2 * b, const v2 * bdir, r32 * t, r32 * u)
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

	v2 ab;
	v2_sub(b, a, &ab);
	const r32 rxs = v2_cross(adir, bdir);

	if (rxs == 0)
	{
		// parallel or collinear
		// treating collinear lines as not intersecting
		return false;
	}
	else
	{
		*t = v2_cross(&ab, bdir) / rxs;
		*u = v2_cross(&ab, adir) / rxs;
		return true;
	}
}

b8 line_intersect_coords(const v2 * a0, const v2 * a1,
                         const v2 * b0, const v2 * b1, r32 * t, r32 * u)
{
	v2 a;
	v2_sub(a1, a0, &a);
	v2 b;
	v2_sub(b1, b0, &b);
	return ray_intersect_coords(a0, &a, b0, &b, t, u);
}

b8 line_intersect(const v2 * a0, const v2 * a1,
                  const v2 * b0, const v2 * b1, v2 * isec)
{
	r32 t, u;
	const b8 result = line_intersect_coords(a0, a1, b0, b1, &t, &u);
	line_extrapolate(a0, a1, t, isec);
	return result;
}

b8 segment_intersect(const v2 * a0, const v2 * a1,
                     const v2 * b0, const v2 * b1, v2 * isec)
{
	r32 t, u;
	const b8 result = line_intersect_coords(a0, a1, b0, b1, &t, &u)
	    && 0 < u && u < 1 && 0 < t && t < 1;
	line_extrapolate(a0, a1, t, isec);
	return result;
}

void nearest_point_on_segment(const v2 * a, const v2 * b, const v2 * p,
                              v2 * result)
{
	v2 ab, ap, proj;
	v2_sub(b, a, &ab);
	v2_sub(p, a, &ap);
	const r32 t = clamp(0, v2_dot(&ap, &ab) / v2_mag_squared(&ab), 1);
	v2_scale(&ab, t, t, &proj);
	v2_add(a, &proj, result);
}

void nearest_point_on_line(const v2 * a, const v2 * b, const v2 * p,
                           v2 * result)
{
	v2 ab, ap, proj;
	v2_sub(b, a, &ab);
	v2_sub(p, a, &ap);
	const r32 t = v2_dot(&ap, &ab) / v2_mag_squared(&ab);
	v2_scale(&ab, t, t, &proj);
	v2_add(a, &proj, result);
}

r32 point_to_segment_dist(const v2 * a, const v2 * b, const v2 * p)
{
	v2 nearest_point;
	nearest_point_on_segment(a, b, p, &nearest_point);
	return v2_dist(p, &nearest_point);
}

r32 point_to_line_dist(const v2 * a, const v2 * b, const v2 * p)
{
	v2 nearest_point;
	nearest_point_on_line(a, b, p, &nearest_point);
	return v2_dist(p, &nearest_point);
}
