#ifndef VIOLET_Matrix3_H
#define VIOLET_Matrix3_H

#include "violet/math/Vec2.h"

namespace Violet
{
	template<typename T>
	class Matrix3
	{
	public:

		class Column
		{
		public:

			T & operator[](size_t i);
			const T & operator[](size_t i) const;

		public:

			T m_entries[3];
		};

	public:

		static const Matrix3<T> Identity;
		static const Matrix3<T> Zero;

	public:

		Column & operator[](size_t i);
		Column const & operator[](size_t i) const;

		T * data();
		const T * data() const;

	public:

		Column m_rows[3];
	};

	template <typename T>
	Matrix3<T> operator*(const Matrix3<T> & lhs, const Matrix3<T> & rhs);

	template <typename T>
	Vec2<T> operator*(const Matrix3<T> & lhs, const Vec2<T> & rhs);
}

#include "violet/math/Matrix3.inl"

typedef Violet::Matrix3<int> Matrix3i;
typedef Violet::Matrix3<float> Matrix3f;

#endif
