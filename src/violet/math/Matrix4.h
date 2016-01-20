#ifndef VIOLET_Matrix4_H
#define VIOLET_Matrix4_H

#include "violet/math/Matrix3.h"
#include "violet/math/Vec3.h"
#include "violet/template/TemplateUtilities.h"

#include <nmmintrin.h>

namespace Violet
{
	template <typename T>
	class alignas(16) Matrix4
	{
	public:

		class Column
		{
		public:

			T & operator[](size_t i);
			const T & operator[](size_t i) const;

			T * data();
			const T * data() const;

		public:

			T m_entries[4];
		};

	public:

		static const Matrix4<T> Identity;
		static const Matrix4<T> Zero;

	public:

		Matrix4();
		Matrix4(std::initializer_list<T> values);

		Column & operator[](size_t i);
		Column const & operator[](size_t i) const;

		T * data();
		const T * data() const;

	public:

		Column m_rows[4];
	};

	template <typename T>
	Matrix4<T> operator*(const Matrix4<T> & lhs, const Matrix4<T> & rhs);

	template <typename T>
	Vec3<T> operator*(const Matrix4<T> & lhs, const Vec3<T> & rhs);

	template <typename T>
	bool operator==(const Matrix4<T> & lhs, const Matrix4<T> & rhs);

	template <typename T>
	bool operator!=(const Matrix4<T> & lhs, const Matrix4<T> & rhs);

	template <typename T>
	Matrix4<T> from2d(const Matrix3<T> & mat);

	template <typename T>
	Matrix3<T> to2d(const Matrix4<T> & mat);



	template <typename T>
	class UnalignedMatrix4
	{
	public:

		explicit UnalignedMatrix4(const Matrix4<T> & mat);
		explicit operator Matrix4<T>() const;
		
	private:

		std::shared_ptr<T> m_data;
	};


	template<typename T>
	Serializer & operator<<(Serializer & serializer, const Matrix4<T> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix4<int> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix4<uint32> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix4<float> & vec);
	template<>
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Matrix4<double> & vec);

	template<typename T>
	Deserializer & operator>>(Deserializer & deserializer, Matrix4<T> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix4<int> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix4<uint32> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix4<float> & vec);
	template<>
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Matrix4<double> & vec);
}

#include "violet/math/Matrix4.inl"

namespace Violet
{
	typedef Matrix4<int> Matrix4i;
	typedef Matrix4<float> Matrix4f;
	typedef Matrix4<double> Matrix4d;

	typedef UnalignedMatrix4<int> UMatrix4i;
	typedef UnalignedMatrix4<float> UMatrix4f;
	typedef UnalignedMatrix4<double> UMatrix4d;


	template <typename T>
	struct copyable<Matrix4<T>>
	{
		typedef UnalignedMatrix4<T> type;
	};
}

#endif
