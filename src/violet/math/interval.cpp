// ============================================================================

#include <algorithm>
#include <iostream>
#include <cmath>

#include "violet/math/interval.h"

using namespace vlt;

// ============================================================================

Interval::Interval() :
	left(0),
	right(0)
{
}

// ----------------------------------------------------------------------------

Interval::Interval(const r32 left, const r32 right) :
	left(left),
	right(right)
{
}

// ----------------------------------------------------------------------------

Interval::Interval(const Interval & other) :
	left(other.left),
	right(other.right)
{
}

// ----------------------------------------------------------------------------

Interval & Interval::slide(const r32 delta)
{
	left += delta;
	right += delta;
	return *this;
}

// ----------------------------------------------------------------------------

r32 Interval::length() const
{
	return right - left;
}

// ----------------------------------------------------------------------------

bool Interval::contains(const r32 x) const
{
	return x >= left && x <= right;
}

// ----------------------------------------------------------------------------

bool Interval::contains(const Interval & other) const
{
	return contains(other.left) && contains(other.right);
}

// ----------------------------------------------------------------------------

bool Interval::overlaps(const Interval & other) const
{
	return contains(other.left) || other.contains(left);
}

// ----------------------------------------------------------------------------

r32 Interval::overlap(const Interval &  other) const
{
	r32 diff1 = right - other.left;
	r32 diff2 = other.right - left;
	return diff1 < 0 || diff2 < 0 ? 0 : std::min(std::min(diff1, diff2), std::min(length(), other.length()));
}

// ============================================================================

std::ostream & vlt::operator<<(std::ostream & os, const Interval & Interval)
{
	return os << "[" << Interval.left << "," << Interval.right << "]";
}

// ============================================================================
