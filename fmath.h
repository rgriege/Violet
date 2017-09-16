#ifndef FMATH_H
#define FMATH_H

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef FMATH_NO_SSE
#ifndef _WIN32
#include <x86intrin.h>
#else
#include <intrin.h>
#endif
#endif

#ifdef FMATH_STANDALONE
typedef unsigned b32;
typedef unsigned u32;
typedef float r32;
#define false 0
#define true 1
#endif

#ifndef FMATH_NO_INLINE
#define FMDEF static inline
#define FMGDECL static
#define FMGDEF static
#undef FMATH_STATIC
#define FMATH_STATIC
#elif FMATH_STATIC
#define FMDEF static
#define FMGDECL static
#define FMGDEF static
#else
#define FMDEF
#define FMGDECL extern
#define FMGDEF
#endif

#if defined FMATH_STATIC && !defined FMATH_IMPLEMENTATION
#define FMATH_IMPLEMENTATION
#endif

/* General utilities */

#define fmaxf(x, y) (((x) < (y)) ? (y) : (x))
#define fminf(x, y) (((x) > (y)) ? (y) : (x))

#define fPI 3.14159265359f
#define fDEG2RAD (fPI / 180.f)

#define fmath_clamp(lo, val, hi) fmaxf(lo, fminf(hi, val))
#define fmath_deg2rad(deg) ((deg) * fDEG2RAD)
#define fmath_rad2deg(rad) ((rad) / fDEG2RAD)
#define fmath_eq(a, b, err) (fabs((a)-(b)) <= (err))

/* 2D Vector */

typedef struct v2f
{
	r32 x, y;
} v2f;

FMGDECL const v2f g_v2f_x_axis;
FMGDECL const v2f g_v2f_y_axis;
FMGDECL const v2f g_v2f_zero;

FMDEF void v2f_set(v2f *v, r32 x, r32 y);
FMDEF r32  v2f_mag(v2f v);
FMDEF r32  v2f_mag_sq(v2f v);
FMDEF r32  v2f_dist(v2f lhs, v2f rhs);
FMDEF r32  v2f_dist_sq(v2f lhs, v2f rhs);
FMDEF v2f  v2f_normalize(v2f v);
FMDEF void v2f_normalize_eq(v2f *v);
FMDEF b32  v2f_is_unit(v2f v);
FMDEF v2f  v2f_scale(v2f v, r32 s);
FMDEF void v2f_scale_eq(v2f *v, r32 s);
FMDEF v2f  v2f_add(v2f lhs, v2f rhs);
FMDEF void v2f_add_eq(v2f *lhs, v2f rhs);
FMDEF v2f  v2f_sub(v2f lhs, v2f rhs);
FMDEF void v2f_sub_eq(v2f *lhs, v2f rhs);
FMDEF v2f  v2f_mul(v2f lhs, v2f rhs);
FMDEF void v2f_mul_eq(v2f *lhs, v2f rhs);
FMDEF v2f  v2f_div(v2f lhs, v2f rhs);
FMDEF void v2f_div_eq(v2f *lhs, v2f rhs);
FMDEF v2f  v2f_dir(v2f src, v2f dst);
FMDEF v2f  v2f_rot(v2f v, r32 radians);
FMDEF r32  v2f_dot(v2f lhs, v2f rhs);
FMDEF r32  v2f_cross(v2f lhs, v2f rhs);
FMDEF v2f  v2f_proj(v2f v, v2f axis);
FMDEF v2f  v2f_reflect(v2f v, v2f axis);
FMDEF v2f  v2f_lperp(v2f v);
FMDEF v2f  v2f_rperp(v2f v);
FMDEF v2f  v2f_inverse(v2f v);
FMDEF void v2f_inverse_eq(v2f *v);
FMDEF v2f  v2f_round(v2f v, r32 unit);
FMDEF void v2f_round_eq(v2f *v, r32 unit);
FMDEF b32  v2f_is_zero(v2f v);
FMDEF b32  v2f_equal(v2f lhs, v2f rhs);
FMDEF b32  v2f_share_quadrant(v2f lhs, v2f rhs);

/* 3D Vector */

typedef struct v3f
{
	r32 x, y, z;
} v3f;

FMGDECL const v3f g_v3f_x_axis;
FMGDECL const v3f g_v3f_y_axis;
FMGDECL const v3f g_v3f_z_axis;
FMGDECL const v3f g_v3f_zero;

FMDEF r32 v3f_mag(v3f v);
FMDEF r32 v3f_mag_sq(v3f v);
FMDEF v3f v3f_normalize(v3f v);
FMDEF b32 v3f_is_unit(v3f v);
FMDEF v3f v3f_scale(v3f v, r32 s);
FMDEF v3f v3f_add(v3f lhs, v3f rhs);
FMDEF v3f v3f_sub(v3f lhs, v3f rhs);
FMDEF r32 v3f_dot(v3f lhs, v3f rhs);
FMDEF v3f v3f_cross(v3f lhs, v3f rhs);
FMDEF v3f v3f_proj(v3f v, v3f axis);
FMDEF v3f v3f_inverse(v3f v);
FMDEF b32 v3f_is_zero(v3f v);
FMDEF b32 v3f_equal(v3f lhs, v3f rhs);

