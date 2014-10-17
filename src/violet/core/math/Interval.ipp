#ifndef INTERVAL_IPP
#define INTERVAL_IPP

#include <sstream>
#include <string>
#include <cmath>

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
	T maxDiff = (length() > other.length()) ? length() : other.length();
	T diff1 = right - other.left;
	if (diff1 < 0 || diff1 > maxDiff) diff1 = 0;
	T diff2 = other.right - left;
	if (diff2 < 0 || diff2 > maxDiff) diff2 = 0;
	// return the smallest nonzero if one exists
	return diff1 == 0 || diff2 == 0 ?
		diff1 + diff2 :
		diff1 < diff2 ?
			diff1 :
			diff2;
}

template<typename T>
std::string Interval<T>::toString() const
{
	stringstream ss;
	ss << "(" << left << "," << right << ")";
	return ss.str();
}

#endif