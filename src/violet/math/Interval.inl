// ============================================================================

#include <iostream>
#include <cmath>

// ============================================================================

template<typename T>
Violet::Interval<T>::Interval() :
	m_left(0),
	m_right(0)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Interval<T>::Interval(const T left, const T right) :
	m_left(left),
	m_right(right)
{
}

// ----------------------------------------------------------------------------

template<typename T>
Violet::Interval<T>::Interval(const Interval & other) :
	m_left(other.m_left),
	m_right(other.m_right)
{
}

// ----------------------------------------------------------------------------
template<typename T>
Violet::Interval<T> & Violet::Interval<T>::slide(const T delta)
{
	m_left += delta;
	m_right += delta;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Interval<T>::length() const
{
	return m_right - m_left;
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Interval<T>::contains(const T x) const
{
	return x >= m_left && x <= m_right;
}

// ----------------------------------------------------------------------------

template <typename T>
bool Violet::Interval<T>::contains(const Interval & other) const
{
	return contains(other.m_left) && contains(other.m_right);
}

// ----------------------------------------------------------------------------

template<typename T>
bool Violet::Interval<T>::overlaps(const Interval & other) const
{
	return contains(other.m_left) || other.contains(m_left);
}

// ----------------------------------------------------------------------------

template<typename T>
T Violet::Interval<T>::overlap(const Interval &  other) const
{
	T diff1 = m_right - other.m_left;
	T diff2 = other.m_right - m_left;
	return diff1 < 0 || diff2 < 0 ? 0 : std::min(std::min(diff1, diff2), std::min(length(), other.length()));
}

// ============================================================================

template<typename T>
std::ostream & Violet::operator<<(std::ostream & os, const Interval<T> & interval)
{
	return os << "[" << interval.m_left << "," << interval.m_right << "]";
}

// ============================================================================
