#ifndef VIOLET_AABB_H
#define VIOLET_AABB_H

#include "violet/math/m3.h"
#include "violet/math/v2.h"
#include "violet/math/interval.h"

namespace vlt
{
	struct VIOLET_API aabb
	{
		v2 top_left;
		v2 bottom_right;

		static aabb from_line(const v2 & start, const v2 & end);

		aabb();
		aabb(const v2 & center, const v2 & half_dim);
		aabb(r32 left, r32 top, r32 right, r32 bottom);

		bool empty() const;

		bool contains(const v2 & point) const;
		bool contains(const aabb & other) const;
		bool overlaps(const aabb & other) const;

		aabb & extend(const v2 & point);
		aabb & extend(const aabb & other);
		aabb & translate(const v2 & offset);
		aabb & transform(const m3 & transformation);

		v2 get_center() const;
		v2 get_half_dim() const;
		v2 get_max() const;
		v2 get_min() const;
	};
}

#endif
