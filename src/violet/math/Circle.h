#ifndef VIOLET_Circle_H
#define VIOLET_Circle_H

#include "violet/math/Vec2.h"
#include "violet/math/Interval.h"

namespace Violet
{
	class VIOLET_API Circle
	{
	public:

		Circle();
		Circle(const Vec2f & center, float radius);
		FloatInterval project(const Vec2f & axis) const;

	public:
	
		const Vec2f m_center;
		const float m_radius;
	};
}

#endif
