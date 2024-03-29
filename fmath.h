#ifndef VIOLET_FMATH_H
#define VIOLET_FMATH_H

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* General utilities */

#define fmaxf(x, y) (((x) < (y)) ? (y) : (x))
#define fminf(x, y) (((x) > (y)) ? (y) : (x))

#define fPI 3.14159265359f
#define f2PI (2.f * fPI)
#define fDEG2RAD (fPI / 180.f)

#define fmath_clamp(lo, val, hi) fmaxf(lo, fminf(hi, val))
#define fmath_deg2rad(deg) ((deg) * fDEG2RAD)
#define fmath_rad2deg(rad) ((rad) / fDEG2RAD)
#define fmath_eq(a, b, err) (fabs((a)-(b)) <= (err))
#define froundto(x, step) (roundf((x) / (step)) * (step))

/* 2D Vector */

typedef union v2f
{
	struct { r32 x, y; };
	struct { r32 u, v; };
	struct { r32 d[2]; };
} v2f;

extern const v2f g_v2f_x_axis;
extern const v2f g_v2f_y_axis;
extern const v2f g_v2f_zero;
extern const v2f g_v2f_one;
extern const v2f g_v2f_up;
extern const v2f g_v2f_down;
extern const v2f g_v2f_left;
extern const v2f g_v2f_right;

void v2f_set(v2f *v, r32 x, r32 y);
r32  v2f_mag(v2f v);
r32  v2f_mag_sq(v2f v);
r32  v2f_dist(v2f lhs, v2f rhs);
r32  v2f_dist_sq(v2f lhs, v2f rhs);
v2f  v2f_normalize(v2f v);
void v2f_normalize_eq(v2f *v);
v2f  v2f_scale(v2f v, r32 s);
void v2f_scale_eq(v2f *v, r32 s);
v2f  v2f_add(v2f lhs, v2f rhs);
void v2f_add_eq(v2f *lhs, v2f rhs);
v2f  v2f_sub(v2f lhs, v2f rhs);
void v2f_sub_eq(v2f *lhs, v2f rhs);
v2f  v2f_mul(v2f lhs, v2f rhs);
void v2f_mul_eq(v2f *lhs, v2f rhs);
v2f  v2f_div(v2f lhs, v2f rhs);
void v2f_div_eq(v2f *lhs, v2f rhs);
v2f  v2f_dir(v2f src, v2f dst);
v2f  v2f_fmadd(v2f v, v2f dir, r32 s);
v2f  v2f_fmsub(v2f v, v2f dir, r32 s);
v2f  v2f_rot(v2f v, r32 radians);
r32  v2f_dot(v2f lhs, v2f rhs);
r32  v2f_cross(v2f lhs, v2f rhs);
v2f  v2f_proj(v2f v, v2f axis);
v2f  v2f_reflect(v2f v, v2f axis);
v2f  v2f_lperp(v2f v);
v2f  v2f_rperp(v2f v);
v2f  v2f_perp_cw(v2f v);
v2f  v2f_perp_ccw(v2f v);
v2f  v2f_inverse(v2f v);
void v2f_inverse_eq(v2f *v);
v2f  v2f_round(v2f v, r32 unit);
void v2f_round_eq(v2f *v, r32 unit);
b32  v2f_is_zero(v2f v);
b32  v2f_equal(v2f lhs, v2f rhs);
b32  v2f_share_quadrant(v2f lhs, v2f rhs);
v2f  v2f_midpoint(v2f v0, v2f v1);
v2f  v2f_bisect(v2f v0, v2f v1);
v2f  v2f_abs(v2f v);

/* 3D Vector */

typedef union v3f
{
	struct { r32 x, y, z; };
	struct { v2f xy; };
	struct { r32 d[3]; };
} v3f;

extern const v3f g_v3f_x_axis;
extern const v3f g_v3f_y_axis;
extern const v3f g_v3f_z_axis;
extern const v3f g_v3f_zero;
extern const v3f g_v3f_one;

v3f v3f_create_xy_z(v2f xy, r32 z);
r32 v3f_mag(v3f v);
r32 v3f_mag_sq(v3f v);
r32 v3f_dist(v3f lhs, v3f rhs);
r32 v3f_dist_sq(v3f lhs, v3f rhs);
v3f v3f_normalize(v3f v);
v3f v3f_scale(v3f v, r32 s);
v3f v3f_add(v3f lhs, v3f rhs);
v3f v3f_sub(v3f lhs, v3f rhs);
v3f v3f_dir(v3f src, v3f dst);
v3f v3f_fmadd(v3f v, v3f dir, r32 s);
v3f v3f_fmsub(v3f v, v3f dir, r32 s);
r32 v3f_dot(v3f lhs, v3f rhs);
v3f v3f_cross(v3f lhs, v3f rhs);
v3f v3f_proj(v3f v, v3f axis);
v3f v3f_inverse(v3f v);
b32 v3f_is_zero(v3f v);
b32 v3f_equal(v3f lhs, v3f rhs);

/* 3x3 Matrix */

typedef struct m3f
{
	r32 v[9];
} m3f;

extern const m3f g_m3f_identity;
extern const m3f g_m3f_zero;

m3f m3f_init_scale(v2f scale);
m3f m3f_init_translation(v2f disp);
m3f m3f_init_rotation(r32 radians);
m3f m3f_inverse(m3f m);
m3f m3f_mul_m3(m3f lhs, m3f rhs);
v2f m3f_mul_v2(m3f m, v2f v);
b32 m3f_equal(m3f lhs, m3f rhs);

/* 4D Vector */

typedef union v4f
{
	struct { r32 x, y, z, w; };
	struct { v2f xy; };
	struct { v3f xyz; };
	struct { r32 d[4]; };
} v4f;

v4f v4f_create_xyz_w(v3f xyz, r32 w);

/* 4x4 Matrix */

typedef struct m4f
{
	r32 v[16];
} m4f;

extern const m4f g_m4f_identity;
extern const m4f g_m4f_zero;

m4f  m4f_init_scale(v3f scale);
m4f  m4f_init_translation(v3f disp);
m4f  m4f_init_rotation(v3f axis, r32 radians);
m4f  m4f_init_rotation_y(r32 radians);
m4f  m4f_perspective(r32 fovy, r32 aspect, r32 near_z, r32 far_z);
m4f  m4f_orthographic(r32 left, r32 right, r32 bottom, r32 top,
                            r32 near_z, r32 far_z);
m4f  m4f_look_at(v3f eye, v3f center, v3f up);
m4f  m4f_mul_m4(m4f lhs, m4f rhs);
v3f  m4f_mul_v3(m4f lhs, v3f rhs);
v3f  m4f_mul_v3w(m4f lhs, v3f rhs, r32 w);
v4f  m4f_mul_v4(m4f lhs, v4f rhs);
m4f  m4f_scale(m4f m, v3f scale);
m4f  m4f_inverse(m4f m);
m4f  m4f_translate(m4f m, v3f diff);
m4f  m4f_rotate(m4f m, v3f axis, r32 radians);
m4f  m4f_transpose(m4f m);
b32  m4f_equal(m4f lhs, m4f rhs);


/* Interval */

typedef struct ivalf
{
	r32 l, r;
} ivalf;

extern const ivalf g_ivalf_0_to_1;
extern const ivalf g_ivalf_zero;

