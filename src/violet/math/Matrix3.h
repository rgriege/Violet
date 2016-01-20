#ifndef VIOLET_Matrix3_H
#define VIOLET_Matrix3_H

#include "violet/math/Vec2.h"

namespace Violet
{
	template<typename T>
	class Matrix3
	{
	public:

		class Row
		{
		public:

			T & operator[](size_t i);
			const T & operator[](size_t i) const;

		public:

			T m_entries[3];
		};

	public:

		static Matrix3<T> createRotation(float radians);

	public:

		static const Matrix3<T> Identity;
		static const Matrix3<T> Zero;

	public:

		Row & operator[](size_t i);
		Row const & operator[](size_t i) const;

		T * data();
		const T * data() const;

	public:

		Row m_rows[3];
	};

	template <typename T>
	Matrix3<T> operator*(const Matrix3<T> & lhs, const Matrix3<T> & rhs);

	template <typename T>
	Vec2<T> operator*(const Matrix3<T> & lhs, const Vec2<T> & rhs);

	template <typename T>
	bool operator==(const Matrix3<T> & lhs, const Matrix3<T> & rhs);

	template <typename T>
	bool operator!=(const Matrix3<T> & lhs, const Matrix3<T> & rhs);



	template<typename T>
	Serializer & operator<<(Serializer & serializer, const Matrix3<T> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix3<int> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix3<uint32> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix3<float> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix3<double> & vec);

	template<typename T>
	Deserializer & operator>>(Deserializer & deserializer, Matrix3<T> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix3<int> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix3<uint32> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix3<float> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix3<double> & vec);
}

#include "violet/math/Matrix3.inl"

namespace Violet
{
    typedef Matrix3<int> Matrix3i;
    typedef Matrix3<float> Matrix3f;
	typedef Matrix3<double> Matrix3d;
}

#endif
