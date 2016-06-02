#ifndef VIOLET_V2I_H
#define VIOLET_V2I_H

#include "violet/core/types.h"

typedef struct v2i
{
	union
	{
		struct { s32 x, y; };
		struct { s32 j, i; };
	};
} v2i;

void v2i_add(const v2i * lhs, const v2i * rhs, v2i * result);
void v2i_sub(const v2i * lhs, const v2i * rhs, v2i * result);
void v2i_div(const v2i * v, s32 sx, s32 sy, v2i * result);

s32 v2i_mag(const v2i * v);
b8 v2i_equal(const v2i * lhs, const v2i * rhs);

#endif