ivalf ivalf_range(r32 center, r32 radius);
r32   ivalf_center(ivalf i);
ivalf ivalf_slide(ivalf i, r32 d);
r32   ivalf_length(ivalf i);
b32   ivalf_contains_val(ivalf i, r32 x);
b32   ivalf_contains_val_within(ivalf i, r32 x, r32 error);
b32   ivalf_contains_ival(ivalf lhs, ivalf rhs);
b32   ivalf_contains_ival_within(ivalf lhs, ivalf rhs, r32 error);
b32   ivalf_overlaps(ivalf lhs, ivalf rhs);
b32   ivalf_overlaps_within(ivalf lhs, ivalf rhs, r32 error);
r32   ivalf_overlap(ivalf lhs, ivalf rhs);
ivalf ivalf_overlap_ival(ivalf lhs, ivalf rhs);
ivalf ivalf_invert(ivalf i);
ivalf ivalf_join(ivalf lhs, ivalf rhs);

/* 2D Anti-aliased bounding box */

typedef struct box2f
{
	v2f min;
	v2f max;
} box2f;

void box2f_from_point(box2f *b, v2f p);
void box2f_from_line(box2f *b, v2f start, v2f end);
void box2f_from_center(box2f *b, v2f center, v2f half_dim);
void box2f_from_dims(box2f *b, r32 left, r32 top, r32 right, r32 bottom);
void box2f_from_xywh(box2f *b, r32 x, r32 y, r32 w, r32 h);
b32  box2f_empty(box2f b);
b32  box2f_contains_point(box2f b, v2f p);
b32  box2f_contains_box(box2f lhs, box2f rhs);
void box2f_clamp_point(box2f b, v2f *p);
b32  box2f_overlaps(box2f lhs, box2f rhs);
b32  box2f_overlaps_within(box2f lhs, box2f rhs, r32 error);
b32  box2f_eq(box2f lhs, box2f rhs);
void box2f_extend_point(box2f *b, v2f p);
void box2f_extend_points(box2f *b, const v2f *p, u32 n);
void box2f_extend_box(box2f *b, box2f other);
void box2f_extend_scalar(box2f *b, r32 dim);
void box2f_translate(box2f *b, v2f v);
void box2f_transform(box2f *b, const m3f mat);
v2f  box2f_get_center(box2f b);
v2f  box2f_get_extent(box2f b);
v2f  box2f_get_half_dim(box2f b);
r32  box2f_get_width(box2f b);
r32  box2f_get_height(box2f b);

/* 3D Anti-aliased bounding box */

typedef struct box3f
{
	v3f min;
	v3f max;
} box3f;

void box3f_from_point(box3f *b, v3f p);
void box3f_extend_point(box3f *b, v3f p);
void box3f_extend_box(box3f *b, box3f other);
v3f  box3f_get_center(box3f b);
v3f  box3f_get_extent(box3f b);
v3f  box3f_get_half_dim(box3f b);
void box3f_clip_v3f(box3f b, v3f *p);

/* Line/Segment utilities */

v2f fmath_line_extrapolate(v2f a, v2f b, r32 t);
r32 fmath_line_interpolate(v2f a, v2f b, v2f p);
b32 fmath_ray_intersect_coords(v2f a, v2f adir, v2f b, v2f bdir, r32 *t, r32 *u);
b32 fmath_ray_intersect(v2f a, v2f adir, v2f b, v2f bdir, v2f *isec);
b32 fmath_line_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1, r32 *t, r32 *u);
b32 fmath_line_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec);
b32 fmath_segment_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1, r32 *t, r32 *u);
b32 fmath_segment_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec);
v2f fmath_nearest_point_on_segment(v2f a, v2f b, v2f p);
v2f fmath_nearest_point_on_line(v2f a, v2f b, v2f p);
r32 fmath_point_to_segment_dist(v2f a, v2f b, v2f p);
r32 fmath_point_to_segment_dist_sq(v2f a, v2f b, v2f p);
r32 fmath_point_to_line_dist(v2f a, v2f b, v2f p);
r32 fmath_point_to_line_dist_sq(v2f a, v2f b, v2f p);
r32 fmath_point_to_ray_dist(v2f a0, v2f dir, v2f p);

ivalf linef_project(v2f a, v2f b, v2f axis);
ivalf linef_project_onto_ray(v2f a, v2f b, v2f start, v2f dir);

/* NOTE(rgriege): returns a unix-style error code - see implementation for details */
s32 fmath_ray_intersect_coords_3d(v3f a, v3f adir, v3f b, v3f bdir, r32 *t, r32 *u);
v3f fmath_nearest_point_on_line_3d(v3f a, v3f b, v3f p);
r32 fmath_point_to_line_dist_3d(v3f a, v3f b, v3f p);
r32 fmath_point_to_line_dist_sq_3d(v3f a, v3f b, v3f p);

/* Polygon */

void  polyf_from_box(v2f v[4], box2f box);
b32   polyf_is_simple(const v2f *v, u32 n);
b32   polyf_is_convex(const v2f *v, u32 n);
b32   polyf_is_concave(const v2f *v, u32 n);
b32   polyf_contains(const v2f *v, u32 n, v2f point);
b32   polyf_contains_fast(const v2f *v, u32 n, box2f *bbox, v2f pt);
void  polyf_bounding_box(const v2f *v, u32 n, box2f *box);
box2f polyf_bbox(const v2f *v, u32 n);
v2f   polyf_extent(const v2f *v, u32 n);
void  polyf_translate(v2f *v, u32 n, v2f delta);
void  polyf_transform(v2f *v, u32 n, const m3f mat);
ivalf polyf_project(const v2f *v, u32 n, v2f axis);
ivalf polyf_project_onto_ray(const v2f *v, u32 n, v2f start, v2f dir);
v2f   polyf_center(const v2f *v, u32 n);
r32   polyf_area(const v2f *v, u32 n);
r32   polyf_area_signed(const v2f *v, u32 n);
r32   polyf_perimeter(const v2f *v, u32 n);
v2f   polyf_centroid(const v2f *v, u32 n);
b32   polyf_is_cw(const v2f *v, u32 n);
b32   polyf_is_ccw(const v2f *v, u32 n);
b32   polyf_line_intersect(const v2f *v, u32 n, v2f v0, v2f v1);
u32   polyf_line_intersections(const v2f *v, u32 n, v2f v0, v2f v1, v2f *first, v2f *last);
b32   polyf_intersect(const v2f *p1, u32 n1, const v2f *p2, u32 n2, v2f *isec);
r32   polyf_pt_dist(const v2f *v, u32 n, v2f p);
r32   polyf_pt_dist_sq(const v2f *v, u32 n, v2f p);

#endif // VIOLET_FMATH_H


/* Implementation */


#ifdef FMATH_IMPLEMENTATION

/* 2D Vector */

const v2f g_v2f_x_axis = {  1,  0 };
const v2f g_v2f_y_axis = {  0,  1 };
const v2f g_v2f_zero   = {  0,  0 };
const v2f g_v2f_one    = {  1,  1 };
const v2f g_v2f_up     = {  0,  1 };
const v2f g_v2f_down   = {  0, -1 };
const v2f g_v2f_left   = { -1,  0 };
const v2f g_v2f_right  = {  1,  0 };

void v2f_set(v2f *v, r32 x, r32 y)
{
	v->x = x;
	v->y = y;
}

r32 v2f_mag(v2f v)
{
	return sqrtf(v2f_mag_sq(v));
}

r32 v2f_mag_sq(v2f v)
{
	return v2f_dot(v, v);
}

r32 v2f_dist(v2f lhs, v2f rhs)
{
	return sqrtf(v2f_dist_sq(lhs, rhs));
}

r32 v2f_dist_sq(v2f lhs, v2f rhs)
{
	v2f_sub_eq(&lhs, rhs);
	return v2f_mag_sq(lhs);
}

