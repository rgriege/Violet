#ifdef _MATH_DEFINES
#include <math.h>

#include "violet/core/macros.h"
#include "violet/core/types.h"

#define V2 CONCAT(v2, SUFFIX)
#define V2_(name) CONCAT(V2, CONCAT(_, name))
#define V2G_(name) CONCAT(g_, V2_(name))

#endif // _MATH_DEFINES

#ifdef _MATH_INTERFACE

typedef struct V2
{
	union
	{
		struct { SCALAR x, y; };
		struct { SCALAR u, v; };
	};
} V2;

static const V2 V2G_(x_axis) = { 1, 0 };
static const V2 V2G_(y_axis) = { 0, 1 };
static const V2 V2G_(zero) = { 0, 0 };

static inline void V2_(set)(V2 *v, SCALAR x, SCALAR y);
static inline SCALAR V2_(mag)(const V2 *v);
static inline SCALAR V2_(mag_sq)(const V2 *v);
static inline SCALAR V2_(dist)(const V2 *lhs, const V2 *rhs);
static inline SCALAR V2_(dist_sq)(const V2 *lhs, const V2 *rhs);
static inline void V2_(normalize)(const V2 *v, V2 *result);
static inline b8 V2_(is_unit)(const V2 *v);
static inline void V2_(scale)(const V2 *v, SCALAR sx, SCALAR sy,
                              V2 *result);
static inline void V2_(scale_inv)(const V2 *v, SCALAR sx, SCALAR sy,
                                  V2 *result);
static inline void V2_(add)(const V2 *lhs, const V2 *rhs, V2 *result);
static inline void V2_(sub)(const V2 *lhs, const V2 *rhs, V2 *result);
static inline void V2_(mul)(const V2 *lhs, const V2 *rhs, V2 *result);
static inline void V2_(div)(const V2 *lhs, const V2 *rhs, V2 *result);
static inline void V2_(rot)(const V2 *v, SCALAR radians, V2 *result);
static inline SCALAR V2_(dot)(const V2 *lhs, const V2 *rhs);
static inline SCALAR V2_(cross)(const V2 *lhs, const V2 *rhs);
static inline void V2_(proj)(const V2 *v, const V2 *axis, V2 *result);
static inline void V2_(perp)(const V2 *v, b8 left, V2 *result);
static inline void V2_(inverse)(const V2 *v, V2 *result);
static inline b8 V2_(is_zero)(const V2 *v);
static inline b8 V2_(equal)(const V2 *lhs, const V2 *rhs);
static inline b8 V2_(share_quadrant)(const V2 *lhs, const V2 *rhs);


/* Implementation */

static inline void V2_(set)(V2 *v, SCALAR x, SCALAR y)
{
	v->x = x;
	v->y = y;
}

static inline SCALAR V2_(mag)(const V2 *v)
{
	return sqrt(V2_(mag_sq)(v));
}

static inline SCALAR V2_(mag_sq)(const V2 *v)
{
	return v->x * v->x + v->y * v->y;
}

static inline SCALAR V2_(dist)(const V2 *lhs, const V2 *rhs)
{
	return sqrt(V2_(dist_sq)(lhs, rhs));
}

static inline SCALAR V2_(dist_sq)(const V2 *lhs, const V2 *rhs)
{
	V2 diff;
	V2_(sub)(lhs, rhs, &diff);
	return V2_(mag_sq)(&diff);
}

static inline void V2_(normalize)(const V2 *v, V2 *result)
{
	const SCALAR mag = V2_(mag)(v);
	V2_(scale_inv)(v, mag, mag, result);
}

static inline b8 V2_(is_unit)(const V2 *v)
{
	return V2_(mag_sq)(v) == 1;
}

static inline
void V2_(scale)(const V2 *v, SCALAR sx, SCALAR sy, V2 *result)
{
	result->x = v->x * sx;
	result->y = v->y * sy;
}

static inline
void V2_(scale_inv)(const V2 *v, SCALAR sx, SCALAR sy, V2 *result)
{
	result->x = v->x / sx;
	result->y = v->y / sy;
}

static inline void V2_(add)(const V2 *lhs, const V2 *rhs, V2 *result)
{
	result->x = lhs->x + rhs->x;
	result->y = lhs->y + rhs->y;
}

static inline void V2_(sub)(const V2 *lhs, const V2 *rhs, V2 *result)
{
	result->x = lhs->x - rhs->x;
	result->y = lhs->y - rhs->y;
}

static inline void V2_(mul)(const V2 *lhs, const V2 *rhs, V2 *result)
{
	result->x = lhs->x * rhs->x;
	result->y = lhs->y * rhs->y;
}

static inline void V2_(div)(const V2 *lhs, const V2 *rhs, V2 *result)
{
	result->x = lhs->x / rhs->x;
	result->y = lhs->y / rhs->y;
}

static inline void V2_(rot)(const V2 *v, SCALAR radians, V2 *result)
{
	SCALAR x = v->x;
	const SCALAR c = cos(radians);
	const SCALAR s = sin(radians);
	result->x = x * c - v->y * s;
	result->y = x * s + v->y * c;
}

static inline SCALAR V2_(dot)(const V2 *lhs, const V2 *rhs)
{
	return lhs->x * rhs->x + lhs->y * rhs->y;
}

static inline SCALAR V2_(cross)(const V2 *lhs, const V2 *rhs)
{
	return lhs->x * rhs->y - lhs->y * rhs->x;
}

static inline void V2_(proj)(const V2 *v, const V2 *axis, V2 *result)
{
	V2 dir;
	V2_(normalize)(axis, &dir);
	SCALAR scale = V2_(dot)(v, &dir);
	V2_(scale)(&dir, scale, scale, result);
}

static inline void V2_(perp)(const V2 *v, b8 left, V2 *result)
{
	if (left)
	{
		const SCALAR y = v->x;
		result->x = -v->y;
		result->y = y;
	}
	else
	{
		const SCALAR y = -v->x;
		result->x = v->y;
		result->y = y;
	}
}

static inline void V2_(inverse)(const V2 *v, V2 *result)
{
	result->x = -v->x;
	result->y = -v->y;
}

static inline b8 V2_(is_zero)(const V2 *v)
{
	return v->x == 0 && v->y == 0;
}

static inline b8 V2_(equal)(const V2 *lhs, const V2 *rhs)
{
	return lhs->x == rhs->x && lhs->y == rhs->y;
}

static inline b8 V2_(share_quadrant)(const V2 *lhs, const V2 *rhs)
{
	return V2_(dot)(lhs, rhs) >= 0;
}

#endif // _MATH_INTERFACE

#ifdef _MATH_UNDEFINES
#undef V2_
#undef V2
#endif // _MATH_UNDEFINES

