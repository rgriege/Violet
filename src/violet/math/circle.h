#ifndef VIOLET_CIRCLE_H
#define VIOLET_CIRCLE_H

#include "violet/math/v2.h"
#include "violet/math/interval.h"

namespace vlt
{
	struct VIOLET_API circle
	{
		const v2 center;
		const r32 radius;

		circle();
		circle(const v2 & center, r32 radius);
		interval project(const v2 & axis) const;
	};
}

#endif
