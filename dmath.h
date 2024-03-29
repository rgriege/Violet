#ifndef VIOLET_DMATH_H
#define VIOLET_DMATH_H

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* General utilities */

#define fmax(x, y) (((x) < (y)) ? (y) : (x))
#define fmin(x, y) (((x) > (y)) ? (y) : (x))

#define PI 3.14159265359
#define DEG2RAD (PI / 180)

#define dmath_clamp(lo, val, hi) fmax(lo, fmin(hi, val))
#define dmath_deg2rad(deg) ((deg) * DEG2RAD)
#define dmath_rad2deg(rad) ((rad) / DEG2RAD)
#define dmath_eq(a, b, err) (fabs((a)-(b)) <= (err))

/* 2D Vector */

typedef struct v2d
{
	r64 x, y;
} v2d;

extern const v2d g_v2d_zero;

void v2d_set(v2d *v, r64 x, r64 y);
r64  v2d_mag(v2d v);
r64  v2d_mag_sq(v2d v);
r64  v2d_dist(v2d lhs, v2d rhs);
r64  v2d_dist_sq(v2d lhs, v2d rhs);
v2d  v2d_normalize(v2d v);
void v2d_normalize_eq(v2d *v);
v2d  v2d_scale(v2d v, r64 s);
void v2d_scale_eq(v2d *v, r64 s);
v2d  v2d_add(v2d lhs, v2d rhs);
void v2d_add_eq(v2d *lhs, v2d rhs);
v2d  v2d_sub(v2d lhs, v2d rhs);
void v2d_sub_eq(v2d *lhs, v2d rhs);
v2d  v2d_mul(v2d lhs, v2d rhs);
void v2d_mul_eq(v2d *lhs, v2d rhs);
v2d  v2d_div(v2d lhs, v2d rhs);
void v2d_div_eq(v2d *lhs, v2d rhs);
r64  v2d_dot(v2d lhs, v2d rhs);
r64  v2d_cross(v2d lhs, v2d rhs);
v2d  v2d_lperp(v2d v);
v2d  v2d_rperp(v2d v);
v2d  v2d_inverse(v2d v);
void v2d_invert(v2d *v);
b32  v2d_equal(v2d lhs, v2d rhs);
v2d  v2d_midpoint(v2d v0, v2d v1);

/* 2x2 Matrix */

typedef union m2d
{
	r64 a[4];
	v2d v[2];
} m2d;

void m2d_init_rot(m2d *m, r64 radians);
v2d  m2d_mul_v2d(m2d m, v2d v);

/* 2D Anti-aliased bounding box */

typedef struct box2d
{
	v2d min;
	v2d max;
} box2d;

void box2d_from_xywh(box2d *b, r64 x, r64 y, r64 w, r64 h);
void box2d_from_point(box2d *b, v2d p);
void box2d_from_dims(box2d *box, r64 left, r64 top, r64 right, r64 bottom);
void box2d_extend_point(box2d *b, v2d p);

/* Polygon */

b32 polyd_is_cc(const v2d *v, u32 n);

#endif // VIOLET_DMATH_H


/* Implementation */

#ifdef DMATH_IMPLEMENTATION

/* 2D Vector */

const v2d g_v2d_zero = { 0, 0 };

void v2d_set(v2d *v, r64 x, r64 y)
{
	v->x = x;
	v->y = y;
}

r64 v2d_mag(v2d v)
{
	return sqrt(v2d_mag_sq(v));
}

r64 v2d_mag_sq(v2d v)
{
	return v2d_dot(v, v);
}

r64 v2d_dist(v2d lhs, v2d rhs)
{
	return sqrt(v2d_dist_sq(lhs, rhs));
}

r64 v2d_dist_sq(v2d lhs, v2d rhs)
{
	return v2d_mag_sq(v2d_sub(lhs, rhs));
}

v2d v2d_normalize(v2d v)
{
	return v2d_scale(v, 1.0 / v2d_mag(v));
}

