// ============================================================================

template <typename T>
T & Violet::Matrix3<T>::Row::operator[](const size_t i)
{
	return m_entries[i];
}

// ----------------------------------------------------------------------------

template <typename T>
const T & Violet::Matrix3<T>::Row::operator[](const size_t i) const
{
	return m_entries[i];
}

// ============================================================================

template <typename T>
Violet::Matrix3<T> Violet::Matrix3<T>::createRotation(const float radians)
{
	return {
		cos(radians), -sin(radians), 0.f,
		sin(radians), cos(radians), 0.f,
		0.f, 0.f, 1.f
	};
}

// ============================================================================

template <typename T>
const Violet::Matrix3<T> Violet::Matrix3<T>::Identity = {
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
};

// ----------------------------------------------------------------------------

template <typename T>
const Violet::Matrix3<T> Violet::Matrix3<T>::Zero = {
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f
};

// ============================================================================

template <typename T>
typename Violet::Matrix3<T>::Row & Violet::Matrix3<T>::operator[](const size_t i)
{
	return m_rows[i];
}

// ----------------------------------------------------------------------------

template <typename T>
typename Violet::Matrix3<T>::Row const & Violet::Matrix3<T>::operator[](const size_t i) const
{
	return m_rows[i];
}

// ----------------------------------------------------------------------------

template <typename T>
T * Violet::Matrix3<T>::data()
{
	return &m_rows[0][0];
}

// ----------------------------------------------------------------------------

template <typename T>
const T * Violet::Matrix3<T>::data() const
{
	return &m_rows[0][0];
}

// ============================================================================

template <typename T>
Violet::Matrix3<T> Violet::operator*(const Matrix3<T> & lhs, const Matrix3<T> & rhs)
{
	Matrix3<T> result = Matrix3<T>::Zero;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int h = 0; h < 3; h++)
				result[i][j] += lhs[i][h] * rhs[h][j];
	return result;
}

// ----------------------------------------------------------------------------

template <typename T>
Violet::Vec2<T> Violet::operator*(const Matrix3<T> & lhs, const Vec2<T> & rhs)
{
	return Vec2<T>(lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2], lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2]);
}

// ----------------------------------------------------------------------------

template <typename T>
bool Violet::operator==(const Matrix3<T> & lhs, const Matrix3<T> & rhs)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (lhs[i][j] != rhs[i][j])
				return false;
	return true;
}

// ----------------------------------------------------------------------------

template <typename T>
bool Violet::operator!=(const Matrix3<T> & lhs, const Matrix3<T> & rhs)
{
	return !(lhs == rhs);
}

// ============================================================================