/* 3x3 Matrix */

typedef r32 m3f[9];

FMGDECL const m3f g_m3f_identity;
FMGDECL const m3f g_m3f_zero;

FMDEF void m3f_init_rot(m3f m, r32 radians);
FMDEF void m3f_mul_m3(const m3f lhs, const m3f rhs, m3f res);
FMDEF v2f  m3f_mul_v2(const m3f m, v2f v);
FMDEF b32  m3f_equal(const m3f lhs, const m3f rhs);

/* 4x4 Matrix */

typedef struct m4f
{
#if FMATH_NO_SSE
	r32 v[16];
#elif !defined(_WIN32)
	_Alignas(16) r32 v[16];
#else
	__declspec(align(16)) r32 v[16];
#endif
} m4f;

FMGDECL const m4f g_m4f_identity;
FMGDECL const m4f g_m4f_zero;

FMDEF void m4f_mul_m4(const m4f lhs, const m4f rhs, m4f res);
FMDEF v3f  m4f_mul_v3(const m4f lhs, v3f rhs);
FMDEF b32  m4f_equal(const m4f lhs, const m4f rhs);
FMDEF void m4f_from_m3(m4f dst, const m3f src);
FMDEF void m4f_to_m3(const m4f src, m3f dst);


/* Interval */

typedef struct ivalf
{
	r32 l, r;
} ivalf;

FMGDECL const ivalf g_ivalf_0_to_1;

FMDEF ivalf ivalf_range(r32 center, r32 radius);
FMDEF r32   ivalf_center(ivalf i);
FMDEF void  ivalf_slide(ivalf *i, r32 d);
FMDEF r32   ivalf_length(ivalf i);
FMDEF b32   ivalf_contains_val(ivalf i, r32 x);
FMDEF b32   ivalf_contains_ival(ivalf lhs, ivalf rhs);
FMDEF b32   ivalf_overlaps(ivalf lhs, ivalf rhs);
FMDEF b32   ivalf_overlaps_within(ivalf lhs, ivalf rhs, r32 error);
FMDEF r32   ivalf_overlap(ivalf lhs, ivalf rhs);
FMDEF ivalf ivalf_overlap_ival(ivalf lhs, ivalf rhs);

/* 2D Anti-aliased bounding box */

typedef struct box2f
{
	v2f min;
	v2f max;
} box2f;

FMDEF void box2f_from_point(box2f *b, v2f p);
FMDEF void box2f_from_line(box2f *b, v2f start, v2f end);
FMDEF void box2f_from_center(box2f *b, v2f center, v2f half_dim);
FMDEF void box2f_from_dims(box2f *b, r32 left, r32 top, r32 right, r32 bottom);
FMDEF b32  box2f_empty(box2f b);
FMDEF b32  box2f_contains_point(box2f b, v2f p);
FMDEF b32  box2f_contains_box(box2f lhs, box2f rhs);
FMDEF void box2f_clamp_point(box2f b, v2f *p);
FMDEF b32  box2f_overlaps(box2f lhs, box2f rhs);
FMDEF b32  box2f_eq(box2f lhs, box2f rhs);
FMDEF void box2f_extend_point(box2f *b, v2f p);
FMDEF void box2f_extend_box(box2f *b, box2f other);
FMDEF void box2f_translate(box2f *b, v2f v);
FMDEF void box2f_transform(box2f *b, const m3f mat);
FMDEF v2f  box2f_get_center(box2f b);
FMDEF v2f  box2f_get_half_dim(box2f b);

/* Line/Segment utilities */

FMDEF v2f fmath_line_extrapolate(v2f a, v2f b, r32 t);
FMDEF r32 fmath_line_interpolate(v2f a, v2f b, v2f p);
FMDEF b32 fmath_ray_intersect_coords(v2f a, v2f adir, v2f b, v2f bdir,
                                     r32 *t, r32 *u);
FMDEF b32 fmath_line_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1,
                                      r32 *t, r32 *u);
FMDEF b32 fmath_line_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec);
FMDEF b32 fmath_segment_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1,
                                         r32 *t, r32 *u);
FMDEF b32 fmath_segment_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec);
FMDEF v2f fmath_nearest_point_on_segment(v2f a, v2f b, v2f p);
FMDEF v2f fmath_nearest_point_on_line(v2f a, v2f b, v2f p);
FMDEF r32 fmath_point_to_segment_dist(v2f a, v2f b, v2f p);
FMDEF r32 fmath_point_to_segment_dist_sq(v2f a, v2f b, v2f p);
FMDEF r32 fmath_point_to_line_dist(v2f a, v2f b, v2f p);
FMDEF r32 fmath_point_to_line_dist_sq(v2f a, v2f b, v2f p);

FMDEF ivalf linef_project(v2f a, v2f b, v2f axis);

/* Polygon */

