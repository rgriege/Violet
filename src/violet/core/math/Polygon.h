#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <string>

#include "violet/core/math/Vec2.h"
#include "violet/core/math/Interval.h"

namespace Violet
{
	class Polygon
	{
	public:

		explicit Polygon(std::vector<Vec2f> _vertices);
		explicit Polygon(Deserializer & deserializer);

		FloatInterval project(const Vec2f & vec) const;
		std::vector<Vec2f> getEdgePerps() const;
		const Vec2f & getClosestVertex(const Vec2f & target) const;

	public:

		std::vector<Vec2f> vertices;

	private:

		Vec2f center;
	};
}

#endif
