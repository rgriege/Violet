#ifndef INTERVAL_H
#define INTERVAL_H

#include <string>

template<typename T>
class Interval
{
public:

	Interval();
	Interval(T _left, T _right);
	Interval(const Interval & other);
	T length() const;
	bool contains(T x) const;
	bool overlaps(const Interval & other) const;
	T overlap(const Interval & other) const;
	std::string toString() const;

public:

	T left, right;
};

#endif

#include "violet/core/math/Interval.ipp"

typedef Interval<float> FloatInterval;