FMDEF void  polyf_from_box(v2f *v, box2f box);
FMDEF b32   polyf_is_simple(const v2f *v, u32 n);
FMDEF b32   polyf_is_convex(const v2f *v, u32 n);
FMDEF b32   polyf_contains(const v2f *v, u32 n, v2f point);
FMDEF void  polyf_bounding_box(const v2f *v, u32 n, box2f *box);
FMDEF void  polyf_translate(v2f *v, u32 n, v2f delta);
FMDEF ivalf polyf_project(const v2f *v, u32 n, v2f axis);
FMDEF v2f   polyf_center(const v2f *v, u32 n);
FMDEF r32   polyf_area(const v2f *v, u32 n);
FMDEF v2f   polyf_centroid(const v2f *v, u32 n);
FMDEF b32   polyf_is_cc(const v2f *v, u32 n);
FMDEF b32   polyf_segment_intersect(const v2f *v, u32 n, v2f v0, v2f v1);
FMDEF b32   polyf_intersect(const v2f *p1, u32 n1, const v2f *p2, u32 n2,
                            v2f *isec);
FMDEF r32   polyf_pt_dist(const v2f *v, u32 n, v2f p);
FMDEF r32   polyf_pt_dist_sq(const v2f *v, u32 n, v2f p);

#endif // FMATH_H


/* Implementation */


#ifdef FMATH_IMPLEMENTATION

/* 2D Vector */

FMGDEF const v2f g_v2f_x_axis = { 1, 0 };
FMGDEF const v2f g_v2f_y_axis = { 0, 1 };
FMGDEF const v2f g_v2f_zero   = { 0, 0 };

FMDEF void v2f_set(v2f *v, r32 x, r32 y)
{
	v->x = x;
	v->y = y;
}

FMDEF r32 v2f_mag(v2f v)
{
	return sqrtf(v2f_mag_sq(v));
}

FMDEF r32 v2f_mag_sq(v2f v)
{
	return v2f_dot(v, v);
}

FMDEF r32 v2f_dist(v2f lhs, v2f rhs)
{
	return sqrtf(v2f_dist_sq(lhs, rhs));
}

FMDEF r32 v2f_dist_sq(v2f lhs, v2f rhs)
{
	v2f_sub_eq(&lhs, rhs);
	return v2f_mag_sq(lhs);
}

FMDEF v2f v2f_normalize(v2f v)
{
	return v2f_scale(v, 1.f/v2f_mag(v));
}

FMDEF void v2f_normalize_eq(v2f *v)
{
	*v = v2f_normalize(*v);
}

FMDEF b32 v2f_is_unit(v2f v)
{
	return v2f_mag_sq(v) == 1.f;
}

FMDEF v2f v2f_scale(v2f v, r32 s)
{
	v2f result = { v.x * s, v.y * s };
	return result;
}

FMDEF void v2f_scale_eq(v2f *v, r32 s)
{
	v->x *= s;
	v->y *= s;
}

FMDEF v2f v2f_add(v2f lhs, v2f rhs)
{
	v2f result = { lhs.x + rhs.x, lhs.y + rhs.y };
	return result;
}

FMDEF void v2f_add_eq(v2f *lhs, v2f rhs)
{
	lhs->x += rhs.x;
	lhs->y += rhs.y;
}

