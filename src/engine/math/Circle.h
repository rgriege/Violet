#ifndef CIRCLE_H
#define CIRCLE_H

#include "engine/Defines.h"
#include "engine/math/Vec2.h"
#include "engine/math/Interval.h"

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
