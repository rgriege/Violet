#ifndef VIOLET_IMATH_H
#define VIOLET_IMATH_H

#define IMDEF
#define IMGDECL extern
#define IMGDEF

/* 2D Vector */

typedef union v2i
{
	struct { s32 x, y; };
	struct { s32 d[2]; };
} v2i;

IMGDECL const v2i g_v2i_zero;
IMGDECL const v2i g_v2i_x_axis;
IMGDECL const v2i g_v2i_y_axis;

IMDEF void v2i_set(v2i *v, s32 x, s32 y);
IMDEF s32  v2i_mag_sq(v2i v);
IMDEF s32  v2i_dist_sq(v2i lhs, v2i rhs);
IMDEF v2i  v2i_scale(v2i v, s32 s);
IMDEF void v2i_scale_eq(v2i *v, s32 s);
IMDEF v2i  v2i_scale_inv(v2i v, s32 s);
IMDEF void v2i_scale_inv_eq(v2i *v, s32 s);
IMDEF v2i  v2i_add(v2i lhs, v2i rhs);
IMDEF void v2i_add_eq(v2i *lhs, v2i rhs);
IMDEF v2i  v2i_sub(v2i lhs, v2i rhs);
IMDEF void v2i_sub_eq(v2i *lhs, v2i rhs);
IMDEF v2i  v2i_mul(v2i lhs, v2i rhs);
IMDEF void v2i_mul_eq(v2i *lhs, v2i rhs);
IMDEF v2i  v2i_div(v2i lhs, v2i rhs);
IMDEF void v2i_div_eq(v2i *lhs, v2i rhs);
IMDEF s32  v2i_dot(v2i lhs, v2i rhs);
IMDEF s32  v2i_cross(v2i lhs, v2i rhs);
IMDEF v2i  v2i_lperp(v2i v);
IMDEF v2i  v2i_rperp(v2i v);
IMDEF v2i  v2i_inverse(v2i v);
IMDEF void v2i_inverse_eq(v2i *v);
IMDEF b32  v2i_equal(v2i lhs, v2i rhs);

/* Interval */

typedef struct ivali
{
	s32 l, r;
} ivali;

IMGDECL const ivali g_ivali_0_to_1;

IMDEF ivali ivali_range(s32 center, s32 radius);
IMDEF s32   ivali_center(ivali i);
IMDEF ivali ivali_slide(ivali i, s32 d);
IMDEF s32   ivali_length(ivali i);
IMDEF b32   ivali_overlaps(ivali lhs, ivali rhs);
IMDEF b32   ivali_overlaps_within(ivali lhs, ivali rhs, s32 error);

/* 2D Anti-aliased bounding box */

typedef struct box2i
{
	v2i min;
	v2i max;
} box2i;

IMDEF void  box2i_from_center(box2i *b, v2i center, v2i half_dim);
IMDEF void  box2i_from_dims(box2i *b, s32 left, s32 top, s32 right, s32 bottom);
IMDEF void  box2i_from_xywh(box2i *b, s32 x, s32 y, s32 w, s32 h);
IMDEF void  box2i_to_xywh(box2i box, s32 *x, s32 *y, s32 *w, s32 *h);
IMDEF box2i box2i_intersection(box2i lhs, box2i rhs);
IMDEF b32   box2i_contains_point(box2i b, v2i p);
IMDEF void  box2i_clamp_point(box2i b, v2i *p);
IMDEF b32   box2i_overlaps(box2i lhs, box2i rhs);
IMDEF b32   box2i_overlaps_within(box2i lhs, box2i rhs, s32 error);
IMDEF b32   box2i_eq(box2i lhs, box2i rhs);
IMDEF void  box2i_extend_point(box2i *box, v2i point);
IMDEF void  box2i_extend_box(box2i *b, box2i other);
IMDEF v2i   box2i_get_center(box2i b);
IMDEF v2i   box2i_get_extent(box2i b);
IMDEF v2i   box2i_get_half_dim(box2i b);
IMDEF s32   box2i_width(box2i b);
IMDEF s32   box2i_height(box2i b);

/* Polygon */

IMDEF void polyi_from_box(v2i *v, box2i box);
IMDEF s32  polyi_area(const v2i *v, u32 n);
IMDEF v2i  polyi_center(const v2i *v, u32 n);
IMDEF v2i  polyi_centroid(const v2i *v, u32 n);

#endif // VIOLET_IMATH_H


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

IMDEF v2i v2i_scale_inv(v2i v, s32 s)
{
	return (v2i){ v.x / s, v.y / s };
}

