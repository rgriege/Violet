#ifndef VIOLET_CIRCLE_H
#define VIOLET_CIRCLE_H

#include "violet/math/v2.h"
#include "violet/math/interval.h"

typedef struct circle
{
	v2 center;
	r32 radius;
} circle;

interval circle_project(const circle * c, const v2 * axis);

#endif

