#ifndef VIOLET_V2_H
#define VIOLET_V2_H

#include "violet/core/types.h"

typedef struct reader reader;
typedef struct writer writer;

typedef struct
{
	union
	{
		struct { r32 x, y; };
		struct { r32 u, v; };
	};
} v2;


extern const v2 g_v2_x_axis;
extern const v2 g_v2_y_axis;
extern const v2 g_v2_zero;

r32 v2_mag(const v2 * v);
r32 v2_mag_squared(const v2 * v);
void v2_normalize(const v2 * v, v2 * result);
b8 v2_is_unit(const v2 * v);
void v2_scale(const v2 * v, r32 sx, r32 sy, v2 * result);

void v2_add(const v2 * lhs, const v2 * rhs, v2 * result);
void v2_sub(const v2 * lhs, const v2 * rhs, v2 * result);

void v2_rot(const v2 * v, r32 radians, v2 * result);
r32 v2_dot(const v2 * lhs, const v2 * rhs);
r32 v2_cross(const v2 * lhs, const v2 * rhs);
void v2_proj(const v2 * v, const v2 * axis, v2 * result);
void v2_perp(const v2 * v, b8 left, v2 * result);
void v2_inverse(const v2 * v, v2 * result);
b8 v2_is_zero(const v2 * v);
b8 v2_equal(const v2 * lhs, const v2 * rhs);
b8 v2_share_quadrant(const v2 * lhs, const v2 * rhs);

void v2_read(reader * r, v2 * v);
void v2_write(writer * w, const v2 * v);

#endif
