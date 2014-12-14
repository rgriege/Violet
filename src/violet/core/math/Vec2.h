#ifndef VEC2_H
#define VEC2_H

#include <ostream>

namespace Violet
{
	class Deserializer;

	template<typename T>
	class Vec2
	{
	public:

		Vec2();
		Vec2(const Vec2<T> &other);
		Vec2(T _x, T _y);
		Vec2(Deserializer & deserializer);

		T magnitude() const;
		T magSquared() const;
		void normalize();
		Vec2<T> getUnit() const;
		bool isUnit() const;

		Vec2<T> & operator+=(const Vec2<T> & other);
		Vec2<T> & operator-=(const Vec2<T> & other);
		Vec2<T> & operator*=(T scale);
		Vec2<T> & operator/=(T scale);
		Vec2<T> & operator=(const Vec2<T> & other);

		void rotate(T radians);
		T dot(const Vec2<T> & other) const;
		T cross(const Vec2<T> & other) const;
		Vec2<T> project(const Vec2<T> & axis) const;
		Vec2<T> perpendicular(bool left = true) const;
		Vec2<T> inverse() const;
		void invert();
		bool isZero() const;
		void zero();
		bool sharesQuadrant(const Vec2<T> & other) const;
		Vec2<T> copy() const;

	public:

		static const Vec2<T> X_AXIS, Y_AXIS, ZERO;

	public:

		T x, y;
	};

	template<typename T>
	Vec2<T> operator+(Vec2<T> lhs, const Vec2<T> & rhs);

	template<typename T>
	Vec2<T> operator-(Vec2<T> lhs, const Vec2<T> & rhs);

	template<typename T>
	Vec2<T> operator*(T scale, Vec2<T> vec);

	template<typename T>
	Vec2<T> operator*(Vec2<T> vec, T scale);

	template<typename T>
	Vec2<T> operator/(Vec2<T> vec, T scale);

	template<typename T>
	bool operator==(const Vec2<T> & lhs, const Vec2<T> & rhs);

	template<typename T>
	bool operator!=(const Vec2<T> & lhs, const Vec2<T> & rhs);

	template<typename T>
	std::ostream & operator<<(std::ostream & os, const Vec2<T> & vec);
}

#include "violet/core/math/Vec2.ipp"

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;

#endif