FMDEF v2f v2f_sub(v2f lhs, v2f rhs)
{
	return (v2f){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
}

FMDEF void v2f_sub_eq(v2f *lhs, v2f rhs)
{
	lhs->x -= rhs.x;
	lhs->y -= rhs.y;
}

FMDEF v2f  v2f_mul(v2f lhs, v2f rhs)
{
	return (v2f){ .x = lhs.x * rhs.x, .y = lhs.y * rhs.y };
}

FMDEF void v2f_mul_eq(v2f *lhs, v2f rhs)
{
	lhs->x *= rhs.x;
	lhs->y *= rhs.y;
}

FMDEF v2f v2f_div(v2f lhs, v2f rhs)
{
	return (v2f){ .x = lhs.x / rhs.x, .y = lhs.y / rhs.y };
}

FMDEF void v2f_div_eq(v2f *lhs, v2f rhs)
{
	lhs->x /= rhs.x;
	lhs->y /= rhs.y;
}

FMDEF v2f v2f_dir(v2f src, v2f dst)
{
	return v2f_normalize(v2f_sub(dst, src));
}

FMDEF v2f v2f_rot(v2f v, r32 radians)
{
	const r32 c = cosf(radians);
	const r32 s = sinf(radians);
	return (v2f){ .x = v.x * c - v.y * s, .y = v.x * s + v.y * c };
}

FMDEF r32 v2f_dot(v2f lhs, v2f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

FMDEF r32 v2f_cross(v2f lhs, v2f rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

FMDEF v2f v2f_proj(v2f v, v2f axis)
{
	return v2f_scale(axis, v2f_dot(v, axis) / v2f_mag_sq(axis));
}

FMDEF v2f v2f_reflect(v2f v, v2f axis)
{
	v2f proj = v2f_proj(v, axis);
	v2f perp = v2f_sub(v, proj);
	return v2f_sub(proj, perp);
}

FMDEF v2f v2f_lperp(v2f v)
{
	return (v2f){ .x = -v.y, .y = v.x };
}

FMDEF v2f v2f_rperp(v2f v)
{
	return (v2f){ .x = v.y, .y = -v.x };
}

FMDEF v2f v2f_inverse(v2f v)
{
	return (v2f){ .x = -v.x, .y = -v.y };
}

FMDEF void v2f_inverse_eq(v2f *v)
{
	v->x = -v->x;
	v->y = -v->y;
}

FMDEF v2f v2f_round(v2f v, r32 unit)
{
	v2f_round_eq(&v, unit);
	return v;
}

FMDEF void v2f_round_eq(v2f *v, r32 unit)
{
	const r32 inv_unit = 1.f / unit;
	v->x = roundf(v->x * inv_unit) / inv_unit;
	v->y = roundf(v->y * inv_unit) / inv_unit;
}

FMDEF b32 v2f_is_zero(v2f v)
{
	return v.x == 0 && v.y == 0;
}

FMDEF b32 v2f_equal(v2f lhs, v2f rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

FMDEF b32 v2f_share_quadrant(v2f lhs, v2f rhs)
{
	return v2f_dot(lhs, rhs) >= 0;
}


/* 3D Vector */

FMGDEF const v3f g_v3f_x_axis = { 1, 0, 0 };
FMGDEF const v3f g_v3f_y_axis = { 0, 1, 0 };
FMGDEF const v3f g_v3f_z_axis = { 0, 0, 1 };
FMGDEF const v3f g_v3f_zero   = { 0, 0, 0 };

FMDEF r32 v3f_mag(v3f v)
{
	return sqrtf(v3f_mag_sq(v));
}

FMDEF r32 v3f_mag_sq(v3f v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

FMDEF v3f v3f_normalize(v3f v)
{
	return v3f_scale(v, 1.f / v3f_mag_sq(v));
}

FMDEF b32 v3f_is_unit(v3f v)
{
	return v3f_mag_sq(v) == 1;
}

FMDEF v3f v3f_scale(v3f v, r32 s)
{
	return (v3f){ .x = v.x * s, .y = v.y * s, .z = v.z * s };
}

FMDEF v3f v3f_add(v3f lhs, v3f rhs)
{
	return (v3f){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y, .z = lhs.z + rhs.z };
}

FMDEF v3f v3f_sub(v3f lhs, v3f rhs)
{
	return (v3f){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y, .z = lhs.z - rhs.z };
}

FMDEF r32 v3f_dot(v3f lhs, v3f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

FMDEF v3f v3f_cross(v3f lhs, v3f rhs)
{
	return (v3f){
		.x = lhs.y * rhs.z - lhs.z * rhs.y,
		.y = lhs.z * rhs.x - lhs.x * rhs.z,
		.z = lhs.x * rhs.y - lhs.y * rhs.x
	};
}

FMDEF v3f v3f_proj(v3f v, v3f axis)
{
	return v3f_scale(axis, v3f_dot(v, axis) / v3f_mag_sq(axis));
}

FMDEF v3f v3f_inverse(v3f v)
{
	return (v3f){ .x = -v.x, .y = -v.y, .z = -v.z };
}

FMDEF b32 v3f_is_zero(v3f v)
{
	return v3f_equal(v, g_v3f_zero);
}

FMDEF b32 v3f_equal(v3f lhs, v3f rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}


/* 3x3 Matrix */

FMGDEF const m3f g_m3f_identity = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

FMGDEF const m3f g_m3f_zero = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
};

FMDEF void m3f_init_rot(m3f m, r32 radians)
{
	m[0] = cosf(radians);
	m[1] = -sinf(radians);
	m[2] = 0;
	m[3] = -m[1];
	m[4] = m[0];
	m[5] = 0;
	m[6] = 0;
	m[7] = 0;
	m[8] = 1;
}

FMDEF void m3f_mul_m3(const m3f lhs, const m3f rhs, m3f res)
{
	const r32 *lhs_i, *rhs_k;
	r32 *res_i, lhs_ik;
	memset(res, 0, 9 * sizeof(r32));
	for (int i = 0; i < 3; i++) {
		lhs_i = lhs + 3 * i;
		res_i = res + 3 * i;
		for (int k = 0; k < 3; k++) {
			rhs_k = rhs + 3 * k;
			lhs_ik = lhs_i[k];
			for (int j = 0; j < 3; j++)
				res_i[j] += lhs_ik * rhs_k[j];
		}
	}
}

FMDEF v2f m3f_mul_v2(const m3f m, v2f v)
{
	v2f result;
	result.x = m[0] * v.x + m[1] * v.y + m[2];
	result.y = m[3] * v.x + m[4] * v.y + m[5];
	return result;
}

FMDEF b32 m3f_equal(const m3f lhs, const m3f rhs)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (lhs[3*i+j] != rhs[3*i+j])
				return false;
	return true;
}

/* 4x4 Matrix */

FMGDEF const m4f g_m4f_identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

FMGDEF const m4f g_m4f_zero = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

FMDEF void m4f_mul_m4(const m4f lhs, const m4f rhs, m4f res)
{
	const r32 *lhs_i, *rhs_k;
	r32 *res_i, lhs_ik;
	memset(res.v, 0, 16 * sizeof(r32));
	for (int i = 0; i < 4; ++i) {
		lhs_i = lhs.v + 4 * i;
		res_i = res.v + 4 * i;
		for (int k = 0; k < 4; ++k) {
			rhs_k = rhs.v + 4 * k;
			lhs_ik = lhs_i[k];
#ifndef FMATH_NO_SSE
			__m128 r_vec = _mm_set_ps1(lhs_ik);
			__m128 rhs_vec = _mm_load_ps(rhs_k);
			__m128 res_vec = _mm_load_ps(res_i);
			__m128 mul = _mm_mul_ps(r_vec, rhs_vec);
			__m128 add = _mm_add_ps(res_vec, mul);
			_mm_store_ps(res_i, add);
#else
			for (int j = 0; j < 4; j++)
				res_i[j] += lhs_ik * rhs_k[j];
#endif
		}
	}
}

FMDEF v3f m4f_mul_v3(const m4f lhs, v3f rhs)
{
	v3f result;
	result.x = lhs.v[0] * rhs.x + lhs.v[1] * rhs.y + lhs.v[2] * rhs.z + lhs.v[3];
	result.y = lhs.v[4] * rhs.x + lhs.v[5] * rhs.y + lhs.v[6] * rhs.z + lhs.v[7];
	result.z = lhs.v[8] * rhs.x + lhs.v[9] * rhs.y + lhs.v[10] * rhs.z + lhs.v[11];
	return result;
}

FMDEF b32 m4f_equal(const m4f lhs, const m4f rhs)
{
	return memcmp(lhs.v, rhs.v, 16 * sizeof(r32)) == 0;
}

FMDEF void m4f_from_m3(m4f dst, const m3f src)
{
	dst.v[0] = src[0];
	dst.v[1] = src[1];
	dst.v[2] = 0;
	dst.v[3] = src[2];
	dst.v[4] = src[3];
	dst.v[5] = src[4];
	dst.v[6] = 0;
	dst.v[7] = src[5];
	dst.v[8] = 0;
	dst.v[9] = 0;
	dst.v[10] = 1;
	dst.v[11] = 0;
	dst.v[12] = src[6];
	dst.v[13] = src[7];
	dst.v[14] = 0;
	dst.v[15] = src[8];
}

FMDEF void m4f_to_m3(const m4f src, m3f dst)
{
	memcpy(dst, src.v, 3 * sizeof(r32));
	memcpy(&dst[3], &src.v[4], 3 * sizeof(r32));
	memcpy(&dst[6], &src.v[8], 3 * sizeof(r32));
}

/* Interval */

FMGDEF const ivalf g_ivalf_0_to_1 = { .l = 0.f, .r = 1.f };

FMDEF void ivalf_slide(ivalf *i, r32 d)
{
	i->l += d;
	i->r += d;
}

FMDEF ivalf ivalf_range(r32 center, r32 radius)
{
	return (ivalf){
		.l = center - radius,
		.r = center + radius,
	};
}

FMDEF r32 ivalf_center(ivalf i)
{
	return (i.l + i.r) / 2.f;
}

FMDEF r32 ivalf_length(ivalf i)
{
	return i.r - i.l;
}

FMDEF b32 ivalf_contains_val(ivalf i, r32 x)
{
	return x >= i.l && x <= i.r;
}

FMDEF b32 ivalf_contains_ival(ivalf lhs, ivalf rhs)
{
	return rhs.l >= lhs.l && rhs.r <= lhs.r;
}

FMDEF b32 ivalf_overlaps(ivalf lhs, ivalf rhs)
{
	return lhs.l <= rhs.r && rhs.l <= lhs.r;
}

FMDEF b32 ivalf_overlaps_within(ivalf lhs, ivalf rhs, r32 error)
{
	return rhs.r - lhs.l > -error && lhs.r - rhs.l > -error;
}

FMDEF r32 ivalf_overlap(ivalf lhs, ivalf rhs)
{
	const ivalf overlap = ivalf_overlap_ival(lhs, rhs);
	const r32 len = ivalf_length(overlap);
	return fmaxf(len, 0);
}

FMDEF ivalf ivalf_overlap_ival(ivalf lhs, ivalf rhs)
{
	return (ivalf){ .l = fmaxf(lhs.l, rhs.l), .r = fminf(lhs.r, rhs.r) };
}

/* 2D Anti-aliased bounding box */

FMDEF void box2f_from_point(box2f *b, v2f p)
{
	b->min = p;
	b->max = p;
}

FMDEF void box2f_from_line(box2f *box, v2f start, v2f end)
{
	r32 xmin, xmax, ymin, ymax;

	if (start.x < end.x)
	{
		xmin = start.x;
		xmax = end.x;
	}
	else
	{
		xmin = end.x;
		xmax = start.x;
	}

	if (start.y < end.y)
	{
		ymin = start.y;
		ymax = end.y;
	}
	else
	{
		ymin = end.y;
		ymax = start.y;
	}

	box2f_from_dims(box, xmin, ymax, xmax, ymin);
}

FMDEF void box2f_from_center(box2f *box, v2f center, v2f half_dim)
{
	box->min.x = center.x - half_dim.x;
	box->min.y = center.y - half_dim.y;
	box->max.x = center.x + half_dim.x;
	box->max.y = center.y + half_dim.y;
}

FMDEF void box2f_from_dims(box2f *box, r32 left, r32 top, r32 right, r32 bottom)
{
	box->min.x = left;
	box->min.y = bottom;
	box->max.x = right;
	box->max.y = top;
}

FMDEF b32 box2f_empty(box2f box)
{
	return v2f_equal(box.min, box.max);
}

FMDEF b32 box2f_contains_point(box2f box, v2f point)
{
	return    point.x >= box.min.x
	       && point.x <= box.max.x
	       && point.y >= box.min.y
	       && point.y <= box.max.y;
}

FMDEF b32 box2f_contains_box(box2f lhs, box2f rhs)
{
	ivalf lhs_x = { lhs.min.x, lhs.max.x };
	ivalf rhs_x = { rhs.min.x, rhs.max.x };
	ivalf lhs_y = { lhs.min.y, lhs.max.y };
	ivalf rhs_y = { rhs.min.y, rhs.max.y };
	return    ivalf_contains_ival(lhs_x, rhs_x)
	       && ivalf_contains_ival(lhs_y, rhs_y);
}

FMDEF void box2f_clamp_point(box2f b, v2f *p)
{
	p->x = fmath_clamp(b.min.x, p->x, b.max.x);
	p->y = fmath_clamp(b.min.y, p->y, b.max.y);
}

FMDEF b32 box2f_overlaps(box2f lhs, box2f rhs)
{
	ivalf lhs_x = { lhs.min.x, lhs.max.x };
	ivalf rhs_x = { rhs.min.x, rhs.max.x };
	ivalf lhs_y = { lhs.min.y, lhs.max.y };
	ivalf rhs_y = { rhs.min.y, rhs.max.y };
	return    ivalf_overlaps(lhs_x, rhs_x)
	       && ivalf_overlaps(lhs_y, rhs_y);
}

FMDEF b32 box2f_eq(box2f lhs, box2f rhs)
{
	return    lhs.min.x == rhs.min.x
	       && lhs.min.y == rhs.min.y
	       && lhs.max.x == rhs.max.x
	       && lhs.max.y == rhs.max.y;
}

FMDEF void box2f_extend_point(box2f *box, v2f point)
{
	box2f_from_dims(box, fminf(box->min.x, point.x), fmaxf(box->max.y, point.y),
	                fmaxf(box->max.x, point.x), fminf(box->min.y, point.y));
}

FMDEF void box2f_extend_box(box2f *lhs, box2f rhs)
{
	box2f_extend_point(lhs, rhs.min);
	box2f_extend_point(lhs, rhs.max);
}

FMDEF void box2f_translate(box2f *box, v2f off)
{
	v2f_add_eq(&box->min, off);
	v2f_add_eq(&box->max, off);
}

FMDEF void box2f_transform(box2f *box, const m3f mat)
{
	v2f poly[4];
	const v2f top_left = { box->min.x, box->max.y };
	const v2f bottom_right = { box->max.x, box->min.y };

	poly[0] = m3f_mul_v2(mat, top_left);
	poly[1] = m3f_mul_v2(mat, box->max);
	poly[2] = m3f_mul_v2(mat, bottom_right);
	poly[3] = m3f_mul_v2(mat, box->min);

	polyf_bounding_box(poly, 4, box);
}

FMDEF v2f box2f_get_center(box2f box)
{
	return v2f_add(box.min, box2f_get_half_dim(box));
}

FMDEF v2f box2f_get_half_dim(box2f box)
{
	return v2f_scale(v2f_sub(box.max, box.min), 0.5f);
}

/* Line/Segment utilities */

FMDEF v2f fmath_line_extrapolate(v2f a, v2f b, r32 t)
{
	return v2f_add(a, v2f_scale(v2f_sub(b, a), t));
}

static r32 fmath__line_interpolate(v2f a, v2f b, v2f p, v2f *ab)
{
	*ab = v2f_sub(b, a);
	return v2f_dot(v2f_sub(p, a), *ab) / v2f_mag_sq(*ab);
}

FMDEF r32 fmath_line_interpolate(v2f a, v2f b, v2f p)
{
	v2f ab;
	return fmath__line_interpolate(a, b, p, &ab);
}

FMDEF b32 fmath_ray_intersect_coords(v2f a, v2f adir, v2f b, v2f bdir, r32 *t, r32 *u)
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
	r32 rxs;

	ab = v2f_sub(b, a);
	rxs = v2f_cross(adir, bdir);

	if (rxs != 0)
	{
		*t = v2f_cross(ab, bdir) / rxs;
		*u = v2f_cross(ab, adir) / rxs;
		return true;
	}
	// parallel or collinear
	// treating collinear lines as not intersecting
	return false;
}

FMDEF b32 fmath_line_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1, r32 *t, r32 *u)
{
	v2f a, b;
	a = v2f_sub(a1, a0);
	b = v2f_sub(b1, b0);
	return fmath_ray_intersect_coords(a0, a, b0, b, t, u);
}

FMDEF b32 fmath_line_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec)
{
	r32 t = 0, u = 0;
	b32 result = fmath_line_intersect_coords(a0, a1, b0, b1, &t, &u);
	*isec = fmath_line_extrapolate(a0, a1, t);
	return result;
}

FMDEF b32 fmath_segment_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1, r32 *t, r32 *u)
{
	return    fmath_line_intersect_coords(a0, a1, b0, b1, t, u)
	       && 0 < *u && *u < 1 && 0 < *t && *t < 1;
}

