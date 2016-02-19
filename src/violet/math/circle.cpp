// ============================================================================

#include "violet/math/circle.h"

using namespace vlt;

// ============================================================================

circle::circle() :
	center(),
	radius(0)
{
}

// ----------------------------------------------------------------------------

circle::circle(const v2 & center, const r32 radius) :
	center(center),
	radius(radius)
{
}

// ----------------------------------------------------------------------------

interval circle::project(const v2 & vec) const
{
	const v2 axis = vec.is_unit() ? vec : vec.get_unit();

	r32 center_proj = center.dot(axis);
	return interval(center_proj - radius, center_proj + radius);
}

// ============================================================================
