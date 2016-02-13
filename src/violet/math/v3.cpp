// ============================================================================

#include <sstream>
#include <cmath>

#include "violet/math/v3.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const v3 v3::X_Axis(1, 0, 0);
const v3 v3::Y_Axis(0, 1, 0);
const v3 v3::Z_Axis(0, 0, 1);
const v3 v3::Zero;

// ============================================================================

v3::v3() :
	x(0),
	y(0),
	z(0)
{
}

// ----------------------------------------------------------------------------

v3::v3(const v3 & rhs) :
	x(rhs.x),
	y(rhs.y),
	z(rhs.z)
{
}

// ----------------------------------------------------------------------------

v3::v3(const r32 _x, const r32 _y, const r32 _z) :
	x(_x),
	y(_y),
	z(_z)
{
}

// ----------------------------------------------------------------------------

v3::v3(deserializer & deserializer) :
	x(),
	y(),
	z()
{
	auto segment = deserializer.enter_segment("vec");
	x = segment->get_r32("x");
	y = segment->get_r32("y");
	z = segment->get_r32("z");
}

// ----------------------------------------------------------------------------

r32 v3::magnitude() const
{
	return sqrt(mag_squared());
}

// ----------------------------------------------------------------------------

r32 v3::mag_squared() const
{
	return x*x + y*y + z*z;
}

// ----------------------------------------------------------------------------

v3 & v3::normalize()
{
	*this /= magnitude();
	return *this;
}

// ----------------------------------------------------------------------------

v3 v3::get_unit() const
{
	if (is_unit())
		return copy();
	else
		return *this / magnitude();
}

// ----------------------------------------------------------------------------

bool v3::is_unit() const
{
	return mag_squared() == 1;
}

// ----------------------------------------------------------------------------

v3 & v3::scale(const r32 sx, const r32 sy, const r32 sz)
{
	x *= sx;
	y *= sy;
	z *= sz;
	return *this;
}

// ----------------------------------------------------------------------------

v3 & v3::scale(const v3 & s)
{
	return scale(s.x, s.y, s.z);
}

// ----------------------------------------------------------------------------

v3 & v3::operator+=(const v3 & rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

// ----------------------------------------------------------------------------

v3 & v3::operator-=(const v3 & rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

// ----------------------------------------------------------------------------

v3 & v3::operator*=(const r32 scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

// ----------------------------------------------------------------------------

v3 & v3::operator/=(const r32 scale)
{
	x /= scale;
	y /= scale;
	z /= scale;
	return *this;
}

// ----------------------------------------------------------------------------

v3 & v3::operator=(const v3 & rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

// ----------------------------------------------------------------------------

r32 v3::dot(const v3 & rhs) const
{
	return x*rhs.x + y*rhs.y + z*rhs.z;
}

// ----------------------------------------------------------------------------

v3 v3::cross(const v3 & rhs) const
{
	return v3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
}

// ----------------------------------------------------------------------------

v3 v3::project(const v3 & rhs) const
{
	return rhs.get_unit() * dot(rhs);
}

// ----------------------------------------------------------------------------

v3 v3::operator-() const
{
	return inverse();
}

// ----------------------------------------------------------------------------

v3 v3::inverse() const
{
	return v3(-x, -y, -z);
}

// ----------------------------------------------------------------------------

void v3::invert()
{
	x = -x;
	y = -y;
	z = -z;
}

// ----------------------------------------------------------------------------

bool v3::is_zero() const
{
	return x == 0 && y == 0 && z == 0;
}

// ----------------------------------------------------------------------------

void v3::zero()
{
	x = y = z = 0;
}

// ----------------------------------------------------------------------------

v3 v3::copy() const
{
	return v3(x, y, z);
}

// ============================================================================

v3 vlt::operator+(v3 lhs, const v3 & rhs)
{
	lhs += rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

v3 vlt::operator-(v3 lhs, const v3 & rhs)
{
	lhs -= rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

v3 vlt::operator*(r32 scale, v3 rhs)
{
	rhs *= scale;
	return rhs;
}

// ----------------------------------------------------------------------------

v3 vlt::operator*(v3 lhs, r32 scale)
{
	lhs *= scale;
	return lhs;
}

// ----------------------------------------------------------------------------

v3 vlt::operator/(v3 vec, const r32 scale)
{
	vec /= scale;
	return vec;
}

// ----------------------------------------------------------------------------

bool vlt::operator==(const v3 & lhs, const v3 & rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

// ----------------------------------------------------------------------------

bool vlt::operator!=(const v3 & lhs, const v3 & rhs)
{
	return !(lhs == rhs);
}

// ----------------------------------------------------------------------------

std::ostream & vlt::operator<<(std::ostream & os, const v3 & vec)
{
	return os << "<" << vec.x << "," << vec.y << "," << vec.z << ">";
}

// ============================================================================

serializer & vlt::operator<<(serializer & serializer, const v3 & vec)
{
	auto segment = serializer.create_segment("vec");
	segment->write_r32("x", vec.x);
	segment->write_r32("y", vec.y);
	segment->write_r32("z", vec.z);
	return serializer;
}

// ----------------------------------------------------------------------------

deserializer & vlt::operator>>(deserializer & deserializer, v3 & vec)
{
	auto segment = deserializer.enter_segment("vec");
	vec.x = segment->get_r32("x");
	vec.y = segment->get_r32("y");
	vec.z = segment->get_r32("z");
	return deserializer;
}

// ============================================================================
