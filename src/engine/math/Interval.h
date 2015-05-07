#ifndef VIOLET_Interval_H
#define VIOLET_Interval_H

#include <iosfwd>

namespace Violet
{
	template <typename T>
	class Interval
	{
	public:

		Interval();
		Interval(T left, T right);
		Interval(const Interval & other);
		T length() const;
		bool contains(T x) const;
		bool overlaps(const Interval & other) const;
		T overlap(const Interval & other) const;

	public:

		T m_left, m_right;
	};

	template<typename T>
	std::ostream & operator<<(std::ostream & os, const Interval<T> & interval);
}

#include "engine/math/Interval.inl"

typedef Violet::Interval<float> FloatInterval;

#endif