FMDEF b32 fmath_segment_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec)
{
	r32 t = 0, u = 0;
	b32 result = fmath_segment_intersect_coords(a0, a1, b0, b1, &t, &u);
	*isec = fmath_line_extrapolate(a0, a1, t);
	return result;
}

FMDEF v2f fmath_nearest_point_on_segment(v2f a, v2f b, v2f p)
{
	v2f ab;
	r32 t = fmath_clamp(0, fmath__line_interpolate(a, b, p, &ab), 1);
	return v2f_add(a, v2f_scale(ab, t));
}

FMDEF v2f fmath_nearest_point_on_line(v2f a, v2f b, v2f p)
{
	v2f ab;
	r32 t = fmath__line_interpolate(a, b, p, &ab);
	return v2f_add(a, v2f_scale(ab, t));
}

FMDEF r32 fmath_point_to_segment_dist(v2f a, v2f b, v2f p)
{
	return sqrtf(fmath_point_to_segment_dist_sq(a, b, p));
}

FMDEF r32 fmath_point_to_segment_dist_sq(v2f a, v2f b, v2f p)
{
	return v2f_dist_sq(p, fmath_nearest_point_on_segment(a, b, p));
}

FMDEF r32 fmath_point_to_line_dist(v2f a, v2f b, v2f p)
{
	return sqrtf(fmath_point_to_line_dist_sq(a, b, p));
}

