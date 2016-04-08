#ifndef VIOLET_IBOX_H
#define VIOLET_IBOX_H

#include "violet/math/v2i.h"

typedef struct ibox
{
	v2i top_left;
	v2i bottom_right;
} ibox;

void ibox_from_line(ibox * b, const v2i * start, const v2i * end);
void ibox_from_center(ibox * b, const v2i * center, const v2i * half_dim);
void ibox_from_dims(ibox * b, s32 left, s32 top, s32 right, s32 bottom);

b8 ibox_empty(const ibox * b);

b8 ibox_contains_v(const ibox * b, const v2i * p);
b8 ibox_contains_b(const ibox * lhs, const ibox * rhs);
#define ibox_contains(X, Y) _Generic((Y), \
	v2i* : ibox_contains_v,               \
	ibox*: ibox_contains_b                \
	)(X, Y)

b8 ibox_overlaps(const ibox * lhs, const ibox * rhs);

void ibox_extend_v(ibox * b, const v2i * p);
void ibox_extend_b(ibox * b, const ibox * other);
#define ibox_extend(X, Y) _Generic((Y), \
	v2i* : ibox_extend_v,               \
	ibox*: ibox_extend_b                \
	)(X, Y)

void ibox_translate(ibox * b, const v2i * v);

void ibox_get_center(const ibox * b, v2i * center);
void ibox_get_half_dim(const ibox * b, v2i * half_dim);
void ibox_get_min(const ibox * b, v2i * min);
void ibox_get_max(const ibox * b, v2i * max);

#endif
