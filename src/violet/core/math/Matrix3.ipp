#ifndef Matrix3_IPP
#define Matrix3_IPP

using namespace Violet;

template <typename T>
T & Matrix3<T>::Column::operator[](const size_t i)
{
	return m_entries[i];
}

template <typename T>
const T & Matrix3<T>::Column::operator[](const size_t i) const
{
	return m_entries[i];
}

template <typename T>
const Matrix3<T> Matrix3<T>::Identity = {
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
};

template <typename T>
const Matrix3<T> Matrix3<T>::Zero;

template <typename T>
typename Matrix3<T>::Column & Matrix3<T>::operator[](const size_t i)
{
	return m_rows[i];
}

template <typename T>
typename Matrix3<T>::Column const & Matrix3<T>::operator[](const size_t i) const
{
	return m_rows[i];
}

template <typename T>
T * Matrix3<T>::data()
{
	return &m_rows[0][0];
}

template <typename T>
const T * Matrix3<T>::data() const
{
	return &m_rows[0][0];
}

#endif
