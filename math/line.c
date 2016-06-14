#include "violet/math/line.h"
#include "violet/math/math.h"

void line_extrapolate(const v2f * a, const v2f * b, r32 t, v2f * p)
{
	v2f ab;
	v2f_sub(b, a, &ab);
	v2f_scale(&ab, t, t, p);
	v2f_add(p, a, p);
}

b8 ray_intersect_coords(const v2f * a, const v2f * adir,
                        const v2f * b, const v2f * bdir, r32 * t, r32 * u)
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

	v2f ab;
	v2f_sub(b, a, &ab);
	const r32 rxs = v2f_cross(adir, bdir);

	if (rxs == 0)
	{
		// parallel or collinear
		// treating collinear lines as not intersecting
		return false;
	}
	else
	{
		*t = v2f_cross(&ab, bdir) / rxs;
		*u = v2f_cross(&ab, adir) / rxs;
		return true;
	}
}

b8 line_intersect_coords(const v2f * a0, const v2f * a1,
                         const v2f * b0, const v2f * b1, r32 * t, r32 * u)
{
	v2f a;
	v2f_sub(a1, a0, &a);
	v2f b;
	v2f_sub(b1, b0, &b);
	return ray_intersect_coords(a0, &a, b0, &b, t, u);
}

b8 line_intersect(const v2f * a0, const v2f * a1,
                  const v2f * b0, const v2f * b1, v2f * isec)
{
	r32 t, u;
	const b8 result = line_intersect_coords(a0, a1, b0, b1, &t, &u);
	line_extrapolate(a0, a1, t, isec);
	return result;
}

b8 segment_intersect(const v2f * a0, const v2f * a1,
                     const v2f * b0, const v2f * b1, v2f * isec)
{
	r32 t, u;
	const b8 result = line_intersect_coords(a0, a1, b0, b1, &t, &u)
	    && 0 < u && u < 1 && 0 < t && t < 1;
	line_extrapolate(a0, a1, t, isec);
	return result;
}

void nearest_point_on_segment(const v2f * a, const v2f * b, const v2f * p,
                              v2f * result)
{
	v2f ab, ap, proj;
	v2f_sub(b, a, &ab);
	v2f_sub(p, a, &ap);
	const r32 t = clamp(0, v2f_dot(&ap, &ab) / v2f_mag_squared(&ab), 1);
	v2f_scale(&ab, t, t, &proj);
	v2f_add(a, &proj, result);
}

void nearest_point_on_line(const v2f * a, const v2f * b, const v2f * p,
                           v2f * result)
{
	v2f ab, ap, proj;
	v2f_sub(b, a, &ab);
	v2f_sub(p, a, &ap);
	const r32 t = v2f_dot(&ap, &ab) / v2f_mag_squared(&ab);
	v2f_scale(&ab, t, t, &proj);
	v2f_add(a, &proj, result);
}

r32 point_to_segment_dist(const v2f * a, const v2f * b, const v2f * p)
{
	v2f nearest_point;
	nearest_point_on_segment(a, b, p, &nearest_point);
	return v2f_dist(p, &nearest_point);
}

r32 point_to_line_dist(const v2f * a, const v2f * b, const v2f * p)
{
	v2f nearest_point;
	nearest_point_on_line(a, b, p, &nearest_point);
	return v2f_dist(p, &nearest_point);
}
