#ifndef VIOLET_V3_H
#define VIOLET_V3_H

#include "violet/core/types.h"

typedef struct reader reader;
typedef struct writer writer;

typedef struct v3
{
	r32 x, y, z;
} v3;

extern const v3 g_v3_x_axis;
extern const v3 g_v3_y_axis;
extern const v3 g_v3_z_axis;
extern const v3 g_v3_zero;

r32 v3_mag(const v3 * v);
r32 v3_mag_squared(const v3 * v);
void normalize(const v3 * v, v3 * res);
b8 v3_is_unit(const v3 * v);
void v3_scale(const v3 * v, r32 sx, r32 sy, r32 sz, v3 * res);

void v3_add(const v3 * lhs, const v3 * rhs, v3 * res);
void v3_sub(const v3 * lhs, const v3 * rhs, v3 * res);

r32 v3_dot(const v3 * lhs, const v3 * rhs);
void v3_cross(const v3 * lhs, const v3 * rhs, v3 * res);
void v3_proj(const v3 * v, const v3 * axis, v3 * res);
void v3_inverse(const v3 * v, v3 * res);
b8 v3_is_zero(const v3 * v);
b8 v3_equal(const v3 * lhs, const v3 * rhs);

void v3_read(reader * r, v3 * v);
void v3_write(writer * w, const v3 * v);

#endif

