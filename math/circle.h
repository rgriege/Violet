#ifndef VIOLET_CIRCLE_H
#define VIOLET_CIRCLE_H

#include "violet/math/v2.h"
#include "violet/math/interval.h"

typedef struct
{
	const v2 center;
	const r32 radius;

	Circle();
	Circle(const v2 & center, r32 radius);
	Interval project(const v2 & axis) const;
} circle;

#endif
