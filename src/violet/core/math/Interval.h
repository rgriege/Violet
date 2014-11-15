#ifndef INTERVAL_H
#define INTERVAL_H

#include <ostream>

namespace Violet
{
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

	public:

		T left, right;
	};

	template<typename T>
	std::ostream & operator<<(std::ostream & os, const Interval<T> & interval);
}

#include "violet/core/math/Interval.ipp"

typedef Interval<float> FloatInterval;

#endif