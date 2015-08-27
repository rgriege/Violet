// ============================================================================

#include "engine/math/AABB.h"

#include <algorithm>

using namespace Violet;

// ============================================================================

AABB AABB::createFromLine(const Vec2f & start, const Vec2f & end)
{
	float minX, maxX, minY, maxY;

	if (start.x < end.x)
	{
		minX = start.x;
		maxX = end.x;
	}
	else
	{
		minX = end.x;
		maxX = start.x;
	}

	if (start.y < end.y)
	{
		minY = start.y;
		maxY = end.y;
	}
	else
	{
		minY = end.y;
		maxY = start.y;
	}

	return AABB(minX, maxY, maxX, minY);
}

// ============================================================================

AABB::AABB() :
	m_topLeft(),
	m_bottomRight()
{
}

// ----------------------------------------------------------------------------
	
AABB::AABB(const Vec2f & center, const Vec2f & halfDim) :
	m_topLeft(center - halfDim),
	m_bottomRight(center + halfDim)
{
}

// ----------------------------------------------------------------------------

AABB::AABB(const float left, const float top, const float right, const float bottom) :
	m_topLeft(left, top),
	m_bottomRight(right, bottom)
{
}

// ----------------------------------------------------------------------------

bool AABB::contains(const Vec2f & point) const
{
	return point.x >= m_topLeft.x
		&& point.x <= m_bottomRight.x
		&& point.y <= m_topLeft.y
		&& point.y >= m_bottomRight.y;
}

// ----------------------------------------------------------------------------

bool AABB::contains(const AABB & other) const
{
	return FloatInterval(m_topLeft.x, m_bottomRight.x).contains(FloatInterval(other.m_topLeft.x, other.m_bottomRight.x))
		&& FloatInterval(m_topLeft.y, m_bottomRight.y).contains(FloatInterval(other.m_topLeft.y, other.m_bottomRight.y));
}

// ----------------------------------------------------------------------------

bool AABB::overlaps(const AABB & other) const
{
	return FloatInterval(m_topLeft.x, m_bottomRight.x).overlaps(FloatInterval(other.m_topLeft.x, other.m_bottomRight.x))
		&& FloatInterval(m_topLeft.y, m_bottomRight.y).overlaps(FloatInterval(other.m_topLeft.y, other.m_bottomRight.y));
}

// ----------------------------------------------------------------------------

AABB & AABB::extend(const Vec2f & point)
{
	*this = AABB(std::min(m_topLeft.x, point.x), std::max(m_topLeft.y, point.y), std::max(m_bottomRight.x, point.x), std::min(m_bottomRight.y, point.y));
	return *this;
}

// ----------------------------------------------------------------------------

AABB & AABB::extend(const AABB & other)
{
	extend(other.m_topLeft);
	extend(other.m_bottomRight);
	return *this;
}

// ----------------------------------------------------------------------------

AABB & AABB::translate(const Vec2f & offset)
{
	m_topLeft += offset;
	m_bottomRight += offset;
	return *this;
}

// ----------------------------------------------------------------------------

AABB & AABB::transform(const Matrix3f & transformation)
{
	m_topLeft = transformation * m_topLeft;
	m_bottomRight = transformation * m_bottomRight;
	return *this;
}

// ----------------------------------------------------------------------------

Vec2f AABB::getCenter() const
{
	return Vec2f((m_bottomRight.x - m_topLeft.x) / 2.f, (m_topLeft.y - m_bottomRight.y) / 2.f);
}

// ----------------------------------------------------------------------------

Vec2f AABB::getHalfDim() const
{
	return getCenter() - m_topLeft;
}

// ----------------------------------------------------------------------------

Vec2f AABB::getMinimum() const
{
	return Vec2f(m_topLeft.x, m_bottomRight.y);
}

// ----------------------------------------------------------------------------

Vec2f AABB::getMaximum() const
{
	return Vec2f(m_bottomRight.x, m_topLeft.y);
}

// ============================================================================
