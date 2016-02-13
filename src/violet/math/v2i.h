#ifndef VIOLET_V2I_H
#define VIOLET_V2I_H

#include "violet/core/defines.h"

namespace vlt
{
	struct VIOLET_API v2i
	{
		s32 x, y;

		v2i();
		v2i(const v2i & rhs);
		v2i(s32 _x, s32 _y);
	};
}

#endif
