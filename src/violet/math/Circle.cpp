// ============================================================================

#include "violet/math/Circle.h"

using namespace Violet;

// ============================================================================

Circle::Circle() :
	m_center(),
	m_radius(0)
{
}

// ----------------------------------------------------------------------------

Circle::Circle(const Vec2f & center, const float radius) :
	m_center(center),
	m_radius(radius)
{
}

// ----------------------------------------------------------------------------

FloatInterval Circle::project(const Vec2f & vec) const
{
	const Vec2f axis = vec.isUnit() ? vec : vec.getUnit();

	float center_proj = m_center.dot(axis);
	return FloatInterval(center_proj - m_radius, center_proj + m_radius);
}

// ============================================================================