FMDEF r32 fmath_point_to_line_dist_sq(v2f a, v2f b, v2f p)
{
	return v2f_dist_sq(p, fmath_nearest_point_on_line(a, b, p));
}

FMDEF ivalf linef_project(v2f a, v2f b, v2f axis)
{
	ivalf projection;
	r32 dp_a, dp_b;

	if (!v2f_is_unit(axis))
		v2f_normalize_eq(&axis);

	dp_a = v2f_dot(a, axis);
	dp_b = v2f_dot(b, axis);

	projection.l = fminf(dp_a, dp_b);
	projection.r = fmaxf(dp_a, dp_b);
	return projection;
}

/* Polygon */

FMDEF void polyf_from_box(v2f *v, box2f box)
{
	const v2f top_left = { box.min.x, box.max.y };
	const v2f bottom_right = { box.max.x, box.min.y };

	*v++ = bottom_right;
	*v++ = box.max;
	*v++ = top_left;
	*v = box.min;
}

FMDEF b32 polyf_is_simple(const v2f *v, u32 n)
{
	v2f isec, a0, a1, b0, b1;
	u32 jend;

	assert(n >= 3);

	a0 = v[n-1];
	jend = n-1;
	for (u32 i = 0; i < n-2; ++i)
	{
		a1 = v[i];
		b0 = v[i+1];
		for (u32 j = i+2; j < jend; ++j)
		{
			b1 = v[j];
			if (fmath_segment_intersect(a0, a1, b0, b1, &isec))
				return false;
			b0 = b1;
		}
		a0 = a1;
		jend = n;
	}

	return true;
}

