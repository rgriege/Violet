#ifndef MATRIX3_H
#define MATRIX3_H

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
}

#include "violet/core/math/Matrix3.ipp"

typedef Matrix3<int> Matrix3i;
typedef Matrix3<float> Matrix3f;

#endif
