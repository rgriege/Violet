// ============================================================================

#include "violet/serialization/Deserializer.h"

#include <sstream>
#include <cmath>

// ============================================================================

template<typename T>
const Violet::Vec3<T> Violet::Vec3<T>::X_AXIS(1, 0, 0);

template<typename T>
const Violet::Vec3<T> Violet::Vec3<T>::Y_AXIS(0, 1, 0);

template<typename T>
const Violet::Vec3<T> Violet::Vec3<T>::Z_AXIS(0, 0, 1);

template<typename T>
const Violet::Vec3<T> Violet::Vec3<T>::ZERO;

// ============================================================================

template<typename T>
Violet::Vec3<T>::Vec3() :
	x(0),
	y(0),
	z(0)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T>::Vec3(const Vec3<T> &other) :
	x(other.x),
	y(other.y),
	z(other.z)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T>::Vec3(T const _x, T const _y, T const _z) :
	x(_x),
	y(_y),
	z(_z)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T>::Vec3(Deserializer & deserializer) :
	x(),
	y(),
	z()
{
	auto segment = deserializer.enterSegment("vec");
	x = segment->getFloat("x");
	y = segment->getFloat("y");
	z = segment->getFloat("z");
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec3<T>::magnitude() const
{
	return sqrt(magSquared());
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec3<T>::magSquared() const
{
	return x*x + y*y + z*z;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::normalize()
{
	*this /= magnitude();
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::Vec3<T>::getUnit() const
{
	if (isUnit())
		return copy();
	else
		return *this / magnitude();
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Vec3<T>::isUnit() const
{
	return magSquared() == 1;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::scale(const T scaleX, const T scaleY, const T scaleZ)
{
	x *= scaleX;
	y *= scaleY;
	z *= scaleZ;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::scale(const Vec3 & scales)
{
	return scale(scales.x, scales.y, scales.z);
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::operator+=(const Vec3 & other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::operator-=(const Vec3 & other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::operator*=(const T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::operator/=(const T scale)
{
	x /= scale;
	y /= scale;
	z /= scale;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> & Violet::Vec3<T>::operator=(const Vec3<T> & other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

// ----------------------------------------------------------------------------

/*template<typename T>
void Violet::Vec3<T>::rotate(T radians) {
	T oldX = x;
	T c = cos(radians);
	T s = sin(radians);
	x = x * c - y * s;
	y = oldX * s + y * c;
}*/

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Vec3<T>::dot(const Vec3<T> & other) const
{
	return x*other.x + y*other.y + z*other.z;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::Vec3<T>::cross(const Vec3<T> & other) const
{
	return Vec3(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::Vec3<T>::project(const Vec3<T> & other) const
{
	return other.getUnit() * dot(other);
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::Vec3<T>::operator-() const
{
	return inverse();
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::Vec3<T>::inverse() const
{
	return Vec3(-x, -y, -z);
}

// ----------------------------------------------------------------------------

template<typename T>
void Violet::Vec3<T>::invert()
{
	x = -x;
	y = -y;
	z = -z;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Vec3<T>::isZero() const
{
	return x == 0 && y == 0 && z == 0;
}

// ----------------------------------------------------------------------------

template<typename T>
void Violet::Vec3<T>::zero()
{
	x = y = z = 0;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::Vec3<T>::copy() const
{
	return Vec3<T>(x, y, z);
}

// ============================================================================

template<typename T>
Violet::Vec3<T> Violet::operator+(Vec3<T> lhs, const Vec3<T> & rhs)
{
	lhs += rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::operator-(Vec3<T> lhs, const Vec3<T> & rhs)
{
	lhs -= rhs;
	return lhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::operator*(T scale, Vec3<T> rhs)
{
	rhs *= scale;
	return rhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::operator*(Vec3<T> lhs, T scale)
{
	lhs *= scale;
	return lhs;
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Vec3<T> Violet::operator/(Vec3<T> vec, const T scale)
{
	vec /= scale;
	return vec;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::operator==(const Vec3<T> & lhs, const Vec3<T> & rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::operator!=(const Vec3<T> & lhs, const Vec3<T> & rhs)
{
	return !(lhs == rhs);
}

// ----------------------------------------------------------------------------

template<typename T>
std::ostream & Violet::operator<<(std::ostream & os, const Vec3<T> & vec)
{
	return os << "<" << vec.x << "," << vec.y << "," << vec.z << ">";
}

// ============================================================================
