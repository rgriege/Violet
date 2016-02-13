// ============================================================================

#include <cmath>
#include <sstream>

#include "violet/math/v2.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const v2 v2::X_Axis(1, 0);
const v2 v2::Y_Axis(0, 1);
const v2 v2::Zero;

// ============================================================================

v2::v2() :
	x(0),
	y(0)
{
}

// ----------------------------------------------------------------------------

v2::v2(const v2 & rhs) :
	x(rhs.x),
	y(rhs.y)
{
}

// ----------------------------------------------------------------------------


v2::v2(const r32 _x, const r32 _y) :
	x(_x),
	y(_y)
{
}

// ----------------------------------------------------------------------------


v2::v2(deserializer & deserializer) :
	x(),
	y()
{
	auto segment = deserializer.enter_segment("vec");
	x = segment->get_r32("x");
	y = segment->get_r32("y");
}

// ----------------------------------------------------------------------------


r32 v2::magnitude() const
{
	return sqrt(mag_squared());
}

// ----------------------------------------------------------------------------


r32 v2::mag_squared() const
{
	return x*x + y*y;
}

// ----------------------------------------------------------------------------


v2 & v2::normalize()
{
	*this /= magnitude();
	return *this;
}

// ----------------------------------------------------------------------------


v2 v2::get_unit() const
{
	if (is_unit())
		return copy();
	else
		return *this / magnitude();
}

// ----------------------------------------------------------------------------


bool v2::is_unit() const
{
	return mag_squared() == 1;
}

// ----------------------------------------------------------------------------


v2 & v2::scale(const r32 sx, const r32 sy)
{
	x *= sx;
	y *= sy;
	return *this;
}

// ----------------------------------------------------------------------------


v2 & v2::scale(const v2 & s)
{
	return scale(s.x, s.y);
}

// ----------------------------------------------------------------------------


v2 & v2::operator+=(const v2 & rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

// ----------------------------------------------------------------------------


v2 & v2::operator-=(const v2 & rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

// ----------------------------------------------------------------------------


v2 & v2::operator*=(const r32 scale)
{
	x *= scale;
	y *= scale;
	return *this;
}

// ----------------------------------------------------------------------------


v2 & v2::operator/=(const r32 scale)
{
	x /= scale;
	y /= scale;
	return *this;
}

// ----------------------------------------------------------------------------


v2 & v2::operator=(const v2 & rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

// ----------------------------------------------------------------------------


void v2::rotate(const r32 radians) {
	const r32 oldX = x;
	const r32 c = cos(radians);
	const r32 s = sin(radians);
	x = x * c - y * s;
	y = oldX * s + y * c;
}

// ----------------------------------------------------------------------------


r32 v2::dot(const v2 & rhs) const
{
	return x*rhs.x + y*rhs.y;
}

// ----------------------------------------------------------------------------


r32 v2::cross(const v2 & rhs) const
{
	return x*rhs.y - y*rhs.x;
}

// ----------------------------------------------------------------------------


v2 v2::project(const v2 & rhs) const
{
	return rhs.get_unit() * dot(rhs);
}

// ----------------------------------------------------------------------------


v2 v2::perpendicular(const bool left) const
{
	return left ? v2(y, -x) : v2(-y, x);
}

// ----------------------------------------------------------------------------


v2 v2::operator-() const
{
	return inverse();
}

// ----------------------------------------------------------------------------


v2 v2::inverse() const
{
	return v2(-x, -y);
}

// ----------------------------------------------------------------------------


void v2::invert()
{
	x = -x;
	y = -y;
}

// ----------------------------------------------------------------------------


bool v2::is_zero() const
{
	return x == 0 && y == 0;
}

// ----------------------------------------------------------------------------


void v2::zero()
{
	x = y = 0;
}

// ----------------------------------------------------------------------------


bool v2::shares_quadrant(const v2 & rhs) const
{
	return dot(rhs) >= 0;
}

// ----------------------------------------------------------------------------


v2 v2::copy() const
{
	return v2(x, y);
}

// ============================================================================

v2 vlt::operator+(v2 lhs, const v2 & rhs)
{
	lhs += rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

v2 vlt::operator-(v2 lhs, const v2 & rhs)
{
	lhs -= rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

v2 vlt::operator*(r32 scale, v2 rhs)
{
	rhs *= scale;
	return rhs;
}

// ----------------------------------------------------------------------------

v2 vlt::operator*(v2 lhs, r32 scale)
{
	lhs *= scale;
	return lhs;
}

// ----------------------------------------------------------------------------

v2 vlt::operator/(v2 vec, const r32 scale)
{
	vec /= scale;
	return vec;
}

// ----------------------------------------------------------------------------

bool vlt::operator==(const v2 & lhs, const v2 & rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

// ----------------------------------------------------------------------------

bool vlt::operator!=(const v2 & lhs, const v2 & rhs)
{
	return !(lhs == rhs);
}

// ----------------------------------------------------------------------------

std::ostream & vlt::operator<<(std::ostream & os, const v2 & vec)
{
	return os << "<" << vec.x << "," << vec.y << ">";
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const v2 & vec)
{
	auto segment = serializer.create_segment("vec");
	segment->write_r32("x", vec.x);
	segment->write_r32("y", vec.y);
	return serializer;
}

// ----------------------------------------------------------------------------

deserializer & vlt::operator>>(deserializer & deserializer, v2 & vec)
{
	auto segment = deserializer.enter_segment("vec");
	vec.x = segment->get_r32("x");
	vec.y = segment->get_r32("y");
	return deserializer;
}

// ============================================================================
