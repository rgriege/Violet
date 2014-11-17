#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "violet/plugins/physics/shape/Shape.h"

#include <memory>

namespace Violet
{
	class Intersection
	{
	public:

		Intersection(Shape & shape1, Shape & shape2, float frameTime);
		Intersection(Intersection && other);
		Intersection(const Intersection &) = delete;

		bool exists() const;
		Vec2f getIntersectionAxis() const;
		float getImpulseScalar() const;
		float getTimeOfImpact() const;
		Vec2f getImpactLocation() const;
		Shape & getShape1() const;
		Shape & getShape2() const;

	private:
		
		bool boundsOverlap() const;
		std::pair<std::vector<Vec2f>, uint32> findPossibleIntersectionAxes() const;
		void findIntersectionAxis(const std::pair<std::vector<Vec2f>, uint32> & possibleAxes) const;
		float findTimeOfImpact() const;
		Vec2f findImpactVector() const;

	private:

		mutable std::unique_ptr<Shape> m_shape1;
		mutable std::unique_ptr<Shape> m_shape2;
		const float m_frameTime;
		mutable bool m_tested;

		mutable bool m_axisFromShape1;
		mutable Vec2f m_axis;
		mutable float m_impulseScalar;
		mutable float m_timeOfImpact;
		mutable Vec2f m_impactLocation;
	};
}

#endif