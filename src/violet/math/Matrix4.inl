// ============================================================================

#include <assert.h>
#include <cstring>
#include <nmmintrin.h>

// ============================================================================

template <typename T>
T & Violet::Matrix4<T>::Column::operator[](const size_t i)
{
	return m_entries[i];
}

// ----------------------------------------------------------------------------

template <typename T>
const T & Violet::Matrix4<T>::Column::operator[](const size_t i) const
{
	return m_entries[i];
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::Matrix4<T>::Column::data()
{
	return &m_entries[0];
}

// ----------------------------------------------------------------------------

template <typename T>
const T * Violet::Matrix4<T>::Column::data() const
{
	return &m_entries[0];
}

// ============================================================================

template <typename T>
const Violet::Matrix4<T> Violet::Matrix4<T>::Identity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

// ----------------------------------------------------------------------------

template <typename T>
const Violet::Matrix4<T> Violet::Matrix4<T>::Zero = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

// ============================================================================

template <typename T>
Violet::Matrix4<T>::Matrix4() :
	m_rows()
{
	std::memset(data(), 0, 16 * sizeof(T));
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::Matrix4<T>::Matrix4(std::initializer_list<T> values) :
	m_rows()
{
	assert(values.size() == 16);
	std::memcpy(data(), values.begin(), 16 * sizeof(T));
}

// ----------------------------------------------------------------------------

template <typename T>
typename Violet::Matrix4<T>::Column & Violet::Matrix4<T>::operator[](const size_t i)
{
	return m_rows[i];
}

// ----------------------------------------------------------------------------

template <typename T>
typename Violet::Matrix4<T>::Column const & Violet::Matrix4<T>::operator[](const size_t i) const
{
	return m_rows[i];
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::Matrix4<T>::data()
{
	return &m_rows[0][0];
}

// ----------------------------------------------------------------------------

template <typename T>
const T * Violet::Matrix4<T>::data() const
{
	return &m_rows[0][0];
}

// ============================================================================

template <typename T>
Violet::Matrix4<T> Violet::operator*(const Matrix4<T> & lhs, const Matrix4<T> & rhs)
{
	Matrix4<T> result = Matrix4<T>::Zero;
	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 4; ++k)
		{
			T r = lhs[i][k];
			__m128 rVec = _mm_set_ps(r, r, r, r);

			__m128 rhsVec = _mm_load_ps(rhs[k].data());
			__m128 resultVec = _mm_load_ps(result[i].data());
			__m128 mul = _mm_mul_ps(rVec, rhsVec);
			__m128 add = _mm_add_ps(resultVec, mul);
			_mm_store_ps(result[i].data(), add);
		}
	}

	return result;
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::Vec3<T> Violet::operator*(const Matrix4<T> & lhs, const Vec3<T> & rhs)
{
	return Vec3<T>(lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2], lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2]);
}

// ----------------------------------------------------------------------------

template <typename T>
bool Violet::operator==(const Matrix4<T> & lhs, const Matrix4<T> & rhs)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (lhs[i][j] != rhs[i][j])
				return false;
	return true;
}

// ----------------------------------------------------------------------------

template <typename T>
bool Violet::operator!=(const Matrix4<T> & lhs, const Matrix4<T> & rhs)
{
	return !(lhs == rhs);
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::Matrix4<T> Violet::from2d(const Matrix3<T> & mat)
{
	return Matrix4<T> {
		mat[0][0], mat[0][1], 0, mat[0][2],
		mat[1][0], mat[1][1], 0, mat[1][2],
				0,		 0, 1,		 0,
		mat[2][0], mat[2][1], 0, mat[2][2]
	};
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::Matrix3<T> Violet::to2d(const Matrix4<T> & mat)
{
	return Matrix3<T> {
		mat[0][0], mat[0][1], mat[0][3],
		mat[1][0], mat[1][1], mat[1][3],
		mat[3][0], mat[3][1], mat[3][3]
	};
}

// ============================================================================

template <typename T>
Violet::UnalignedMatrix4<T>::UnalignedMatrix4(const Matrix4<T> & mat) :
	m_data(new T[16], std::default_delete<T[]>())
{
	memcpy(m_data.get(), mat.data(), 16 * sizeof(T));
}

// ----------------------------------------------------------------------------
	
template <typename T>
Violet::UnalignedMatrix4<T>::operator Violet::Matrix4<T>() const
{
	Matrix4<T> result;
	memcpy(result.data(), m_data.get(), 16 * sizeof(T));
	return result;
}

// ============================================================================
