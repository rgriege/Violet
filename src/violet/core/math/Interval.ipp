#ifndef INTERVAL_IPP
#define INTERVAL_IPP

#include <sstream>
#include <cmath>

using namespace Violet;

template<typename T>
Interval<T>::Interval() :
	left(0),
	right(0),
{
}

template<typename T>
Interval<T>::Interval(const T _left, const T _right) :
	left(_left),
	right(_right)
{
}

template<typename T>
Interval<T>::Interval(const Interval & other) :
	left(other.left),
	right(other.right)
{
}

template<typename T>
T Interval<T>::length() const
{
	return right - left;
}

template<typename T>
bool Interval<T>::contains(const T x) const
{
	return x >= left && x <= right;
}

template<typename T>
bool Interval<T>::overlaps(const Interval & other) const
{
	return contains(other.left) || other.contains(left);
}

template<typename T>
T Interval<T>::overlap(const Interval &  other) const
{
	T diff1 = right - other.left;
	T diff2 = other.right - left;
	return diff1 < 0 || diff2 < 0 ? 0 : std::min(std::min(diff1, diff2), std::min(length(), other.length()));
}


template<typename T>
std::ostream & Violet::operator<<(std::ostream & os, const Interval<T> & interval)
{
	return os << "[" << interval.left << "," << interval.right << "]";
}

#endif