FMDEF b32 polyf_is_convex(const v2f *v, u32 n)
{
	assert(n >= 3);

	b32 cc_determined = false, cc;
	for (u32 i=0, last=n-1; i<=last; ++i)
	{
		v2f a = v[(i>0 ? i-1 : last)];
		v2f b = v[i];
		v2f c = v[(i<last ? i+1 : 0)];

		v2f ab, bc;
		ab = v2f_sub(b, a);
		bc = v2f_sub(c, b);
		const r32 cross = v2f_cross(ab, bc);
		if (cross != 0.f)
		{
			if (!cc_determined)
			{
				cc_determined = true;
				cc = cross > 0.f;
			}
			else if ((cross > 0.f) != cc)
			return false;
		}
	}
	return true;
}

FMDEF b32 polyf_contains(const v2f *v, u32 n, v2f point)
{
	box2f box;
	polyf_bounding_box(v, n, &box);
	if (!box2f_contains_point(box, point))
		return false;

	v2f out_p = v2f_add(v2f_sub(box.max, box.min), box.max);

	u32 intersections = 0;
	r32 t, u;
	for (u32 i = 0; i < n; ++i)
	{
		v2f a = v[i];
		v2f b = v[(i < n-1 ? i+1 : 0)];
		if (   fmath_line_intersect_coords(a, b, out_p, point, &t, &u)
		    && 0 < t && t <= 1 && 0 < u && u < 1)
			++intersections;
	}
	return intersections % 2 == 1;
}

