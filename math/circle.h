#ifndef VIOLET_CIRCLE_H
#define VIOLET_CIRCLE_H

#include "violet/math/v2f.h"
#include "violet/math/interval.h"

typedef struct circle
{
	v2f center;
	r32 radius;
} circle;

interval circle_project(const circle * c, const v2f * axis);

#endif

