#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

#include "violet/core/math/Vec2.h"
#include "violet/core/math/Interval.h"

namespace Violet
{
	class Polygon
	{
	public:

		explicit Polygon(std::vector<Vec2f> vertices);
		explicit Polygon(Deserializer & deserializer);

	public:

		std::vector<Vec2f> m_vertices;
	};
}

#endif