IMDEF void v2i_scale_inv_eq(v2i *v, s32 s)
{
	*v = v2i_scale_inv(*v, s);
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

IMDEF v2i v2i_mul(v2i lhs, v2i rhs)
{
	return (v2i){ .x = lhs.x * rhs.x, .y = lhs.y * rhs.y };
}

IMDEF void v2i_mul_eq(v2i *lhs, v2i rhs)
{
	*lhs = v2i_mul(*lhs, rhs);
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

IMDEF s32 v2i_dot(v2i lhs, v2i rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

IMDEF s32 v2i_cross(v2i lhs, v2i rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

IMDEF v2i v2i_lperp(v2i v)
{
	return (v2i){ .x = -v.y, .y = v.x };
}

IMDEF v2i v2i_rperp(v2i v)
{
	return (v2i){ .x = v.y, .y = -v.x };
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

/* Interval */

IMDEF ivali ivali_range(s32 center, s32 radius)
{
	return (ivali){
		.l = center - radius,
		.r = center + radius,
	};
}

IMDEF s32 ivali_center(ivali i)
{
	return (i.l + i.r) / 2;
}

IMDEF ivali ivali_slide(ivali i, s32 d)
{
	return (ivali){
		.l = i.l + d,
		.r = i.r + d,
	};
}

IMDEF s32 ivali_length(ivali i)
{
	return i.r - i.l;
}

IMDEF b32 ivali_overlaps(ivali lhs, ivali rhs)
{
	return lhs.l <= rhs.r && rhs.l <= lhs.r;
}

IMDEF b32 ivali_overlaps_within(ivali lhs, ivali rhs, s32 error)
{
	return rhs.r - lhs.l > -error && lhs.r - rhs.l > -error;
}

/* 2D Anti-aliased bounding box */

IMDEF void box2i_from_center(box2i *box, v2i center, v2i half_dim)
{
	box->min.x = center.x - half_dim.x;
	box->min.y = center.y - half_dim.y;
	box->max.x = center.x + half_dim.x;
	box->max.y = center.y + half_dim.y;
}

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

IMDEF void box2i_to_xywh(box2i box, s32 *x, s32 *y, s32 *w, s32 *h)
{
	*x = box.min.x;
	*y = box.min.y;
	*w = box.max.x - box.min.x;
	*h = box.max.y - box.min.y;
}

IMDEF box2i box2i_intersection(box2i lhs, box2i rhs)
{
	return (box2i) {
		.min = {
			.x = clamp(lhs.min.x, rhs.min.x, lhs.max.x),
			.y = clamp(lhs.min.y, rhs.min.y, lhs.max.y),
		},
		.max = {
			.x = clamp(lhs.min.x, rhs.max.x, lhs.max.x),
			.y = clamp(lhs.min.y, rhs.max.y, lhs.max.y),
		},
	};
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

IMDEF b32 box2i_overlaps(box2i lhs, box2i rhs)
{
	ivali lhs_x = { lhs.min.x, lhs.max.x };
	ivali rhs_x = { rhs.min.x, rhs.max.x };
	ivali lhs_y = { lhs.min.y, lhs.max.y };
	ivali rhs_y = { rhs.min.y, rhs.max.y };
	return    ivali_overlaps(lhs_x, rhs_x)
	       && ivali_overlaps(lhs_y, rhs_y);
}

IMDEF b32 box2i_overlaps_within(box2i lhs, box2i rhs, s32 error)
{
	ivali lhs_x = { lhs.min.x, lhs.max.x };
	ivali rhs_x = { rhs.min.x, rhs.max.x };
	ivali lhs_y = { lhs.min.y, lhs.max.y };
	ivali rhs_y = { rhs.min.y, rhs.max.y };
	return    ivali_overlaps_within(lhs_x, rhs_x, error)
	       && ivali_overlaps_within(lhs_y, rhs_y, error);
}

IMDEF b32 box2i_eq(box2i lhs, box2i rhs)
{
	return    lhs.min.x == rhs.min.x
	       && lhs.min.y == rhs.min.y
	       && lhs.max.x == rhs.max.x
	       && lhs.max.y == rhs.max.y;
}

IMDEF void box2i_extend_point(box2i *box, v2i point)
{
	const s32 left   = min(box->min.x, point.x);
	const s32 top    = max(box->max.y, point.y);
	const s32 right  = max(box->max.x, point.x);
	const s32 bottom = min(box->min.y, point.y);
	box2i_from_dims(box, left, top, right, bottom);
}

IMDEF void box2i_extend_box(box2i *b, box2i other)
{
	box2i_extend_point(b, other.min);
	box2i_extend_point(b, other.max);
}

IMDEF v2i box2i_get_center(box2i box)
{
	return v2i_add(box.min, box2i_get_half_dim(box));
}

IMDEF v2i box2i_get_extent(box2i b)
{
	return v2i_sub(b.max, b.min);
}

IMDEF v2i box2i_get_half_dim(box2i box)
{
	return v2i_div(v2i_sub(box.max, box.min), (v2i){2,2});
}

IMDEF s32 box2i_width(box2i box)
{
	return box.max.x - box.min.x;
}

IMDEF s32 box2i_height(box2i box)
{
	return box.max.y - box.min.y;
}

/* Polygon */

IMDEF void polyi_from_box(v2i *v, box2i box)
{
	const v2i top_left = { box.min.x, box.max.y };
	const v2i bottom_right = { box.max.x, box.min.y };

	v[0] = bottom_right;
	v[1] = box.max;
	v[2] = top_left;
	v[3] = box.min;
}

/* NOTE(rgriege): Green's theorem */
static
s32 polyi__area(const v2i *v, u32 n)
{
	s32 area = 0;
	v2i prev = v[n-1];
	for (const v2i *vn = v+n; v != vn; ++v) {
		area += v2i_cross(prev, *v);
		prev = *v;
	}
	return area/2;
}

IMDEF s32 polyi_area(const v2i *v, u32 n)
{
	return abs(polyi__area(v, n));
}

IMDEF v2i polyi_center(const v2i *v, u32 n)
{
	v2i center = { .x=0, .y=0 };
	for (const v2i *vn = v+n; v != vn; ++v)
		v2i_add_eq(&center, *v);
	return v2i_div(center, (v2i){n,n});
}

IMDEF v2i polyi_centroid(const v2i *v, u32 n)
{
	const s32 denom = 6 * polyi__area(v, n);
	v2i centroid = { .x=0, .y=0 };
	v2i prev = v[n-1];
	for (const v2i *vn = v+n; v != vn; ++v) {
		const s32 cross = v2i_cross(prev, *v);
		v2i_add_eq(&centroid, v2i_scale(v2i_add(prev, *v), cross));
		prev = *v;
	}
	return v2i_div(centroid, (v2i){ denom, denom });
}

#undef IMATH_IMPLEMENTATION
#endif // IMATH_IMPLEMENTATION
