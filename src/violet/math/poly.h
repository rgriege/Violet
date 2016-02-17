#ifndef VIOLET_POLYGON_H
#define VIOLET_POLYGON_H

#include "violet/math/v2.h"
#include "violet/math/aabb.h"
#include "violet/structures/vector.h"

namespace vlt
{
	struct VIOLET_API poly
	{
		vector<v2> vertices;

		explicit poly(vector<v2> && vertices);
		explicit poly(deserializer & deserializer);
		explicit poly(const aabb & box);
		poly(const poly & rhs) = default;
		poly(poly && rhs) = default;
		poly & operator=(const poly & rhs) = default;
		poly & operator=(poly && rhs) = default;

		bool contains(const v2 & point) const;
		aabb get_bounding_box() const;

		void translate(const v2 delta);

		interval project(const v2 & axis) const;
		v2 get_center() const;

		v2 & operator[](u32 index);
		const v2 & operator[](u32 index) const;

		vector<v2>::iterator begin();
		vector<v2>::iterator end();
	};

	VIOLET_API serializer & operator<<(serializer & serializer, const poly & poly);
	VIOLET_API deserializer & operator>>(deserializer & deserializer, poly & poly);
}

#endif
