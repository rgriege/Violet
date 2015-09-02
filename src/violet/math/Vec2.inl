// ============================================================================

#include "violet/serialization/Deserializer.h"

#include <sstream>
#include <cmath>

// ============================================================================

template<typename T>
const Violet::Vec2<T> Violet::Vec2<T>::X_AXIS(1, 0);

template<typename T>
const Violet::Vec2<T> Violet::Vec2<T>::Y_AXIS(0, 1);

template<typename T>
const Violet::Vec2<T> Violet::Vec2<T>::ZERO;

// ============================================================================

template<typename T>
Violet::Vec2<T>::Vec2() :
	x(0),
	y(0)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T>::Vec2(const Vec2<T> &other) :
	x(other.x),
	y(other.y)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T>::Vec2(T const _x, T const _y) :
	x(_x),
	y(_y)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T>::Vec2(Deserializer & deserializer) :
	x(),
	y()
{
	auto segment = deserializer.enterSegment("vec");
	x = segment->getFloat("x");
	y = segment->getFloat("y");
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec2<T>::magnitude() const
{
	return sqrt(magSquared());
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec2<T>::magSquared() const
{
	return x*x + y*y;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> & Violet::Vec2<T>::normalize()
{
	*this /= magnitude();
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::Vec2<T>::getUnit() const
{
	if (isUnit())
		return copy();
	else
		return *this / magnitude();
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Vec2<T>::isUnit() const
{
	return magSquared() == 1;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> & Violet::Vec2<T>::operator+=(const Vec2 & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> & Violet::Vec2<T>::operator-=(const Vec2 & other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> & Violet::Vec2<T>::operator*=(const T scale)
{
	x *= scale;
	y *= scale;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> & Violet::Vec2<T>::operator/=(const T scale)
{
	x /= scale;
	y /= scale;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> & Violet::Vec2<T>::operator=(const Vec2<T> & other)
{
	x = other.x;
	y = other.y;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
void Violet::Vec2<T>::rotate(T radians) {
	T oldX = x;
	T c = cos(radians);
	T s = sin(radians);
	x = x * c - y * s;
	y = oldX * s + y * c;
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec2<T>::dot(const Vec2<T> & other) const
{
	return x*other.x + y*other.y;
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec2<T>::cross(const Vec2<T> & other) const
{
	return x*other.y - y*other.x;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::Vec2<T>::project(const Vec2<T> & other) const
{
	return other.getUnit() * dot(other);
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::Vec2<T>::perpendicular(const bool left) const
{
	return left ? Vec2<T>(y, -x) : Vec2<T>(-y, x);
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::Vec2<T>::operator-() const
{
	return inverse();
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::Vec2<T>::inverse() const
{
	return Vec2(-x, -y);
}

// ----------------------------------------------------------------------------

template<typename T>
void Violet::Vec2<T>::invert()
{
	x = -x;
	y = -y;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Vec2<T>::isZero() const
{
	return x == 0 && y == 0;
}

// ----------------------------------------------------------------------------

template<typename T>
void Violet::Vec2<T>::zero()
{
	x = y = 0;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Vec2<T>::sharesQuadrant(const Vec2 & other) const
{
	return dot(other) >= 0;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::Vec2<T>::copy() const
{
	return Vec2<T>(x, y);
}

// ============================================================================

template<typename T>
Violet::Vec2<T> Violet::operator+(Vec2<T> lhs, const Vec2<T> & rhs)
{
	lhs += rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::operator-(Vec2<T> lhs, const Vec2<T> & rhs)
{
	lhs -= rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::operator*(T scale, Vec2<T> rhs)
{
	rhs *= scale;
	return rhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::operator*(Vec2<T> lhs, T scale)
{
	lhs *= scale;
	return lhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec2<T> Violet::operator/(Vec2<T> vec, const T scale)
{
	vec /= scale;
	return vec;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::operator==(const Vec2<T> & lhs, const Vec2<T> & rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::operator!=(const Vec2<T> & lhs, const Vec2<T> & rhs)
{
	return !(lhs == rhs);
}

// ----------------------------------------------------------------------------

template<typename T>
std::ostream & Violet::operator<<(std::ostream & os, const Vec2<T> & vec)
{
	return os << "<" << vec.x << "," << vec.y << ">";
}

// ============================================================================
