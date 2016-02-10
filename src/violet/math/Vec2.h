#ifndef VIOLET_Vec2_H
#define VIOLET_Vec2_H

#include "violet/Defines.h"

#include <ostream>

namespace Violet
{
	class Deserializer;
	class Serializer;

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
		Vec2<T> & normalize();
		Vec2<T> getUnit() const;
		bool isUnit() const;
		Vec2<T> & scale(T scaleX, T scaleY);
		Vec2<T> & scale(const Vec2<T> & scale);

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
		Vec2<T> operator-() const;
		Vec2<T> inverse() const;
		void invert();
		bool isZero() const;
		void zero();
		bool sharesQuadrant(const Vec2<T> & other) const;
		Vec2<T> copy() const;

	public:

		static const Vec2<T> X_AXIS;
		static const Vec2<T> Y_AXIS;
		static const Vec2<T> ZERO;

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


	template<typename T>
	Serializer & operator<<(Serializer & serializer, const Vec2<T> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec2<int> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec2<uint32> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec2<float> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec2<double> & vec);

	template<typename T>
	Deserializer & operator>>(Deserializer & deserializer, Vec2<T> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec2<int> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec2<uint32> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec2<float> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec2<double> & vec);
}

#include "violet/math/Vec2.inl"

namespace Violet
{
	typedef Vec2<int> Vec2i;
	typedef Vec2<float> Vec2f;
	typedef Vec2<double> Vec2d;
}

#endif
