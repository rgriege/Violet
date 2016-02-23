#ifndef VIOLET_POLYGON_H
#define VIOLET_POLYGON_H

#include "violet/math/v2.h"
#include "violet/math/aabb.h"
#include "violet/structures/vector.h"

namespace vlt
{
	struct VIOLET_API Poly
	{
		Vector<v2> vertices;

		explicit Poly(Vector<v2> && vertices);
		explicit Poly(Deserializer & deserializer);
		explicit Poly(const aabb & box);
		Poly(const Poly & rhs) = default;
		Poly(Poly && rhs) = default;
		Poly & operator=(const Poly & rhs) = default;
		Poly & operator=(Poly && rhs) = default;

		bool contains(const v2 & point) const;
		aabb get_bounding_box() const;

		void translate(const v2 delta);

		Interval project(const v2 & axis) const;
		v2 get_center() const;

		v2 & operator[](u32 index);
		const v2 & operator[](u32 index) const;

		Vector<v2>::iterator begin();
		Vector<v2>::iterator end();
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Poly & poly);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Poly & poly);
}

#endif
