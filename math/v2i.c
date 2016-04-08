#include <math.h>

#include "violet/math/v2i.h"

void v2i_add(const v2i * lhs, const v2i * rhs, v2i * result)
{
	result->x = lhs->x + rhs->x;
	result->y = lhs->y + rhs->y;
}

void v2i_sub(const v2i * lhs, const v2i * rhs, v2i * result)
{
	result->x = lhs->x - rhs->x;
	result->y = lhs->y - rhs->y;
}

b8 v2i_equal(const v2i * lhs, const v2i * rhs)
{
	return lhs->x == rhs->x && lhs->y == rhs->y;
}

