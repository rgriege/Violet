#ifndef DMATH_H
#define DMATH_H

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef DMATH_NO_SSE
#ifndef _WIN32
#include <x86intrin.h>
#else
#include <intrin.h>
#endif
#endif

#ifdef DMATH_STANDALONE
typedef unsigned b32;
typedef unsigned u32;
typedef double r64;
#define false 0
#define true 1
#endif

#ifndef DMATH_NO_INLINE
#define DMDEF static inline
#define DMGDECL static
#define DMGDEF static
#undef DMATH_STATIC
#define DMATH_STATIC
#elif DMATH_STATIC
#define DMDEF static
#define DMGDECL static
#define DMGDEF static
#else
#define DMDEF
#define DMGDECL extern
#define DMGDEF
#endif

#if defined DMATH_STATIC && !defined DMATH_IMPLEMENTATION
#define DMATH_IMPLEMENTATION
#endif

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
#ifndef DMATH_NO_SSE
	union
	{
		struct { r64 x, y; };
		__m128d v;
	};
#else
	r64 x, y;
#endif
} v2d;

DMGDECL const v2d g_v2d_zero;

DMDEF void v2d_set(v2d *v, r64 x, r64 y);
DMDEF r64  v2d_mag(v2d v);
DMDEF r64  v2d_mag_sq(v2d v);
DMDEF v2d  v2d_normalize(v2d v);
DMDEF void v2d_normalize_eq(v2d *v);
DMDEF v2d  v2d_scale(v2d v, r64 s);
DMDEF void v2d_scale_eq(v2d *v, r64 s);
DMDEF v2d  v2d_add(v2d lhs, v2d rhs);
DMDEF void v2d_add_eq(v2d *lhs, v2d rhs);
DMDEF v2d  v2d_sub(v2d lhs, v2d rhs);
DMDEF void v2d_sub_eq(v2d *lhs, v2d rhs);
DMDEF v2d  v2d_mul(v2d lhs, v2d rhs);
DMDEF void v2d_mul_eq(v2d *lhs, v2d rhs);
DMDEF v2d  v2d_div(v2d lhs, v2d rhs);
DMDEF void v2d_div_eq(v2d *lhs, v2d rhs);
DMDEF r64  v2d_dot(v2d lhs, v2d rhs);
DMDEF r64  v2d_cross(v2d lhs, v2d rhs);
DMDEF v2d  v2d_lperp(v2d v);
DMDEF v2d  v2d_rperp(v2d v);
DMDEF v2d  v2d_inverse(v2d v);
DMDEF void v2d_invert(v2d *v);
DMDEF b32  v2d_equal(v2d lhs, v2d rhs);

/* 2x2 Matrix */

typedef union m2d
{
	r64 a[4];
	v2d v[2];
} m2d;

DMDEF void m2d_init_rot(m2d *m, r64 radians);
DMDEF v2d  m2d_mul_v2d(m2d m, v2d v);

/* 2D Anti-aliased bounding box */

typedef struct box2d
{
	v2d min;
	v2d max;
} box2d;

DMDEF void box2d_from_xywh(box2d *b, r64 x, r64 y, r64 w, r64 h);
DMDEF void box2d_from_point(box2d *b, v2d p);
DMDEF void box2d_from_dims(box2d *box, r64 left, r64 top, r64 right, r64 bottom);
DMDEF void box2d_extend_point(box2d *b, v2d p);

#endif // DMATH_H


/* Implementation */

#ifdef DMATH_IMPLEMENTATION

/* 2D Vector */

DMGDEF const v2d g_v2d_zero = { 0, 0 };

#ifndef DMATH_NO_SSE
#define SSE_RETURN_V2D(expr) \
	do { v2d res = { .v = expr }; return res; } while(0)
#endif

DMDEF void v2d_set(v2d *v, r64 x, r64 y)
{
#ifndef DMATH_NO_SSE
	v->v = _mm_setr_pd(x, y);
#else
	v->x = x;
	v->y = y;
#endif
}

DMDEF r64 v2d_mag(v2d v)
{
	return sqrt(v2d_mag_sq(v));
}

DMDEF r64 v2d_mag_sq(v2d v)
{
	return v2d_dot(v, v);
}

DMDEF v2d v2d_normalize(v2d v)
{
	return v2d_scale(v, 1.0 / v2d_mag(v));
}

DMDEF void v2d_normalize_eq(v2d *v)
{
	*v = v2d_normalize(*v);
}

