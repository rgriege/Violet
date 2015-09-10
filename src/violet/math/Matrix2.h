#ifndef VIOLET_Matrix2_H
#define VIOLET_Matrix2_H

#include "violet/math/Vec2.h"

namespace Violet
{
	template<typename T>
	class Matrix2
	{
	public:

		static const Matrix2<T> Identity;
		static const Matrix2<T> Zero;

	public:

		Matrix2();
		Matrix2(T a, T b, T c, T d);
		explicit Matrix2(T rotation);

		Matrix2 inverse() const;

		T determinant() const;

		Matrix2 & operator/=(T scale);

	public:

		T a, b, c, d;
	};

	template <typename T>
	Vec2<T> operator*(const Matrix2<T> & mat, const Vec2<T> & vec);

	template <typename T>
	Matrix2<T> operator/(Matrix2<T> mat, const T scale);

	template <typename T>
	bool operator==(const Matrix2<T> & lhs, const Matrix2<T> & rhs);

	template <typename T>
	std::ostream & operator<<(std::ostream & os, const Matrix2<T> & mat);
}

#include "violet/math/Matrix2.inl"

typedef Violet::Matrix2<int> Matrix2i;
typedef Violet::Matrix2<float> Matrix2f;

#endif
