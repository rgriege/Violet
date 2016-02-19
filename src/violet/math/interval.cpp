#ifndef VIOLET_INTERVAL_H
#define VIOLET_INTERVAL_H

#include <iosfwd>

#include "violet/core/defines.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API interval
	{
		r32 left, right;

		interval();
		interval(r32 left, r32 right);
		interval(const interval & other);

		interval & slide(r32 delta);

		r32 length() const;
		
		bool contains(r32 x) const;
		bool contains(const interval & other) const;
		bool overlaps(const interval & other) const;
		r32 overlap(const interval & other) const;
	};

	VIOLET_API std::ostream & operator<<(std::ostream & os, const interval & interval);
}

#endif