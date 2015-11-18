#ifndef VIOLET_Polygon_H
#define VIOLET_Polygon_H

#include <vector>

#include "violet/math/Vec2.h"
#include "violet/math/AABB.h"

namespace Violet
{
	class VIOLET_API Polygon
	{
	public:

		typedef std::vector<Vec2f> Vertices;

	public:

		explicit Polygon(std::vector<Vec2f> && vertices);
		explicit Polygon(Deserializer & deserializer);
		Polygon(const Polygon & rhs);
		Polygon(Polygon && rhs);

		bool contains(const Vec2f & point) const;
		AABB getBoundingBox() const;

		void translate(const Vec2f delta);

		FloatInterval project(const Vec2f & axis) const;
		Vec2f getCenter() const;

		Vec2f & operator[](uint32 index);
		const Vec2f & operator[](uint32 index) const;

		Vertices::iterator begin();
		Vertices::iterator end();

	public:

		Vertices m_vertices;
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Polygon & poly);
	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, Polygon & poly);
}

#endif
