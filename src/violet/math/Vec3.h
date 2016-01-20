#ifndef VIOLET_Vec3_H
#define VIOLET_Vec3_H

#include "violet/Defines.h"

#include <ostream>

namespace Violet
{
	class Deserializer;
	class Serializer;

	template<typename T>
	class Vec3
	{
	public:

		Vec3();
		Vec3(const Vec3<T> &other);
		Vec3(T _x, T _y, T _z);
		Vec3(Deserializer & deserializer);

		T magnitude() const;
		T magSquared() const;
		Vec3<T> & normalize();
		Vec3<T> getUnit() const;
		bool isUnit() const;
		Vec3<T> & scale(T scaleX, T scaleY, T scaleZ);
		Vec3<T> & scale(const Vec3<T> & scale);

		Vec3<T> & operator+=(const Vec3<T> & other);
		Vec3<T> & operator-=(const Vec3<T> & other);
		Vec3<T> & operator*=(T scale);
		Vec3<T> & operator/=(T scale);
		Vec3<T> & operator=(const Vec3<T> & other);

		// void rotate(T radians);
		T dot(const Vec3<T> & other) const;
		Vec3<T> cross(const Vec3<T> & other) const;
		Vec3<T> project(const Vec3<T> & axis) const;
		Vec3<T> operator-() const;
		Vec3<T> inverse() const;
		void invert();
		bool isZero() const;
		void zero();
		Vec3<T> copy() const;

	public:

		static const Vec3<T> X_AXIS;
		static const Vec3<T> Y_AXIS;
		static const Vec3<T> Z_AXIS;
		static const Vec3<T> ZERO;

	public:

		T x, y, z;
	};

	template<typename T>
	Vec3<T> operator+(Vec3<T> lhs, const Vec3<T> & rhs);

	template<typename T>
	Vec3<T> operator-(Vec3<T> lhs, const Vec3<T> & rhs);

	template<typename T>
	Vec3<T> operator*(T scale, Vec3<T> vec);

	template<typename T>
	Vec3<T> operator*(Vec3<T> vec, T scale);

	template<typename T>
	Vec3<T> operator/(Vec3<T> vec, T scale);

	template<typename T>
	bool operator==(const Vec3<T> & lhs, const Vec3<T> & rhs);

	template<typename T>
	bool operator!=(const Vec3<T> & lhs, const Vec3<T> & rhs);

	template<typename T>
	std::ostream & operator<<(std::ostream & os, const Vec3<T> & vec);


	template<typename T>
	Serializer & operator<<(Serializer & serializer, const Vec3<T> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec3<int> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec3<uint32> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec3<float> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Vec3<double> & vec);

	template<typename T>
	Deserializer & operator>>(Deserializer & deserializer, Vec3<T> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec3<int> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec3<uint32> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec3<float> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Vec3<double> & vec);
}

#include "violet/math/Vec3.inl"

typedef Violet::Vec3<int> Vec3i;
typedef Violet::Vec3<float> Vec3f;
typedef Violet::Vec3<double> Vec3d;

#endif