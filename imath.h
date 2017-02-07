#ifndef IMATH_H
#define IMATH_H

#ifdef IMATH_STANDALONE
typedef unsigned b32;
typedef int s32;
#define false 0
#define true 1
#endif

#ifndef IMATH_NO_INLINE
#define IMDEF static inline
#define IMGDECL static
#define IMGDEF static
#undef IMATH_STATIC
#define IMATH_STATIC
#elif IMATH_STATIC
#define IMDEF static
#define IMGDECL static
#define IMGDEF static
#else
#define IMDEF
#define IMGDECL extern
#define IMGDEF
#endif

#if defined IMATH_STATIC && !defined IMATH_IMPLEMENTATION
#define IMATH_IMPLEMENTATION
#endif

/* General utilities */

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define max(x, y) (((x) < (y)) ? (y) : (x))
#define min(x, y) (((x) > (y)) ? (y) : (x))

#define imath_clamp(lo, val, hi) max(lo, min(hi, val))

/* 2D Vector */

typedef struct v2i
{
	s32 x, y;
} v2i;

FMGDECL const v2i g_v2i_zero;

IMDEF s32  v2i_mag_sq(v2i v);
IMDEF s32  v2i_dist_sq(v2i lhs, v2i rhs);
IMDEF v2i  v2i_div(v2i lhs, v2i rhs);
IMDEF void v2i_div_eq(v2i *lhs, v2i rhs);
IMDEF b32  v2i_equal(v2i lhs, v2i rhs);

/* 2D Anti-aliased bounding box */

typedef struct box2i
{
	v2i min;
	v2i max;
} box2i;

IMDEF void box2i_from_dims(box2i *b, s32 left, s32 top, s32 right, s32 bottom);
IMDEF b32  box2i_contains_point(box2i b, v2i p);

#endif // IMATH_H


/* Implementation */


#ifdef IMATH_IMPLEMENTATION

/* 2D Vector */

FMGDEF const v2i g_v2i_zero = { 0, 0 };

IMDEF s32 v2i_mag_sq(v2i v)
{
	return v.x * v.x + v.y * v.y;
}

IMDEF s32 v2i_dist_sq(v2i lhs, v2i rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return v2i_mag_sq(lhs);
}

IMDEF v2i v2i_div(v2i lhs, v2i rhs)
{
	return (v2i){ .x = lhs.x / rhs.x, .y = lhs.y / rhs.y };
}

IMDEF void v2i_div_eq(v2i *lhs, v2i rhs)
{
	lhs->x /= rhs.x;
	lhs->y /= rhs.y;
}

IMDEF b32 v2i_equal(v2i lhs, v2i rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

/* 2D Anti-aliased bounding box */

IMDEF void box2i_from_dims(box2i *box, s32 left, s32 top, s32 right, s32 bottom)
{
	box->min.x = left;
	box->min.y = bottom;
	box->max.x = right;
	box->max.y = top;
}

IMDEF b32 box2i_contains_point(box2i box, v2i point)
{
	return    point.x >= box.min.x
	       && point.x <= box.max.x
	       && point.y >= box.min.y
	       && point.y <= box.max.y;
}

#undef IMATH_IMPLEMENTATION
#endif // IMATH_IMPLEMENTATION