FMDEF void polyf_bounding_box(const v2f *v, u32 n, box2f *box)
{
	box2f_from_point(box, *v++);
	for (const v2f *vn = v+n-1; v != vn; ++v)
		box2f_extend_point(box, *v);
}

FMDEF void polyf_translate(v2f *v, u32 n, v2f delta)
{
	for (const v2f *vn = v+n; v != vn; ++v)
		v2f_add_eq(v, delta);
}

FMDEF ivalf polyf_project(const v2f *v, u32 n, v2f axis)
{
	if (!v2f_is_unit(axis))
		v2f_normalize_eq(&axis);

	const r32 v0_proj = v2f_dot(*v, axis);
	ivalf projection = { .l = v0_proj, .r = v0_proj };

	for (const v2f *vn=v+n; v!=vn; ++v) {
		const r32 dp = v2f_dot(*v, axis);
		if (dp < projection.l)
			projection.l = dp;
		else if (dp > projection.r)
			projection.r = dp;
	}
	return projection;
}

FMDEF v2f polyf_center(const v2f *v, u32 n)
{
	v2f center = { .x=0, .y=0 };
	for (const v2f *vn = v+n; v != vn; ++v)
		v2f_add_eq(&center, *v);
	return v2f_scale(center, 1.f/n);
}

// NOTE(rgriege): Green's theorem
static r32 polyf__area(const v2f *v, u32 n)
{
	r32 area = 0;
	v2f prev = v[n-1];

	assert(n >= 3);

	for (const v2f *vn = v+n; v != vn; ++v)
	{
		area += v2f_cross(prev, *v);
		prev = *v;
	}
	return area/2;
}

FMDEF r32 polyf_area(const v2f *v, u32 n)
{
	return fabsf(polyf__area(v, n));
}

FMDEF v2f polyf_centroid(const v2f *v, u32 n)
{
	const r32 denom = 6 * polyf__area(v, n);
	v2f centroid = { .x=0, .y=0 };
	v2f prev = v[n-1];
	for (const v2f *vn = v+n; v != vn; ++v)
	{
		const r32 cross = v2f_cross(prev, *v);
		v2f_add_eq(&centroid, v2f_scale(v2f_add(prev, *v), cross));
		prev = *v;
	}
	return v2f_scale(centroid, 1.f/denom);
}

FMDEF b32 polyf_is_cc(const v2f *v, u32 n)
{
	assert(n>=3);

	r32 sine_sum = 0;
	for (u32 i=0, last=n-1; i<=last; ++i)
	{
		v2f a = v[(i>0 ? i-1 : last)];
		v2f b = v[i];
		v2f c = v[(i<last ? i+1 : 0)];

		v2f ab, bc;
		ab = v2f_sub(b, a);
		bc = v2f_sub(c, b);

		sine_sum += v2f_cross(ab, bc) / v2f_mag(ab) / v2f_mag(bc);
	}
	return sine_sum > 0;
}

FMDEF b32 polyf_segment_intersect(const v2f *v, u32 n, v2f v0, v2f v1)
{
	v2f isec, prev = v[n-1];
	for (const v2f *vn=v+n; v!=vn; ++v)
	{
		if (fmath_segment_intersect(prev, *v, v0, v1, &isec))
			return true;
		prev = *v;
	}
	return false;
}

FMDEF b32 polyf_intersect(const v2f *p1, u32 n1, const v2f *p2, u32 n2, v2f *isec)
{
	v2f p1a, p1b, p2a, p2b;
	for (u32 i=0; i<n1; ++i)
	{
		p1a = p1[i];
		p1b = p1[(i+1)%n1];
		for (u32 j=0; j<n2; ++j)
		{
			p2a = p2[j];
			p2b = p2[(j+1)%n2];
			if (fmath_segment_intersect(p1a, p1b, p2a, p2b, isec))
				return true;
		}
	}
	return false;
}

FMDEF r32 polyf_pt_dist(const v2f *v, u32 n, v2f p)
{
	return sqrtf(polyf_pt_dist_sq(v, n , p));
}

FMDEF r32 polyf_pt_dist_sq(const v2f *v, u32 n, v2f p)
{
	r32 min_dist = FLT_MAX;
	v2f prev = v[n-1];
	for (const v2f *vn=v+n; v!=vn; ++v)
	{
		const r32 d = fmath_point_to_segment_dist_sq(prev, *v, p);
		min_dist = fminf(min_dist, d);
		prev = *v;
	}
	return min_dist;
}

#undef FMATH_IMPLEMENTATION
#endif // FMATH_IMPLEMENTATION
