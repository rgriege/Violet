#include "violet/math/common.h"

u32 usub_capped(u32 lhs, u32 rhs)
{
	return lhs > rhs ? lhs - rhs : 0;
}

