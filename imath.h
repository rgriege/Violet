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

#define clamp(lo, val, hi) max(lo, min(hi, val))

/* 2D Vector */

typedef struct v2i
{
	s32 x, y;
} v2i;

IMGDECL const v2i g_v2i_zero;
IMGDECL const v2i g_v2i_x_axis;
IMGDECL const v2i g_v2i_y_axis;

IMDEF void v2i_set(v2i *v, s32 x, s32 y);
IMDEF s32  v2i_mag_sq(v2i v);
IMDEF s32  v2i_dist_sq(v2i lhs, v2i rhs);
IMDEF v2i  v2i_scale(v2i v, s32 s);
IMDEF void v2i_scale_eq(v2i *v, s32 s);
IMDEF v2i  v2i_add(v2i lhs, v2i rhs);
IMDEF void v2i_add_eq(v2i *lhs, v2i rhs);
IMDEF v2i  v2i_sub(v2i lhs, v2i rhs);
IMDEF void v2i_sub_eq(v2i *lhs, v2i rhs);
IMDEF v2i  v2i_div(v2i lhs, v2i rhs);
IMDEF void v2i_div_eq(v2i *lhs, v2i rhs);
IMDEF v2i  v2i_inverse(v2i v);
IMDEF void v2i_inverse_eq(v2i *v);
IMDEF b32  v2i_equal(v2i lhs, v2i rhs);

/* 2D Anti-aliased bounding box */

typedef struct box2i
{
	v2i min;
	v2i max;
} box2i;

IMDEF void box2i_from_dims(box2i *b, s32 left, s32 top, s32 right, s32 bottom);
IMDEF void box2i_from_xywh(box2i *b, s32 x, s32 y, s32 w, s32 h);
IMDEF b32  box2i_contains_point(box2i b, v2i p);
IMDEF void box2i_clamp_point(box2i b, v2i *p);
IMDEF b32  box2i_eq(box2i lhs, box2i rhs);
IMDEF v2i  box2i_get_center(box2i b);
IMDEF v2i  box2i_get_half_dim(box2i b);

/* Polygon */

IMDEF v2i polyi_center(const v2i *v, u32 n);

#endif // IMATH_H


/* Implementation */


#ifdef IMATH_IMPLEMENTATION

/* 2D Vector */

IMGDEF const v2i g_v2i_zero   = { 0, 0 };
IMGDEF const v2i g_v2i_x_axis = { 1, 0 };
IMGDEF const v2i g_v2i_y_axis = { 0, 1 };

IMDEF void v2i_set(v2i *v, s32 x, s32 y)
{
	v->x = x;
	v->y = y;
}

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

IMDEF v2i v2i_scale(v2i v, s32 s)
{
	return (v2i){ v.x * s, v.y * s };
}

IMDEF void v2i_scale_eq(v2i *v, s32 s)
{
	*v = v2i_scale(*v, s);
}

IMDEF v2i v2i_add(v2i lhs, v2i rhs)
{
	return (v2i){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y };
}

IMDEF void v2i_add_eq(v2i *lhs, v2i rhs)
{
	*lhs = v2i_add(*lhs, rhs);
}

IMDEF v2i v2i_sub(v2i lhs, v2i rhs)
{
	return (v2i){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
}

IMDEF void v2i_sub_eq(v2i *lhs, v2i rhs)
{
	*lhs = v2i_sub(*lhs, rhs);
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

IMDEF v2i v2i_inverse(v2i v)
{
	return (v2i){ .x = -v.x, .y = -v.y };
}

IMDEF void v2i_inverse_eq(v2i *v)
{
	v->x = -v->x;
	v->y = -v->y;
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

IMDEF void box2i_from_xywh(box2i *box, s32 x, s32 y, s32 w, s32 h)
{
	box->min.x = x;
	box->min.y = y;
	box->max.x = x + w;
	box->max.y = y + h;
}

IMDEF b32 box2i_contains_point(box2i box, v2i point)
{
	return    point.x >= box.min.x
	       && point.x <= box.max.x
	       && point.y >= box.min.y
	       && point.y <= box.max.y;
}

IMDEF void box2i_clamp_point(box2i b, v2i *p)
{
	p->x = clamp(b.min.x, p->x, b.max.x);
	p->y = clamp(b.min.y, p->y, b.max.y);
}

IMDEF b32 box2i_eq(box2i lhs, box2i rhs)
{
	return    lhs.min.x == rhs.min.x
	       && lhs.min.y == rhs.min.y
	       && lhs.max.x == rhs.max.x
	       && lhs.max.y == rhs.max.y;
}

IMDEF v2i box2i_get_center(box2i box)
{
	return v2i_add(box.min, box2i_get_half_dim(box));
}

IMDEF v2i box2i_get_half_dim(box2i box)
{
	return v2i_div(v2i_sub(box.max, box.min), (v2i){2,2});
}

/* Polygon */

IMDEF v2i polyi_center(const v2i *v, u32 n)
{
	v2i center = { .x=0, .y=0 };
	for (const v2i *vn = v+n; v != vn; ++v)
		v2i_add_eq(&center, *v);
	return v2i_div(center, (v2i){n,n});
}

#undef IMATH_IMPLEMENTATION
#endif // IMATH_IMPLEMENTATION
