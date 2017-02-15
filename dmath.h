#ifndef DMATH_H
#define DMATH_H

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef DMATH_NO_SSE
#include <x86intrin.h>
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
#define DEG2RAD PI / 180

#define dmath_clamp(lo, val, hi) fmax(lo, fmin(hi, val))
#define dmath_deg2rad(deg) (deg * DEG2RAD)
#define dmath_rad2deg(rad) (rad / DEG2RAD)
#define dmath_eq(a, b, err) (fabs(a-b) <= err)

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
DMDEF v2d  v2d_scale(v2d v, r64 s);
DMDEF void v2d_scale_eq(v2d *v, r64 s);
DMDEF v2d  v2d_add(v2d lhs, v2d rhs);
DMDEF void v2d_add_eq(v2d *lhs, v2d rhs);
DMDEF v2d  v2d_sub(v2d lhs, v2d rhs);
DMDEF void v2d_sub_eq(v2d *lhs, v2d rhs);
DMDEF v2d  v2d_div(v2d lhs, v2d rhs);
DMDEF void v2d_div_eq(v2d *lhs, v2d rhs);
DMDEF b32  v2d_equal(v2d lhs, v2d rhs);

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
	v->v = _mm_set_pd(x, y);
#else
	v->x = x;
	v->y = y;
#endif
}

DMDEF v2d v2d_scale(v2d v, r64 s)
{
#ifndef DMATH_NO_SSE
	SSE_RETURN_V2D(_mm_mul_pd(v.v, _mm_set_pd1(s)));
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

DMDEF b32 v2d_equal(v2d lhs, v2d rhs)
{
#ifndef DMATH_NO_SSE
	return _mm_movemask_pd(_mm_cmpeq_pd(lhs.v, rhs.v)) == 0x3;
#else
	return lhs.x == rhs.x && lhs.y == rhs.y;
#endif
}

#undef DMATH_IMPLEMENTATION
#endif // DMATH_IMPLEMENTATION