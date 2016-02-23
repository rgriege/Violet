// ============================================================================

#include "violet/math/circle.h"

using namespace vlt;

// ============================================================================

Circle::Circle() :
	center(),
	radius(0)
{
}

// ----------------------------------------------------------------------------

Circle::Circle(const v2 & _center, const r32 _radius) :
	center(_center),
	radius(_radius)
{
}

// ----------------------------------------------------------------------------

Interval Circle::project(const v2 & vec) const
{
	const v2 axis = vec.is_unit() ? vec : vec.get_unit();

	r32 center_proj = center.dot(axis);
	return Interval(center_proj - radius, center_proj + radius);
}

// ============================================================================
