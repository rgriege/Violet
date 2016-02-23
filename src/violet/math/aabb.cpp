// ============================================================================

#include <algorithm>

#include "violet/math/aabb.h"
#include "violet/math/poly.h"

using namespace vlt;

// ============================================================================

aabb aabb::from_line(const v2 & start, const v2 & end)
{
	r32 minX, maxX, minY, maxY;

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

	return aabb(minX, maxY, maxX, minY);
}

// ============================================================================

aabb::aabb() :
	top_left(),
	bottom_right()
{
}

// ----------------------------------------------------------------------------
	
aabb::aabb(const v2 & center, const v2 & half_dim) :
	top_left(center - half_dim),
	bottom_right(center + half_dim)
{
}

// ----------------------------------------------------------------------------

aabb::aabb(const r32 left, const r32 top, const r32 right, const r32 bottom) :
	top_left(left, top),
	bottom_right(right, bottom)
{
}

// ----------------------------------------------------------------------------

bool aabb::empty() const
{
	return top_left == bottom_right;
}

// ----------------------------------------------------------------------------

bool aabb::contains(const v2 & point) const
{
	return point.x >= top_left.x
		&& point.x <= bottom_right.x
		&& point.y <= top_left.y
		&& point.y >= bottom_right.y;
}

// ----------------------------------------------------------------------------

bool aabb::contains(const aabb & other) const
{
	return Interval(top_left.x, bottom_right.x).contains(Interval(other.top_left.x, other.bottom_right.x))
		&& Interval(top_left.y, bottom_right.y).contains(Interval(other.top_left.y, other.bottom_right.y));
}

// ----------------------------------------------------------------------------

bool aabb::overlaps(const aabb & other) const
{
	return Interval(top_left.x, bottom_right.x).overlaps(Interval(other.top_left.x, other.bottom_right.x))
		&& Interval(top_left.y, bottom_right.y).overlaps(Interval(other.top_left.y, other.bottom_right.y));
}

// ----------------------------------------------------------------------------

aabb & aabb::extend(const v2 & point)
{
	*this = aabb(std::min(top_left.x, point.x), std::max(top_left.y, point.y), std::max(bottom_right.x, point.x), std::min(bottom_right.y, point.y));
	return *this;
}

// ----------------------------------------------------------------------------

aabb & aabb::extend(const aabb & other)
{
	extend(other.top_left);
	extend(other.bottom_right);
	return *this;
}

// ----------------------------------------------------------------------------

aabb & aabb::translate(const v2 & offset)
{
	top_left += offset;
	bottom_right += offset;
	return *this;
}

// ----------------------------------------------------------------------------

aabb & aabb::transform(const m3 & transformation)
{
	*this = Poly{ {
			transformation * top_left,
			transformation * v2{ bottom_right.x, top_left.y },
			transformation * bottom_right,
			transformation * v2{ top_left.x, bottom_right.y }
		} }.get_bounding_box();

	return *this;
}

// ----------------------------------------------------------------------------

v2 aabb::get_center() const
{
	return v2((bottom_right.x - top_left.x) / 2.f, (top_left.y - bottom_right.y) / 2.f);
}

// ----------------------------------------------------------------------------

v2 aabb::get_half_dim() const
{
	return get_center() - top_left;
}

// ----------------------------------------------------------------------------

v2 aabb::get_min() const
{
	return v2(top_left.x, bottom_right.y);
}

// ----------------------------------------------------------------------------

v2 aabb::get_max() const
{
	return v2(bottom_right.x, top_left.y);
}

// ============================================================================
