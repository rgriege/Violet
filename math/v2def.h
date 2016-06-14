#include <math.h>

#include "violet/core/macros.h"
#include "violet/core/types.h"

#define V2 CONCAT(v2, V2_SUFFIX)
#define V2_PFX(name) CONCAT(V2, name)

typedef struct V2
{
	union
	{
		struct { V2_TYPE x, y; };
		struct { V2_TYPE u, v; };
	};
} V2;

static const V2 CONCAT(g_, V2_PFX(_x_axis)) = { 1, 0 };
static const V2 CONCAT(g_, V2_PFX(_y_axis)) = { 0, 1 };
static const V2 CONCAT(g_, V2_PFX(_zero)) = { 0, 0 };

static inline V2_TYPE V2_PFX(_mag)(const V2 * v);
static inline V2_TYPE V2_PFX(_mag_squared)(const V2 * v);
static inline V2_TYPE V2_PFX(_dist)(const V2 * lhs, const V2 * rhs);
static inline void V2_PFX(_normalize)(const V2 * v, V2 * result);
static inline b8 V2_PFX(_is_unit)(const V2 * v);
static inline void V2_PFX(_scale)(const V2 * v, V2_TYPE sx, V2_TYPE sy, V2 * result);
static inline void V2_PFX(_add)(const V2 * lhs, const V2 * rhs, V2 * result);
static inline void V2_PFX(_sub)(const V2 * lhs, const V2 * rhs, V2 * result);
static inline void V2_PFX(_mul)(const V2 * lhs, const V2 * rhs, V2 * result);
static inline void V2_PFX(_div)(const V2 * lhs, const V2 * rhs, V2 * result);
static inline void V2_PFX(_rot)(const V2 * v, V2_TYPE radians, V2 * result);
static inline V2_TYPE V2_PFX(_dot)(const V2 * lhs, const V2 * rhs);
static inline V2_TYPE V2_PFX(_cross)(const V2 * lhs, const V2 * rhs);
static inline void V2_PFX(_proj)(const V2 * v, const V2 * axis, V2 * result);
static inline void V2_PFX(_perp)(const V2 * v, b8 left, V2 * result);
static inline void V2_PFX(_inverse)(const V2 * v, V2 * result);
static inline b8 V2_PFX(_is_zero)(const V2 * v);
static inline b8 V2_PFX(_equal)(const V2 * lhs, const V2 * rhs);
static inline b8 V2_PFX(_share_quadrant)(const V2 * lhs, const V2 * rhs);


/* Implementation */

static inline V2_TYPE V2_PFX(_mag)(const V2 * v)
{
	return sqrt(V2_PFX(_mag_squared)(v));
}

static inline V2_TYPE V2_PFX(_mag_squared)(const V2 * v)
{
	return v->x * v->x + v->y * v->y;
}

static inline V2_TYPE V2_PFX(_dist)(const V2 * lhs, const V2 * rhs)
{
	V2 diff;
	V2_PFX(_sub)(lhs, rhs, &diff);
	return V2_PFX(_mag)(&diff);
}

static inline void V2_PFX(_normalize)(const V2 * v, V2 * result)
{
	const V2_TYPE inv_mag = 1 / V2_PFX(_mag)(v);
	V2_PFX(_scale)(v, inv_mag, inv_mag, result);
}

static inline b8 V2_PFX(_is_unit)(const V2 * v)
{
	return V2_PFX(_mag_squared)(v) == 1;
}

static inline void V2_PFX(_scale)(const V2 * v, V2_TYPE sx, V2_TYPE sy, V2 * result)
{
	result->x = v->x * sx;
	result->y = v->y * sy;
}

static inline void V2_PFX(_add)(const V2 * lhs, const V2 * rhs, V2 * result)
{
	result->x = lhs->x + rhs->x;
	result->y = lhs->y + rhs->y;
}

static inline void V2_PFX(_sub)(const V2 * lhs, const V2 * rhs, V2 * result)
{
	result->x = lhs->x - rhs->x;
	result->y = lhs->y - rhs->y;
}

static inline void V2_PFX(_mul)(const V2 * lhs, const V2 * rhs, V2 * result)
{
	result->x = lhs->x * rhs->x;
	result->y = lhs->y * rhs->y;
}

static inline void V2_PFX(_div)(const V2 * lhs, const V2 * rhs, V2 * result)
{
	result->x = lhs->x / rhs->x;
	result->y = lhs->y / rhs->y;
}

static inline void V2_PFX(_rot)(const V2 * v, V2_TYPE radians, V2 * result)
{
	V2_TYPE x = v->x;
	const V2_TYPE c = cos(radians);
	const V2_TYPE s = sin(radians);
	result->x = x * c - v->y * s;
	result->y = x * s + v->y * c;
}

static inline V2_TYPE V2_PFX(_dot)(const V2 * lhs, const V2 * rhs)
{
	return lhs->x * rhs->x + lhs->y * rhs->y;
}

static inline V2_TYPE V2_PFX(_cross)(const V2 * lhs, const V2 * rhs)
{
	return lhs->x * rhs->y - lhs->y * rhs->x;
}

static inline void V2_PFX(_proj)(const V2 * v, const V2 * axis, V2 * result)
{
	V2 dir;
	V2_PFX(_normalize)(axis, &dir);
	V2_TYPE scale = V2_PFX(_dot)(v, &dir);
	V2_PFX(_scale)(&dir, scale, scale, result);
}

static inline void V2_PFX(_perp)(const V2 * v, b8 left, V2 * result)
{
	if (left)
	{
		const V2_TYPE y = v->x;
		result->x = -v->y;
		result->y = y;
	}
	else
	{
		const V2_TYPE y = -v->x;
		result->x = v->y;
		result->y = y;
	}
}

static inline void V2_PFX(_inverse)(const V2 * v, V2 * result)
{
	result->x = -v->x;
	result->y = -v->y;
}

static inline b8 V2_PFX(_is_zero)(const V2 * v)
{
	return v->x == 0 && v->y == 0;
}

static inline b8 V2_PFX(_equal)(const V2 * lhs, const V2 * rhs)
{
	return lhs->x == rhs->x && lhs->y == rhs->y;
}

static inline b8 V2_PFX(_share_quadrant)(const V2 * lhs, const V2 * rhs)
{
	return V2_PFX(_dot)(lhs, rhs) >= 0;
}

#undef V2_PFX
#undef V2

