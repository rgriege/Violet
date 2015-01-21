#ifndef CIRCLE_H
#define CIRCLE_H

#include "violet/core/math/Vec2.h"
#include "violet/core/math/Interval.h"

#include <cmath>
#include <string>
#include <iostream>

namespace Violet
{
	class VIOLET_API Circle
	{
	public:

		Circle();
		Circle(Vec2f _center, float _radius);
		FloatInterval project(const Vec2f & axis) const;

	public:
	
		Vec2f const center;
		float const radius;
	};
}

#endif
