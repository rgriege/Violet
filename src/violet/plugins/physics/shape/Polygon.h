#ifndef POLYGON_H
#define POLYGON_H

#include "violet/plugins/physics/shape/Shape.h"

#include "violet/core/math/Interval.h"
#include "violet/core/math/Vec2.h"

#include <vector>

namespace Violet
{
	class Deserializer;

	class Polygon : public Shape
	{
	public:

		Polygon(float mass, std::vector<Vec2f> vertices);
		Polygon(Deserializer & deserializer);

		virtual void rotate(float radians) override;

		virtual float calculateMomentOfInertia() const override;
		virtual FloatInterval project(const Vec2f & axis) const override;
		virtual float maxRadius() const override;
		virtual void findIntersectionAxes(const Shape & other, std::vector<Vec2f> & axes) const override;
		virtual std::unique_ptr<Shape> copy() const override;
		virtual void findClosestVertices(const Vec2f & axis, const Vec2f & goal, std::vector<Vec2f> & vertices) const override;

	private:

		std::vector<Vec2f> m_vertices;
	};
}

#endif