#ifndef DISK_H
#define DISK_H

#include "violet/core/math/Interval.h"
#include "violet/core/math/Vec2.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/plugins/physics/shape/Shape.h"

#include <vector>

namespace Violet
{
	class Disk : public Shape
	{
	public:

		Disk(Vec2f center, float mass, float radius);
		Disk(Deserializer & deserializer);

	public:

		virtual void rotate(float radians) override;

		virtual float calculateMomentOfInertia() const override;
		virtual FloatInterval project(const Vec2f & axis) const override;
		virtual float maxRadius() const override;
		virtual void findIntersectionAxes(const Shape & other, std::vector<Vec2f> & axes) const override;

	public:

		float m_radius;
	};
}

#endif