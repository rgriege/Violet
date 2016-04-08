#ifndef VIOLET_V2I_H
#define VIOLET_V2I_H

#include "violet/core/types.h"

typedef struct v2i
{
	s32 x, y;
} v2i;

void v2i_add(const v2i * lhs, const v2i * rhs, v2i * result);
void v2i_sub(const v2i * lhs, const v2i * rhs, v2i * result);

b8 v2i_equal(const v2i * lhs, const v2i * rhs);

#endif
