#ifndef VEC2_H
#define VEC2_H

#include <string>

namespace Violet
{
	//class Matrix2;

	template<typename T>
	class Vec2
	{
	public:

		Vec2();
		Vec2(const Vec2<T> &other);
		Vec2(T _x, T _y);

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

		//void rotate(T radians);
		T dot(const Vec2<T> & other) const;
		Vec2<T> project(const Vec2<T> & axis) const;
		Vec2<T> perpendicular(bool left) const;
		Vec2<T> inverse() const;
		void invert();
		bool isZero() const;
		bool sharesQuadrant(const Vec2<T> & other) const;
		Vec2<T> copy() const;
		std::string toString() const;

	public:

		static const Vec2<T> X_AXIS, Y_AXIS;

	public:

		T x, y;
	};

	template<typename T>
	Vec2<T> operator+(Vec2<T> lhs, const Vec2<T> & rhs);

	template<typename T>
	Vec2<T> operator-(Vec2<T> lhs, const Vec2<T> & rhs);

	//Vec2<T> operator*(const Matrix2 & mat, const Vec2 & vec);

	template<typename T>
	Vec2<T> operator*(T scale, Vec2<T> vec);

	template<typename T>
	Vec2<T> operator*(Vec2<T> vec, T scale);

	template<typename T>
	Vec2<T> operator/(Vec2<T> vec, T scale);

	template<typename T>
	bool operator==(const Vec2<T> & lhs, const Vec2<T> & rhs);

	template<typename T>
	const Vec2<T> Vec2<T>::X_AXIS(1, 0);

	template<typename T>
	const Vec2<T> Vec2<T>::Y_AXIS(0, 1);

	template<typename T>
	Vec2<T>::Vec2() :
		x(0),
		y(0)
	{
	}

	template<typename T>
	Vec2<T>::Vec2(const Vec2<T> &other) :
		x(other.x),
		y(other.y)
	{
	}

	template<typename T>
	Vec2<T>::Vec2(T const _x, T const _y) :
		x(_x),
		y(_y)
	{
	}

	template<typename T>
	T Vec2<T>::magnitude() const
	{
		return sqrt(magSquared());
	}

	template<typename T>
	T Vec2<T>::magSquared() const
	{
		return x*x + y*y;
	}

	template<typename T>
	void Vec2<T>::normalize()
	{
		scale_equals(1/magnitude());
	}

	template<typename T>
	Vec2<T> Vec2<T>::getUnit() const
	{
		if (isUnit())
			return copy();
		else
			return *this/magnitude();
	}

	template<typename T>
	bool Vec2<T>::isUnit() const
	{
		return magSquared() == 1;
	}

	template<typename T>
	Vec2<T> & Vec2<T>::operator+=(const Vec2 & other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	template<typename T>
	Vec2<T> & Vec2<T>::operator-=(const Vec2 & other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template<typename T>
	Vec2<T> & Vec2<T>::operator*=(const T scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	template<typename T>
	Vec2<T> & Vec2<T>::operator/=(const T scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}

	template<typename T>
	Vec2<T> & Vec2<T>::operator=(const Vec2<T> & other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	/*template<typename T>
	void Vec2<T>::rotate(T radians) {
		Matrix2 mat(radians);
		//*this = mat*(*this);
		T old_x = x;
		x = x*mat.get_ul() + y*mat.get_ur();
		y = old_x*mat.get_dl() + y*mat.get_dr();
	}*/

	template<typename T>
	T Vec2<T>::dot(const Vec2<T> & other) const
	{
		return x*other.x + y*other.y;
	}

	template<typename T>
	Vec2<T> Vec2<T>::project(const Vec2<T> & other) const
	{
		Vec2 axis = other.get_unit_vector();
		return axis.scale(dot(other));
	}

	template<typename T>
	Vec2<T> Vec2<T>::perpendicular(const bool left) const
	{
		return left ? Vec2<T>(y, -x) : Vec2<T>(-y, x);
	}

	template<typename T>
	Vec2<T> Vec2<T>::inverse() const
	{
		return Vec2(-x, -y);
	}

	template<typename T>
	void Vec2<T>::invert()
	{
		x = -x;
		y = -y;
	}

	template<typename T>
	bool Vec2<T>::isZero() const
	{
		return x == 0 && y == 0;
	}

	template<typename T>
	bool Vec2<T>::sharesQuadrant(const Vec2 & other) const
	{
		return dot(other) >= 0;
	}

	template<typename T>
	Vec2<T> Vec2<T>::copy() const
	{
		return Vec2<T>(x, y);
	}

	template<typename T>
	std::string Vec2<T>::toString() const
	{
		stringstream ss;
		ss << "(" << x << "," << y << ")";
		return ss.str();
	}

	template<typename T>
	Vec2<T> operator+(Vec2<T> lhs, const Vec2<T> & rhs)
	{
		lhs += rhs;
		return lhs;
	}

	template<typename T>
	Vec2<T> operator-(Vec2<T> lhs, const Vec2<T> & rhs)
	{
		lhs -= rhs;
		return rhs;
	}

	template<typename T>
	Vec2<T> operator*(T scale, Vec2<T> rhs)
	{
		rhs *= scale;
		return rhs;
	}

	template<typename T>
	Vec2<T> operator*(Vec2<T> lhs, T scale)
	{
		lhs *= scale;
		return rhs;
	}

	template<typename T>
	Vec2<T> operator/(Vec2<T> vec, const T scale)
	{
		vec /= scale;
		return vec;
	}

	template<typename T>
	bool operator==(const Vec2<T> & lhs, const Vec2<T> & rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	typedef Vec2<int> Vec2i;
	typedef Vec2<float> Vec2f;
}

#endif