v2f v2f_normalize(v2f v)
{
	return v2f_scale(v, 1.f/v2f_mag(v));
}

void v2f_normalize_eq(v2f *v)
{
	*v = v2f_normalize(*v);
}

v2f v2f_scale(v2f v, r32 s)
{
	v2f result = { v.x * s, v.y * s };
	return result;
}

void v2f_scale_eq(v2f *v, r32 s)
{
	v->x *= s;
	v->y *= s;
}

v2f v2f_add(v2f lhs, v2f rhs)
{
	v2f result = { lhs.x + rhs.x, lhs.y + rhs.y };
	return result;
}

void v2f_add_eq(v2f *lhs, v2f rhs)
{
	lhs->x += rhs.x;
	lhs->y += rhs.y;
}

v2f v2f_sub(v2f lhs, v2f rhs)
{
	return (v2f){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
}

void v2f_sub_eq(v2f *lhs, v2f rhs)
{
	lhs->x -= rhs.x;
	lhs->y -= rhs.y;
}

v2f  v2f_mul(v2f lhs, v2f rhs)
{
	return (v2f){ .x = lhs.x * rhs.x, .y = lhs.y * rhs.y };
}

void v2f_mul_eq(v2f *lhs, v2f rhs)
{
	lhs->x *= rhs.x;
	lhs->y *= rhs.y;
}

v2f v2f_div(v2f lhs, v2f rhs)
{
	return (v2f){ .x = lhs.x / rhs.x, .y = lhs.y / rhs.y };
}

void v2f_div_eq(v2f *lhs, v2f rhs)
{
	lhs->x /= rhs.x;
	lhs->y /= rhs.y;
}

v2f v2f_dir(v2f src, v2f dst)
{
	return v2f_normalize(v2f_sub(dst, src));
}

v2f v2f_fmadd(v2f v, v2f dir, r32 s)
{
	return v2f_add(v, v2f_scale(dir, s));
}

v2f v2f_fmsub(v2f v, v2f dir, r32 s)
{
	return v2f_sub(v, v2f_scale(dir, s));
}

v2f v2f_rot(v2f v, r32 radians)
{
	const r32 c = cosf(radians);
	const r32 s = sinf(radians);
	return (v2f){ .x = v.x * c - v.y * s, .y = v.x * s + v.y * c };
}

r32 v2f_dot(v2f lhs, v2f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

r32 v2f_cross(v2f lhs, v2f rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

v2f v2f_proj(v2f v, v2f axis)
{
	return v2f_scale(axis, v2f_dot(v, axis) / v2f_mag_sq(axis));
}

v2f v2f_reflect(v2f v, v2f axis)
{
	v2f proj = v2f_proj(v, axis);
	v2f perp = v2f_sub(v, proj);
	return v2f_sub(proj, perp);
}

v2f v2f_lperp(v2f v)
{
	return (v2f){ .x = -v.y, .y = v.x };
}

v2f v2f_rperp(v2f v)
{
	return (v2f){ .x = v.y, .y = -v.x };
}

v2f v2f_perp_cw(v2f v)
{
	return (v2f){ .x = v.y, .y = -v.x };
}

v2f v2f_perp_ccw(v2f v)
{
	return (v2f){ .x = -v.y, .y = v.x };
}

v2f v2f_inverse(v2f v)
{
	return (v2f){ .x = -v.x, .y = -v.y };
}

void v2f_inverse_eq(v2f *v)
{
	v->x = -v->x;
	v->y = -v->y;
}

v2f v2f_round(v2f v, r32 unit)
{
	v2f_round_eq(&v, unit);
	return v;
}

void v2f_round_eq(v2f *v, r32 unit)
{
	const r32 inv_unit = 1.f / unit;
	v->x = roundf(v->x * inv_unit) / inv_unit;
	v->y = roundf(v->y * inv_unit) / inv_unit;
}

b32 v2f_is_zero(v2f v)
{
	return v.x == 0 && v.y == 0;
}

b32 v2f_equal(v2f lhs, v2f rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

b32 v2f_share_quadrant(v2f lhs, v2f rhs)
{
	return v2f_dot(lhs, rhs) >= 0;
}

v2f v2f_midpoint(v2f v0, v2f v1)
{
	return v2f_scale(v2f_add(v0, v1), 0.5f);
}

v2f v2f_bisect(v2f v0, v2f v1)
{
	return v2f_normalize(v2f_add(v0, v1));
}

v2f v2f_abs(v2f v)
{
	return (v2f){ .x = fabsf(v.x), .y = fabsf(v.y) };
}

/* 3D Vector */

const v3f g_v3f_x_axis = { 1, 0, 0 };
const v3f g_v3f_y_axis = { 0, 1, 0 };
const v3f g_v3f_z_axis = { 0, 0, 1 };
const v3f g_v3f_zero   = { 0, 0, 0 };
const v3f g_v3f_one    = { 1, 1, 1 };

v3f v3f_create_xy_z(v2f xy, r32 z)
{
	return (v3f){ .x = xy.x, .y = xy.y, .z = z };
}

r32 v3f_mag(v3f v)
{
	return sqrtf(v3f_mag_sq(v));
}

r32 v3f_mag_sq(v3f v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

r32 v3f_dist(v3f lhs, v3f rhs)
{
	return sqrtf(v3f_dist_sq(lhs, rhs));
}

r32 v3f_dist_sq(v3f lhs, v3f rhs)
{
	return v3f_mag_sq(v3f_sub(lhs, rhs));
}

v3f v3f_normalize(v3f v)
{
	return v3f_scale(v, 1.f / v3f_mag(v));
}

v3f v3f_scale(v3f v, r32 s)
{
	return (v3f){ .x = v.x * s, .y = v.y * s, .z = v.z * s };
}

v3f v3f_add(v3f lhs, v3f rhs)
{
	return (v3f){ .x = lhs.x + rhs.x, .y = lhs.y + rhs.y, .z = lhs.z + rhs.z };
}

v3f v3f_sub(v3f lhs, v3f rhs)
{
	return (v3f){ .x = lhs.x - rhs.x, .y = lhs.y - rhs.y, .z = lhs.z - rhs.z };
}

v3f v3f_dir(v3f src, v3f dst)
{
	return v3f_normalize(v3f_sub(dst, src));
}

v3f v3f_fmadd(v3f v, v3f dir, r32 s)
{
	return v3f_add(v, v3f_scale(dir, s));
}

v3f v3f_fmsub(v3f v, v3f dir, r32 s)
{
	return v3f_sub(v, v3f_scale(dir, s));
}

r32 v3f_dot(v3f lhs, v3f rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

v3f v3f_cross(v3f lhs, v3f rhs)
{
	return (v3f){
		.x = lhs.y * rhs.z - lhs.z * rhs.y,
		.y = lhs.z * rhs.x - lhs.x * rhs.z,
		.z = lhs.x * rhs.y - lhs.y * rhs.x
	};
}

v3f v3f_proj(v3f v, v3f axis)
{
	return v3f_scale(axis, v3f_dot(v, axis) / v3f_mag_sq(axis));
}

v3f v3f_inverse(v3f v)
{
	return (v3f){ .x = -v.x, .y = -v.y, .z = -v.z };
}

b32 v3f_is_zero(v3f v)
{
	return v3f_equal(v, g_v3f_zero);
}

b32 v3f_equal(v3f lhs, v3f rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}


/* 3x3 Matrix */

const m3f g_m3f_identity = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
};

const m3f g_m3f_zero = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
};

m3f m3f_init_scale(v2f scale)
{
	return (m3f) {
		scale.x, 0,       0,
		0,       scale.y, 0,
		0,       0,       1,
	};
}

m3f m3f_init_translation(v2f disp)
{
	return (m3f) {
		1, 0, disp.x,
		0, 1, disp.y,
		0, 0, 1,
	};
}

m3f m3f_init_rotation(r32 radians)
{
	m3f m;
	m.v[0] = cosf(radians);
	m.v[1] = -sinf(radians);
	m.v[2] = 0;
	m.v[3] = -m.v[1];
	m.v[4] = m.v[0];
	m.v[5] = 0;
	m.v[6] = 0;
	m.v[7] = 0;
	m.v[8] = 1;
	return m;
}

m3f m3f_inverse(m3f m)
{
	const v3f c0 = { m.v[0], m.v[3], m.v[6] };
	const v3f c1 = { m.v[1], m.v[4], m.v[7] };
	const v3f c2 = { m.v[2], m.v[5], m.v[8] };
	const r32 det = v3f_dot(c0, v3f_cross(c1, c2));
	if (det == 0.f) {
		ASSERT_FALSE_AND_LOG("uninvertible");
		return m;
	}
	const r32 s = 1.f / det;
	const v3f r0 = v3f_scale(v3f_cross(c1, c2), s);
	const v3f r1 = v3f_scale(v3f_cross(c2, c0), s);
	const v3f r2 = v3f_scale(v3f_cross(c0, c1), s);
	return (m3f) {
		r0.x, r0.y, r0.z,
		r1.x, r1.y, r1.z,
		r2.x, r2.y, r2.z,
	};
}

m3f m3f_mul_m3(m3f lhs, m3f rhs)
{
	m3f res = g_m3f_zero;
	const r32 *lhs_i, *rhs_k;
	r32 *res_i, lhs_ik;
	for (int i = 0; i < 3; i++) {
		lhs_i = lhs.v + 3 * i;
		res_i = res.v + 3 * i;
		for (int k = 0; k < 3; k++) {
			rhs_k = rhs.v + 3 * k;
			lhs_ik = lhs_i[k];
			for (int j = 0; j < 3; j++)
				res_i[j] += lhs_ik * rhs_k[j];
		}
	}
	return res;
}

v2f m3f_mul_v2(m3f m, v2f v)
{
	v2f result;
	result.x = m.v[0] * v.x + m.v[1] * v.y + m.v[2];
	result.y = m.v[3] * v.x + m.v[4] * v.y + m.v[5];
	return result;
}

b32 m3f_equal(m3f lhs, m3f rhs)
{
	return memcmp(lhs.v, rhs.v, 9 * sizeof(r32)) == 0;
}


/* 4D Vector */

v4f v4f_create_xyz_w(v3f xyz, r32 w)
{
	return (v4f){ .x = xyz.x, .y = xyz.y, .z = xyz.z, .w = w };
}


/* 4x4 Matrix */

const m4f g_m4f_identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const m4f g_m4f_zero = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

m4f m4f_init_scale(v3f scale)
{
	return (m4f) {
		scale.x, 0,       0,       0,
		0,       scale.y, 0,       0,
		0,       0,       scale.z, 0,
		0,       0,       0,       1,
	};
}

m4f m4f_init_translation(v3f disp)
{
	return (m4f) {
		1, 0, 0, disp.x,
		0, 1, 0, disp.y,
		0, 0, 1, disp.z,
		0, 0, 0, 1,
	};
}

m4f m4f_init_rotation(v3f axis, r32 radians)
{
	const v3f a = axis;
	const r32 c = cos(radians);
	const r32 nc = 1.f - c;
	const r32 s = sin(radians);
	return (m4f) {
		c + a.x * a.x * nc,       a.x * a.y * nc - a.z * s, a.x * a.z * nc + a.y * s, 0,
		a.y * a.x * nc + a.z * s, c + a.y * a.y * nc,       a.y * a.z * nc - a.x * s, 0,
		a.z * a.x * nc - a.y * s, a.z * a.y * nc + a.x * s, c + a.z * a.z * nc,       0,
		0,                        0,                        0,                        1,
	};
}

m4f m4f_init_rotation_y(r32 radians)
{
	const r32 c = cosf(radians);
	const r32 s = sinf(radians);
	return (m4f) {
		 c, 0, s, 0,
		 0, 1, 0, 0,
		-s, 0, c, 0,
		 0, 0, 0, 1,
	};
}

m4f m4f_perspective(r32 fovy, r32 aspect, r32 near_z, r32 far_z)
{
	const r32 f = 1.f / tanf(fmath_deg2rad(fovy / 2.f));
	const r32 denom = near_z == far_z ? 1.f : near_z - far_z;
	return (m4f) {
		f / aspect, 0, 0,                        0,
		0,          f, 0,                        0,
		0,          0, (far_z + near_z) / denom, 2.f * far_z * near_z / denom,
		0,          0, -1,                       0
	};
}

m4f m4f_orthographic(r32 left, r32 right, r32 bottom, r32 top,
                           r32 near_z, r32 far_z)
{
	const r32 w = (right - left);
	const r32 h = (top - bottom);
	const r32 depth = (far_z - near_z);
	return (m4f) {
		2.f / w, 0.f,      0.f,          (right + left) / w,
		0.f,     2.f / h,  0.f,          (top + bottom) / h,
		0.f,     0.f,     -2.f / depth, -(far_z + near_z) / depth,
		0.f,     0.f,      0.f,          1.f
	};
}


m4f m4f_look_at(v3f eye, v3f center, v3f up_)
{
	const v3f f  = v3f_normalize(v3f_sub(center, eye));
	const v3f up = v3f_normalize(up_);
	const v3f s  = v3f_cross(f, up);
	const v3f u  = v3f_cross(s, f);
	return (m4f) {
		 s.x,  s.y,  s.z, -v3f_dot(s, eye),
		 u.x,  u.y,  u.z, -v3f_dot(u, eye),
		-f.x, -f.y, -f.z,  v3f_dot(f, eye),
		   0,    0,    0, 1,
	};
}

m4f m4f_mul_m4(m4f lhs, m4f rhs)
{
	const r32 *lhs_i, *rhs_k;
	r32 *res_i;
	m4f res = g_m4f_zero;
	for (int i = 0; i < 4; ++i) {
		lhs_i = lhs.v + 4 * i;
		res_i = res.v + 4 * i;
		for (int k = 0; k < 4; ++k) {
			rhs_k = rhs.v + 4 * k;
			for (int j = 0; j < 4; j++)
				res_i[j] += lhs_i[k] * rhs_k[j];
		}
	}
	return res;
}

v3f m4f_mul_v3(m4f lhs, v3f rhs)
{
	v4f result = m4f_mul_v4(lhs, v4f_create_xyz_w(rhs, 1.f));
	return v3f_scale(result.xyz, 1.f / result.w);
}

v3f m4f_mul_v3w(m4f lhs, v3f rhs, r32 w)
{
	return m4f_mul_v4(lhs, v4f_create_xyz_w(rhs, w)).xyz;
}

v4f m4f_mul_v4(m4f lhs, v4f rhs)
{
	return (v4f) {
		.x = lhs.v[ 0] * rhs.x + lhs.v[ 1] * rhs.y + lhs.v[ 2] * rhs.z + lhs.v[ 3] * rhs.w,
		.y = lhs.v[ 4] * rhs.x + lhs.v[ 5] * rhs.y + lhs.v[ 6] * rhs.z + lhs.v[ 7] * rhs.w,
		.z = lhs.v[ 8] * rhs.x + lhs.v[ 9] * rhs.y + lhs.v[10] * rhs.z + lhs.v[11] * rhs.w,
		.w = lhs.v[12] * rhs.x + lhs.v[13] * rhs.y + lhs.v[14] * rhs.z + lhs.v[15] * rhs.w,
	};
}

m4f m4f_scale(m4f m, v3f scale)
{
	return m4f_mul_m4(m4f_init_scale(scale), m);
}

m4f m4f_inverse(m4f m)
{
	m4f dst;

	const r32 m00 = m.v[0 * 4 + 0];
	const r32 m01 = m.v[0 * 4 + 1];
	const r32 m02 = m.v[0 * 4 + 2];
	const r32 m03 = m.v[0 * 4 + 3];
	const r32 m10 = m.v[1 * 4 + 0];
	const r32 m11 = m.v[1 * 4 + 1];
	const r32 m12 = m.v[1 * 4 + 2];
	const r32 m13 = m.v[1 * 4 + 3];
	const r32 m20 = m.v[2 * 4 + 0];
	const r32 m21 = m.v[2 * 4 + 1];
	const r32 m22 = m.v[2 * 4 + 2];
	const r32 m23 = m.v[2 * 4 + 3];
	const r32 m30 = m.v[3 * 4 + 0];
	const r32 m31 = m.v[3 * 4 + 1];
	const r32 m32 = m.v[3 * 4 + 2];
	const r32 m33 = m.v[3 * 4 + 3];
	const r32 tmp_0  = m22 * m33;
	const r32 tmp_1  = m32 * m23;
	const r32 tmp_2  = m12 * m33;
	const r32 tmp_3  = m32 * m13;
	const r32 tmp_4  = m12 * m23;
	const r32 tmp_5  = m22 * m13;
	const r32 tmp_6  = m02 * m33;
	const r32 tmp_7  = m32 * m03;
	const r32 tmp_8  = m02 * m23;
	const r32 tmp_9  = m22 * m03;
	const r32 tmp_10 = m02 * m13;
	const r32 tmp_11 = m12 * m03;
	const r32 tmp_12 = m20 * m31;
	const r32 tmp_13 = m30 * m21;
	const r32 tmp_14 = m10 * m31;
	const r32 tmp_15 = m30 * m11;
	const r32 tmp_16 = m10 * m21;
	const r32 tmp_17 = m20 * m11;
	const r32 tmp_18 = m00 * m31;
	const r32 tmp_19 = m30 * m01;
	const r32 tmp_20 = m00 * m21;
	const r32 tmp_21 = m20 * m01;
	const r32 tmp_22 = m00 * m11;
	const r32 tmp_23 = m10 * m01;

	const r32 t0 = (tmp_0 * m11 + tmp_3 * m21 + tmp_4 * m31) -
		(tmp_1 * m11 + tmp_2 * m21 + tmp_5 * m31);
	const r32 t1 = (tmp_1 * m01 + tmp_6 * m21 + tmp_9 * m31) -
		(tmp_0 * m01 + tmp_7 * m21 + tmp_8 * m31);
	const r32 t2 = (tmp_2 * m01 + tmp_7 * m11 + tmp_10 * m31) -
		(tmp_3 * m01 + tmp_6 * m11 + tmp_11 * m31);
	const r32 t3 = (tmp_5 * m01 + tmp_8 * m11 + tmp_11 * m21) -
		(tmp_4 * m01 + tmp_9 * m11 + tmp_10 * m21);

	const r32 d = 1.0 / (m00 * t0 + m10 * t1 + m20 * t2 + m30 * t3);

	dst.v[ 0] = d * t0;
	dst.v[ 1] = d * t1;
	dst.v[ 2] = d * t2;
	dst.v[ 3] = d * t3;
	dst.v[ 4] = d * ((tmp_1 * m10 + tmp_2 * m20 + tmp_5 * m30) -
	    	(tmp_0 * m10 + tmp_3 * m20 + tmp_4 * m30));
	dst.v[ 5] = d * ((tmp_0 * m00 + tmp_7 * m20 + tmp_8 * m30) -
	    	(tmp_1 * m00 + tmp_6 * m20 + tmp_9 * m30));
	dst.v[ 6] = d * ((tmp_3 * m00 + tmp_6 * m10 + tmp_11 * m30) -
	    	(tmp_2 * m00 + tmp_7 * m10 + tmp_10 * m30));
	dst.v[ 7] = d * ((tmp_4 * m00 + tmp_9 * m10 + tmp_10 * m20) -
	    	(tmp_5 * m00 + tmp_8 * m10 + tmp_11 * m20));
	dst.v[ 8] = d * ((tmp_12 * m13 + tmp_15 * m23 + tmp_16 * m33) -
	    	(tmp_13 * m13 + tmp_14 * m23 + tmp_17 * m33));
	dst.v[ 9] = d * ((tmp_13 * m03 + tmp_18 * m23 + tmp_21 * m33) -
	    	(tmp_12 * m03 + tmp_19 * m23 + tmp_20 * m33));
	dst.v[10] = d * ((tmp_14 * m03 + tmp_19 * m13 + tmp_22 * m33) -
	    	(tmp_15 * m03 + tmp_18 * m13 + tmp_23 * m33));
	dst.v[11] = d * ((tmp_17 * m03 + tmp_20 * m13 + tmp_23 * m23) -
	    	(tmp_16 * m03 + tmp_21 * m13 + tmp_22 * m23));
	dst.v[12] = d * ((tmp_14 * m22 + tmp_17 * m32 + tmp_13 * m12) -
	    	(tmp_16 * m32 + tmp_12 * m12 + tmp_15 * m22));
	dst.v[13] = d * ((tmp_20 * m32 + tmp_12 * m02 + tmp_19 * m22) -
	    	(tmp_18 * m22 + tmp_21 * m32 + tmp_13 * m02));
	dst.v[14] = d * ((tmp_18 * m12 + tmp_23 * m32 + tmp_15 * m02) -
	    	(tmp_22 * m32 + tmp_14 * m02 + tmp_19 * m12));
	dst.v[15] = d * ((tmp_22 * m22 + tmp_16 * m02 + tmp_21 * m12) -
			(tmp_20 * m12 + tmp_23 * m22 + tmp_17 * m02));

	return dst;
}

m4f m4f_translate(m4f m, v3f diff)
{
	return m4f_mul_m4(m4f_init_translation(diff), m);
}

m4f m4f_rotate(m4f m, v3f axis, r32 radians)
{
	return m4f_mul_m4(m4f_init_rotation(axis, radians), m);
}

m4f m4f_transpose(m4f m)
{
	return (m4f) {
		m.v[0], m.v[4], m.v[8],  m.v[12],
		m.v[1], m.v[5], m.v[9],  m.v[13],
		m.v[2], m.v[6], m.v[10], m.v[14],
		m.v[3], m.v[7], m.v[11], m.v[15],
	};
}

b32 m4f_equal(m4f lhs, m4f rhs)
{
	return memcmp(lhs.v, rhs.v, 16 * sizeof(r32)) == 0;
}

/* Interval */

const ivalf g_ivalf_0_to_1 = { .l = 0.f, .r = 1.f };
const ivalf g_ivalf_zero   = { .l = 0.f, .r = 0.f };

ivalf ivalf_range(r32 center, r32 radius)
{
	return (ivalf){
		.l = center - radius,
		.r = center + radius,
	};
}

r32 ivalf_center(ivalf i)
{
	return (i.l + i.r) / 2.f;
}

ivalf ivalf_slide(ivalf i, r32 d)
{
	return (ivalf){
		.l = i.l + d,
		.r = i.r + d,
	};
}

r32 ivalf_length(ivalf i)
{
	return i.r - i.l;
}

b32 ivalf_contains_val(ivalf i, r32 x)
{
	return x >= i.l && x <= i.r;
}

b32 ivalf_contains_val_within(ivalf i, r32 x, r32 error)
{
	return x - i.l >= -error && i.r - x >= -error;
}

b32 ivalf_contains_ival(ivalf lhs, ivalf rhs)
{
	return rhs.l >= lhs.l && rhs.r <= lhs.r;
}

b32 ivalf_contains_ival_within(ivalf lhs, ivalf rhs, r32 error)
{
	return rhs.l - lhs.l >= -error && rhs.r - lhs.r <= error;
}

b32 ivalf_overlaps(ivalf lhs, ivalf rhs)
{
	return lhs.l <= rhs.r && rhs.l <= lhs.r;
}

b32 ivalf_overlaps_within(ivalf lhs, ivalf rhs, r32 error)
{
	return rhs.r - lhs.l > -error && lhs.r - rhs.l > -error;
}

r32 ivalf_overlap(ivalf lhs, ivalf rhs)
{
	const ivalf overlap = ivalf_overlap_ival(lhs, rhs);
	const r32 len = ivalf_length(overlap);
	return fmaxf(len, 0);
}

ivalf ivalf_overlap_ival(ivalf lhs, ivalf rhs)
{
	return (ivalf){ .l = fmaxf(lhs.l, rhs.l), .r = fminf(lhs.r, rhs.r) };
}

ivalf ivalf_invert(ivalf i)
{
	return (ivalf){ .l = -i.r, .r = -i.l };
}

ivalf ivalf_join(ivalf lhs, ivalf rhs)
{
	return (ivalf){ .l = fminf(lhs.l, rhs.l), .r = fmaxf(lhs.r, rhs.r) };
}

/* 2D Anti-aliased bounding box */

void box2f_from_point(box2f *b, v2f p)
{
	b->min = p;
	b->max = p;
}

void box2f_from_line(box2f *box, v2f start, v2f end)
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

void box2f_from_center(box2f *box, v2f center, v2f half_dim)
{
	box->min.x = center.x - half_dim.x;
	box->min.y = center.y - half_dim.y;
	box->max.x = center.x + half_dim.x;
	box->max.y = center.y + half_dim.y;
}

void box2f_from_dims(box2f *box, r32 left, r32 top, r32 right, r32 bottom)
{
	box->min.x = left;
	box->min.y = bottom;
	box->max.x = right;
	box->max.y = top;
}

void box2f_from_xywh(box2f *box, r32 x, r32 y, r32 w, r32 h)
{
	box->min.x = x;
	box->min.y = y;
	box->max.x = x + w;
	box->max.y = y + h;
}

b32 box2f_empty(box2f box)
{
	return v2f_equal(box.min, box.max);
}

b32 box2f_contains_point(box2f box, v2f point)
{
	return    point.x >= box.min.x
	       && point.x <= box.max.x
	       && point.y >= box.min.y
	       && point.y <= box.max.y;
}

b32 box2f_contains_box(box2f lhs, box2f rhs)
{
	ivalf lhs_x = { lhs.min.x, lhs.max.x };
	ivalf rhs_x = { rhs.min.x, rhs.max.x };
	ivalf lhs_y = { lhs.min.y, lhs.max.y };
	ivalf rhs_y = { rhs.min.y, rhs.max.y };
	return    ivalf_contains_ival(lhs_x, rhs_x)
	       && ivalf_contains_ival(lhs_y, rhs_y);
}

void box2f_clamp_point(box2f b, v2f *p)
{
	p->x = fmath_clamp(b.min.x, p->x, b.max.x);
	p->y = fmath_clamp(b.min.y, p->y, b.max.y);
}

b32 box2f_overlaps(box2f lhs, box2f rhs)
{
	ivalf lhs_x = { lhs.min.x, lhs.max.x };
	ivalf rhs_x = { rhs.min.x, rhs.max.x };
	ivalf lhs_y = { lhs.min.y, lhs.max.y };
	ivalf rhs_y = { rhs.min.y, rhs.max.y };
	return    ivalf_overlaps(lhs_x, rhs_x)
	       && ivalf_overlaps(lhs_y, rhs_y);
}

b32 box2f_overlaps_within(box2f lhs, box2f rhs, r32 error)
{
	ivalf lhs_x = { lhs.min.x, lhs.max.x };
	ivalf rhs_x = { rhs.min.x, rhs.max.x };
	ivalf lhs_y = { lhs.min.y, lhs.max.y };
	ivalf rhs_y = { rhs.min.y, rhs.max.y };
	return    ivalf_overlaps_within(lhs_x, rhs_x, error)
	       && ivalf_overlaps_within(lhs_y, rhs_y, error);
}

b32 box2f_eq(box2f lhs, box2f rhs)
{
	return    lhs.min.x == rhs.min.x
	       && lhs.min.y == rhs.min.y
	       && lhs.max.x == rhs.max.x
	       && lhs.max.y == rhs.max.y;
}

void box2f_extend_point(box2f *box, v2f point)
{
	box2f_from_dims(box, fminf(box->min.x, point.x), fmaxf(box->max.y, point.y),
	                fmaxf(box->max.x, point.x), fminf(box->min.y, point.y));
}

void box2f_extend_points(box2f *box, const v2f *p, u32 n)
{
	for (u32 i = 0; i < n; ++i)
		box2f_extend_point(box, p[i]);
}

void box2f_extend_box(box2f *lhs, box2f rhs)
{
	box2f_extend_point(lhs, rhs.min);
	box2f_extend_point(lhs, rhs.max);
}

void box2f_extend_scalar(box2f *b, r32 dim)
{
	b->min.x -= dim;
	b->min.y -= dim;
	b->max.x += dim;
	b->max.y += dim;
}

void box2f_translate(box2f *box, v2f off)
{
	v2f_add_eq(&box->min, off);
	v2f_add_eq(&box->max, off);
}

void box2f_transform(box2f *box, const m3f mat)
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

v2f box2f_get_center(box2f box)
{
	return v2f_add(box.min, box2f_get_half_dim(box));
}

v2f box2f_get_extent(box2f b)
{
	return v2f_sub(b.max, b.min);
}

v2f box2f_get_half_dim(box2f box)
{
	return v2f_scale(v2f_sub(box.max, box.min), 0.5f);
}

r32 box2f_get_width(box2f b)
{
	return b.max.x - b.min.x;
}

r32 box2f_get_height(box2f b)
{
	return b.max.y - b.min.y;
}

/* Line/Segment utilities */

void box3f_from_point(box3f *b, v3f p)
{
	b->min = p;
	b->max = p;
}

void box3f_extend_point(box3f *b, v3f p)
{
	b->min.x = fminf(b->min.x, p.x);
	b->min.y = fminf(b->min.y, p.y);
	b->min.z = fminf(b->min.z, p.z);
	b->max.x = fmaxf(b->max.x, p.x);
	b->max.y = fmaxf(b->max.y, p.y);
	b->max.z = fmaxf(b->max.z, p.z);
}

void box3f_extend_box(box3f *lhs, box3f rhs)
{
	box3f_extend_point(lhs, rhs.min);
	box3f_extend_point(lhs, rhs.max);
}

v3f box3f_get_center(box3f b)
{
	return v3f_add(b.min, box3f_get_half_dim(b));
}

v3f box3f_get_extent(box3f b)
{
	return v3f_sub(b.max, b.min);
}

v3f box3f_get_half_dim(box3f b)
{
	return v3f_scale(v3f_sub(b.max, b.min), 0.5f);
}

void box3f_clip_v3f(box3f b, v3f *p)
{
	p->x = fmaxf(fminf(p->x, b.max.x), b.min.x);
	p->y = fmaxf(fminf(p->y, b.max.y), b.min.y);
	p->z = fmaxf(fminf(p->z, b.max.z), b.min.z);
}


v2f fmath_line_extrapolate(v2f a, v2f b, r32 t)
{
	return v2f_add(a, v2f_scale(v2f_sub(b, a), t));
}

static r32 fmath__line_interpolate(v2f a, v2f b, v2f p, v2f *ab)
{
	*ab = v2f_sub(b, a);
	return v2f_dot(v2f_sub(p, a), *ab) / v2f_mag_sq(*ab);
}

r32 fmath_line_interpolate(v2f a, v2f b, v2f p)
{
	v2f ab;
	return fmath__line_interpolate(a, b, p, &ab);
}

b32 fmath_ray_intersect_coords(v2f a, v2f adir, v2f b, v2f bdir, r32 *t, r32 *u)
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

	const v2f ab = v2f_sub(b, a);
	const r32 rxs = v2f_cross(adir, bdir);

	// parallel or collinear rays are considered not intersecting
	if (fabsf(rxs) < FLT_EPSILON)
		return false;

	*t = v2f_cross(ab, bdir) / rxs;
	*u = v2f_cross(ab, adir) / rxs;
	return true;
}

b32 fmath_ray_intersect(v2f a, v2f adir, v2f b, v2f bdir, v2f *isec)
{
	r32 t = 0, u = 0;
	b32 result = fmath_ray_intersect_coords(a, adir, b, bdir, &t, &u);
	*isec = v2f_fmadd(a, adir, t);
	return result;
}

b32 fmath_line_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1, r32 *t, r32 *u)
{
	v2f a, b;
	a = v2f_sub(a1, a0);
	b = v2f_sub(b1, b0);
	return fmath_ray_intersect_coords(a0, a, b0, b, t, u);
}

b32 fmath_line_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec)
{
	r32 t = 0, u = 0;
	b32 result = fmath_line_intersect_coords(a0, a1, b0, b1, &t, &u);
	*isec = fmath_line_extrapolate(a0, a1, t);
	return result;
}

b32 fmath_segment_intersect_coords(v2f a0, v2f a1, v2f b0, v2f b1, r32 *t, r32 *u)
{
	return    fmath_line_intersect_coords(a0, a1, b0, b1, t, u)
	       && 0 < *u && *u < 1 && 0 < *t && *t < 1;
}

b32 fmath_segment_intersect(v2f a0, v2f a1, v2f b0, v2f b1, v2f *isec)
{
	r32 t = 0, u = 0;
	b32 result = fmath_segment_intersect_coords(a0, a1, b0, b1, &t, &u);
	*isec = fmath_line_extrapolate(a0, a1, t);
	return result;
}

v2f fmath_nearest_point_on_segment(v2f a, v2f b, v2f p)
{
	v2f ab;
	r32 t = fmath_clamp(0, fmath__line_interpolate(a, b, p, &ab), 1);
	return v2f_add(a, v2f_scale(ab, t));
}

v2f fmath_nearest_point_on_line(v2f a, v2f b, v2f p)
{
	v2f ab;
	r32 t = fmath__line_interpolate(a, b, p, &ab);
	return v2f_add(a, v2f_scale(ab, t));
}

r32 fmath_point_to_segment_dist(v2f a, v2f b, v2f p)
{
	return sqrtf(fmath_point_to_segment_dist_sq(a, b, p));
}

r32 fmath_point_to_segment_dist_sq(v2f a, v2f b, v2f p)
{
	return v2f_dist_sq(p, fmath_nearest_point_on_segment(a, b, p));
}

r32 fmath_point_to_line_dist(v2f a, v2f b, v2f p)
{
	return sqrtf(fmath_point_to_line_dist_sq(a, b, p));
}

r32 fmath_point_to_line_dist_sq(v2f a, v2f b, v2f p)
{
	return v2f_dist_sq(p, fmath_nearest_point_on_line(a, b, p));
}

r32 fmath_point_to_ray_dist(v2f a0, v2f dir, v2f p)
{
	const v2f a1 = v2f_add(a0, dir);
	return fmath_point_to_line_dist(a0, a1, p);
}

ivalf linef_project(v2f a, v2f b, v2f axis)
{
	const r32 dp_a = v2f_dot(a, axis);
	const r32 dp_b = v2f_dot(b, axis);
	return (ivalf){ .l = fminf(dp_a, dp_b), .r = fmaxf(dp_a, dp_b) };
}

ivalf linef_project_onto_ray(v2f a, v2f b, v2f start, v2f dir)
{
	return ivalf_slide(linef_project(a, b, dir), -v2f_dot(start, dir));
}

s32 fmath_ray_intersect_coords_3d(v3f a, v3f adir, v3f b, v3f bdir, r32 *t, r32 *u)
{
	/* (uppercase = vector, lowercase = scalar)
	 * ray intersection equation
	 * A + tR = B + uS
	 *
	 * (A + tR) x S = (B + uS) x S                # cross both sides by S
	 * A x S = tR x S = B x S + uS x S            # distribute cross product
	 * A x S + tR x S = B x S                     # S x S = 0
	 * tR x S = B x S - A x S
	 * t(R x S) = (B - A) x S
	 * t(R x S) = AB x S                          # let AB = B - A
	 * t(R x S) * (R x S) = (AB x S) * (R x S)    # * = dot product
	 * t|R x S|^2 = (AB x S) * (R x S)            # A * A = |A|^2
	 * t = (AB x S) * (R x S) / |R x S|^2
	 * ...
	 * u = (AB x R) * (R x S) / |R x S|^2 */

	const v3f ab = v3f_sub(b, a);
	const v3f rxs = v3f_cross(adir, bdir);
	const r32 rxs_mag_sq = v3f_mag_sq(rxs);

	/* parallel or collinear rays are considered not intersecting */
	if (fabsf(rxs_mag_sq) < FLT_EPSILON)
		return -1;

	*t = v3f_dot(v3f_cross(ab, bdir), rxs) / rxs_mag_sq;
	*u = v3f_dot(v3f_cross(ab, adir), rxs) / rxs_mag_sq;

	/* rays must be co-planar, but I couldn't determine a good error margin,
	 * so we allow the caller to handle it. */
	return (v3f_dot(ab, rxs) > 10.f * FLT_EPSILON) ? 1 : 0;
}

static r32 fmath__line_interpolate_3d(v3f a, v3f b, v3f p, v3f *ab)
{
	*ab = v3f_sub(b, a);
	return v3f_dot(v3f_sub(p, a), *ab) / v3f_mag_sq(*ab);
}

v3f fmath_nearest_point_on_line_3d(v3f a, v3f b, v3f p)
{
	v3f ab;
	r32 t = fmath__line_interpolate_3d(a, b, p, &ab);
	return v3f_add(a, v3f_scale(ab, t));
}

r32 fmath_point_to_line_dist_3d(v3f a, v3f b, v3f p)
{
	return sqrtf(fmath_point_to_line_dist_sq_3d(a, b, p));
}

r32 fmath_point_to_line_dist_sq_3d(v3f a, v3f b, v3f p)
{
	return v3f_dist_sq(p, fmath_nearest_point_on_line_3d(a, b, p));
}

/* Polygon */

void polyf_from_box(v2f v[4], box2f box)
{
	const v2f top_left = { box.min.x, box.max.y };
	const v2f bottom_right = { box.max.x, box.min.y };

	v[0] = bottom_right;
	v[1] = box.max;
	v[2] = top_left;
	v[3] = box.min;
}

b32 polyf_is_simple(const v2f *v, u32 n)
{
	if (n < 3) {
		ASSERT_FALSE_AND_LOG("n = %u", n);
		return false;
	}

	v2f isec, a0, a1, b0, b1;
	u32 jend;

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

b32 polyf_is_convex(const v2f *v, u32 n)
{
	if (n < 3) {
		ASSERT_FALSE_AND_LOG("n = %u", n);
		return false;
	}

	b32 ccw_determined = false, ccw = false;
	for (u32 i = 0, last = n-1; i <= last; ++i) {
		v2f a = v[(i>0 ? i-1 : last)];
		v2f b = v[i];
		v2f c = v[(i<last ? i+1 : 0)];

		v2f ab, bc;
		ab = v2f_sub(b, a);
		bc = v2f_sub(c, b);
		const r32 cross = v2f_cross(ab, bc);
		if (cross != 0.f) {
			if (!ccw_determined) {
				ccw_determined = true;
				ccw = cross > 0.f;
			} else if ((cross > 0.f) != ccw) {
				return false;
			}
		}
	}
	return true;
}

b32 polyf_is_concave(const v2f *v, u32 n)
{
	return !polyf_is_convex(v, n);
}

b32 polyf_contains(const v2f *v, u32 n, v2f point)
{
	box2f box;
	v2f out_p;
	u32 intersections;
	r32 t, u;

	polyf_bounding_box(v, n, &box);
	if (!box2f_contains_point(box, point))
		return false;

	out_p = v2f_add(v2f_sub(box.max, box.min), box.max);

	intersections = 0;
	for (u32 i = 0; i < n; ++i) {
		v2f a = v[i];
		v2f b = v[(i+1)%n];
		if (   fmath_line_intersect_coords(a, b, out_p, point, &t, &u)
		    && FLT_EPSILON < t && t <= 1 + FLT_EPSILON && 0 < u && u < 1)
			++intersections;
	}
	return intersections % 2 == 1;
}

// TODO(luke): write tests... get advice from Ryan
/* Algorithm per https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
   Known to report false negatives (i.e. point not in polygon) when point is
   on some edges of the polygon, but not others. */
b32 polyf_contains_fast(const v2f *v, u32 n, box2f *box, v2f point)
{
	if (!box2f_contains_point(*box, point))
		return false;

	b32 result = false;
	for (u32 i = 0, j = n-1; i < n; j = i++)
		if (   ((v[i].y > point.y) != (v[j].y > point.y))
		    && (point.x < (v[j].x - v[i].x) * (point.y - v[i].y) / (v[j].y - v[i].y) + v[i].x))
			result = !result;

	return result;
}

void polyf_bounding_box(const v2f *v, u32 n, box2f *box)
{
	if (n == 0) {
		ASSERT_FALSE_AND_LOG("n = %u", n);
		box2f_from_point(box, g_v2f_zero);
		return;
	}
	box2f_from_point(box, v[0]);
	box2f_extend_points(box, &v[1], n-1);
}

box2f polyf_bbox(const v2f *v, u32 n)
{
	box2f box;
	polyf_bounding_box(v, n, &box);
	return box;
}

v2f polyf_extent(const v2f *v, u32 n)
{
	box2f bbox;
	polyf_bounding_box(v, n, &bbox);
	return box2f_get_extent(bbox);
}

void polyf_translate(v2f *v, u32 n, v2f delta)
{
	for (const v2f *vn = v+n; v != vn; ++v)
		v2f_add_eq(v, delta);
}

void polyf_transform(v2f *v, u32 n, const m3f mat)
{
	for (const v2f *vn = v+n; v != vn; ++v)
		*v = m3f_mul_v2(mat, *v);
}

ivalf polyf_project(const v2f *v, u32 n, v2f axis)
{
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

ivalf polyf_project_onto_ray(const v2f *v, u32 n, v2f start, v2f dir)
{
	return ivalf_slide(polyf_project(v, n, dir), -v2f_dot(start, dir));
}

v2f polyf_center(const v2f *v, u32 n)
{
	v2f center = { .x=0, .y=0 };
	for (const v2f *vn = v+n; v != vn; ++v)
		v2f_add_eq(&center, *v);
	return v2f_scale(center, 1.f/n);
}

r32 polyf_area(const v2f *v, u32 n)
{
	return fabsf(polyf_area_signed(v, n));
}

// NOTE(rgriege): Green's theorem
r32 polyf_area_signed(const v2f *v, u32 n)
{
	if (n < 3) {
		ASSERT_FALSE_AND_LOG("n = %u", n);
		return 0.f;
	}

	r32 area = 0;
	v2f prev = v[n-1];

	for (const v2f *vn = v+n; v != vn; ++v)
	{
		area += v2f_cross(prev, *v);
		prev = *v;
	}
	return area/2;
}

r32 polyf_perimeter(const v2f *v, u32 n)
{
	r32 perimeter = 0.f;
	for (u32 i = 0; i < n; ++i)
		perimeter += v2f_dist(v[i], v[(i+1)%n]);
	return perimeter;
}

v2f polyf_centroid(const v2f *v, u32 n)
{
	const r32 denom = 6 * polyf_area_signed(v, n);
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

b32 polyf_is_cw(const v2f *v, u32 n)
{
	return !polyf_is_ccw(v, n);
}

b32 polyf_is_ccw(const v2f *v, u32 n)
{
	if (n < 3) {
		ASSERT_FALSE_AND_LOG("n = %u", n);
		return false;
	}

	r32 twice_area = 0.f;
	for (u32 i = 0, last = n-1; i <= last; ++i) {
		const v2f a = v[i];
		const v2f b = v[(i<last ? i+1 : 0)];
		twice_area += v2f_cross(a, b);
	}
	return twice_area > 0.f;
}

b32 polyf_line_intersect(const v2f *v, u32 n, v2f v0, v2f v1)
{
	v2f first, last;
	return polyf_line_intersections(v, n, v0, v1, &first, &last) > 0;
}

u32 polyf_line_intersections(const v2f *v, u32 n, v2f v0, v2f v1, v2f *first, v2f *last)
{
	const v2f dir = v2f_dir(v0, v1);
	u32 cnt = 0;
	v2f prev = v[n-1];
	r32 t, u, min_u = FLT_MAX, max_u = -FLT_MAX;
	for (const v2f *vn=v+n; v!=vn; ++v) {
		if (   fabsf(v2f_dot(dir, v2f_dir(prev, *v))) < 0.999f
		    && fmath_line_intersect_coords(prev, *v, v0, v1, &t, &u)
		    && t >= 0.f && t <= 1.f) {
			if (u < min_u) {
				min_u = u;
				*first = fmath_line_extrapolate(v0, v1, u);
			}
			if (u > max_u) {
				max_u = u;
				*last = fmath_line_extrapolate(v0, v1, u);
			}
			++cnt;
		}
		prev = *v;
	}
	return cnt;
}

b32 polyf_intersect(const v2f *p1, u32 n1, const v2f *p2, u32 n2, v2f *isec)
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

r32 polyf_pt_dist(const v2f *v, u32 n, v2f p)
{
	return sqrtf(polyf_pt_dist_sq(v, n , p));
}

r32 polyf_pt_dist_sq(const v2f *v, u32 n, v2f p)
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
