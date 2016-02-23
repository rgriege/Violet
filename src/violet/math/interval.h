#ifndef VIOLET_INTERVAL_H
#define VIOLET_INTERVAL_H

#include <iosfwd>

#include "violet/core/defines.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API Interval
	{
		r32 left, right;

		Interval();
		Interval(r32 left, r32 right);
		Interval(const Interval & other);

		Interval & slide(r32 delta);

		r32 length() const;
		
		bool contains(r32 x) const;
		bool contains(const Interval & other) const;
		bool overlaps(const Interval & other) const;
		r32 overlap(const Interval & other) const;
	};

	VIOLET_API std::ostream & operator<<(std::ostream & os, const Interval & interval);
}

#endif