void v2d_normalize_eq(v2d *v)
{
	*v = v2d_normalize(*v);
}

v2d v2d_scale(v2d v, r64 s)
{
	return (v2d){ .x = v.x *s, .y = v.y *s };
}

void v2d_scale_eq(v2d *v, r64 s)
{
	*v = v2d_scale(*v, s);
}

v2d v2d_add(v2d lhs, v2d rhs)
{
	return (v2d){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y };
}

void v2d_add_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_add(*lhs, rhs);
}

v2d v2d_sub(v2d lhs, v2d rhs)
{
	return (v2d){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
}

void v2d_sub_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_sub(*lhs, rhs);
}

v2d v2d_mul(v2d lhs, v2d rhs)
{
	return (v2d){ .x = lhs.x * rhs.x, .y = lhs.y * rhs.y };
}

void v2d_mul_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_mul(*lhs, rhs);
}

v2d v2d_div(v2d lhs, v2d rhs)
{
	return (v2d){ .x = lhs.x / rhs.x, .y = lhs.y / rhs.y };
}

void v2d_div_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_div(*lhs, rhs);
}

r64 v2d_dot(v2d lhs, v2d rhs)
{
	const v2d result = v2d_mul(lhs, rhs);
	return result.x + result.y;
}

r64 v2d_cross(v2d lhs, v2d rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

v2d v2d_lperp(v2d v)
{
	return (v2d){ .x = -v.y, .y = v.x };
}

v2d v2d_rperp(v2d v)
{
	return (v2d){ .x = v.y, .y = -v.x };
}

v2d v2d_inverse(v2d v)
{
	return (v2d){ .x = -v.x, .y = -v.y };
}

void v2d_invert(v2d *v)
{
	v2d_set(v, -v->x, -v->y);
}

b32 v2d_equal(v2d lhs, v2d rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

v2d v2d_midpoint(v2d v0, v2d v1)
{
	return v2d_scale(v2d_add(v0, v1), 0.5);
}

/* 2x2 Matrix */

void m2d_init_rot(m2d *m, r64 radians)
{
	v2d_set(&m->v[0], cos(radians), -sin(radians));
	v2d_set(&m->v[1], -m->a[1], m->a[0]);
}

v2d m2d_mul_v2d(m2d m, v2d v)
{
	return (v2d) {
		.x = v2d_dot(m.v[0], v),
		.y = v2d_dot(m.v[1], v),
	};
}

/* 2D Anti-aliased bounding box */

void box2d_from_xywh(box2d *box, r64 x, r64 y, r64 w, r64 h)
{
	box->min.x = x;
	box->min.y = y;
	box->max.x = x + w;
	box->max.y = y + h;
}

void box2d_from_point(box2d *b, v2d p)
{
	b->min = p;
	b->max = p;
}

void box2d_from_dims(box2d *box, r64 left, r64 top, r64 right, r64 bottom)
{
	box->min.x = left;
	box->min.y = bottom;
	box->max.x = right;
	box->max.y = top;
}

void box2d_extend_point(box2d *box, v2d p)
{
	box2d_from_dims(box, fmin(box->min.x, p.x), fmax(box->max.y, p.y),
	                     fmax(box->max.x, p.x), fmin(box->min.y, p.y));
}

/* Polygon */

b32 polyd_is_cc(const v2d *v, u32 n)
{
	const v2d *a, *b, *c;
	v2d ab, bc;
	r64 sine_sum = 0;

	assert(n>=3);

	a = &v[n-1];
	for (u32 i = 0; i < n; ++i) {
		b = &v[i];
		c = &v[(i+1)%n];

		ab = v2d_sub(*b, *a);
		bc = v2d_sub(*c, *b);

		sine_sum += v2d_cross(ab, bc) / v2d_mag(ab) / v2d_mag(bc);
	}
	return sine_sum > 0;
}

#undef DMATH_IMPLEMENTATION
#endif // DMATH_IMPLEMENTATION
