#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

#include "engine/math/Vec2.h"
#include "engine/math/Interval.h"

namespace Violet
{
	class VIOLET_API Polygon
	{
	public:

		explicit Polygon(std::vector<Vec2f> && vertices);
		explicit Polygon(Deserializer & deserializer);

		bool contains(const Vec2f & point) const;

	public:

		std::vector<Vec2f> m_vertices;
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Polygon & poly);
}

#endif
