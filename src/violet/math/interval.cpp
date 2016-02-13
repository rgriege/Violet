// ============================================================================

#include <algorithm>
#include <iostream>
#include <cmath>

#include "violet/math/interval.h"

using namespace vlt;

// ============================================================================

interval::interval() :
	left(0),
	right(0)
{
}

// ----------------------------------------------------------------------------

interval::interval(const r32 left, const r32 right) :
	left(left),
	right(right)
{
}

// ----------------------------------------------------------------------------

interval::interval(const interval & other) :
	left(other.left),
	right(other.right)
{
}

// ----------------------------------------------------------------------------

interval & interval::slide(const r32 delta)
{
	left += delta;
	right += delta;
	return *this;
}

// ----------------------------------------------------------------------------

r32 interval::length() const
{
	return right - left;
}

// ----------------------------------------------------------------------------

bool interval::contains(const r32 x) const
{
	return x >= left && x <= right;
}

// ----------------------------------------------------------------------------

bool interval::contains(const interval & other) const
{
	return contains(other.left) && contains(other.right);
}

// ----------------------------------------------------------------------------

bool interval::overlaps(const interval & other) const
{
	return contains(other.left) || other.contains(left);
}

// ----------------------------------------------------------------------------

r32 interval::overlap(const interval &  other) const
{
	r32 diff1 = right - other.left;
	r32 diff2 = other.right - left;
	return diff1 < 0 || diff2 < 0 ? 0 : std::min(std::min(diff1, diff2), std::min(length(), other.length()));
}

// ============================================================================

std::ostream & vlt::operator<<(std::ostream & os, const interval & interval)
{
	return os << "[" << interval.left << "," << interval.right << "]";
}

// ============================================================================
