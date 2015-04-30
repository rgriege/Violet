#include "engine/math/Circle.h"

#include <string>
#include <cmath>

using namespace Violet;

Circle::Circle() :
	radius(0)
{
}

Circle::Circle(Vec2f const _center, float const _radius) :
	center(_center),
	radius(_radius)
{
}

FloatInterval Circle::project(const Vec2f & vec) const
{
	const Vec2f axis = vec.isUnit() ? vec : vec.getUnit();

	float center_proj = center.dot(axis);
	return FloatInterval(center_proj - radius, center_proj + radius);
}