DMDEF v2d v2d_scale(v2d v, r64 s)
{
#ifndef DMATH_NO_SSE
	SSE_RETURN_V2D(_mm_mul_pd(v.v, _mm_set_pd(s, s)));
#else
	return (v2d){ .x = v.x *s, .y = v.y *s };
#endif
}

DMDEF void v2d_scale_eq(v2d *v, r64 s)
{
	*v = v2d_scale(*v, s);
}

DMDEF v2d v2d_add(v2d lhs, v2d rhs)
{
#ifndef DMATH_NO_SSE
	SSE_RETURN_V2D(_mm_add_pd(lhs.v, rhs.v));
#else
	return (v2d){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y };
#endif
}

DMDEF void v2d_add_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_add(*lhs, rhs);
}

DMDEF v2d v2d_sub(v2d lhs, v2d rhs)
{
#ifndef DMATH_NO_SSE
	SSE_RETURN_V2D(_mm_sub_pd(lhs.v, rhs.v));
#else
	return (v2d){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
#endif
}

DMDEF void v2d_sub_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_sub(*lhs, rhs);
}

DMDEF v2d v2d_mul(v2d lhs, v2d rhs)
{
#ifndef DMATH_NO_SSE
	SSE_RETURN_V2D(_mm_mul_pd(lhs.v, rhs.v));
#else
	return (v2d){ .x = lhs.x * rhs.x, .y = lhs.y * rhs.y };
#endif
}

DMDEF void v2d_mul_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_mul(*lhs, rhs);
}

DMDEF v2d v2d_div(v2d lhs, v2d rhs)
{
#ifndef DMATH_NO_SSE
	SSE_RETURN_V2D(_mm_div_pd(lhs.v, rhs.v));
#else
	return (v2d){ .x = lhs.x / rhs.x, .y = lhs.y / rhs.y };
#endif
}

DMDEF void v2d_div_eq(v2d *lhs, v2d rhs)
{
	*lhs = v2d_div(*lhs, rhs);
}

DMDEF r64 v2d_dot(v2d lhs, v2d rhs)
{
	const v2d result = v2d_mul(lhs, rhs);
	return result.x + result.y;
}

DMDEF r64 v2d_cross(v2d lhs, v2d rhs)
{
#ifndef DMATH_NO_SSE
	const v2d rhs_shuffle = {
		.v = _mm_shuffle_pd(rhs.v, rhs.v, _MM_SHUFFLE2(0, 1))
	};
	const v2d result = v2d_mul(lhs, rhs_shuffle);
	return result.x - result.y;
#else
	return lhs.x * rhs.y - lhs.y * rhs.x;
#endif
}

DMDEF v2d v2d_lperp(v2d v)
{
	return (v2d){ .x = -v.y, .y = v.x };
}

DMDEF v2d v2d_rperp(v2d v)
{
	return (v2d){ .x = v.y, .y = -v.x };
}

DMDEF v2d v2d_inverse(v2d v)
{
	return (v2d){ .x = -v.x, .y = -v.y };
}

DMDEF void v2d_invert(v2d *v)
{
	v2d_set(v, -v->x, -v->y);
}

DMDEF b32 v2d_equal(v2d lhs, v2d rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

/* 2x2 Matrix */

DMDEF void m2d_init_rot(m2d *m, r64 radians)
{
	v2d_set(&m->v[0], cos(radians), -sin(radians));
	v2d_set(&m->v[1], -m->a[1], m->a[0]);
}

DMDEF v2d m2d_mul_v2d(m2d m, v2d v)
{
	return (v2d) {
		.x = v2d_dot(m.v[0], v),
		.y = v2d_dot(m.v[1], v),
	};
}

/* 2D Anti-aliased bounding box */

DMDEF void box2d_from_xywh(box2d *box, r64 x, r64 y, r64 w, r64 h)
{
	box->min.x = x;
	box->min.y = y;
	box->max.x = x + w;
	box->max.y = y + h;
}

DMDEF void box2d_from_point(box2d *b, v2d p)
{
	b->min = p;
	b->max = p;
}

DMDEF void box2d_from_dims(box2d *box, r64 left, r64 top, r64 right, r64 bottom)
{
	box->min.x = left;
	box->min.y = bottom;
	box->max.x = right;
	box->max.y = top;
}

DMDEF void box2d_extend_point(box2d *box, v2d p)
{
	box2d_from_dims(box, fmin(box->min.x, p.x), fmax(box->max.y, p.y),
	                     fmax(box->max.x, p.x), fmin(box->min.y, p.y));
}

#undef DMATH_IMPLEMENTATION
#endif // DMATH_IMPLEMENTATION
