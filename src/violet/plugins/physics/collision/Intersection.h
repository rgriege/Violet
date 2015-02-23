#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "violet/plugins/physics/collision/RigidBody.h"

namespace Violet
{
	class VIOLET_PHYSICS_API Intersection
	{
	public:

		Intersection(RigidBody && rb1, RigidBody && rb2, float frameTime);
		Intersection(Intersection && other);
		Intersection(const Intersection &) = delete;

		bool exists() const;
		Vec2f const & getIntersectionAxis() const;
		float getOverlapDistance() const;
		float getTimeOfImpact() const;
		Vec2f const & getImpactLocation() const;

	private:
		
		bool boundsOverlap() const;
		std::pair<std::vector<Vec2f>, uint32> findPossibleIntersectionAxes() const;
		void findIntersectionAxis(const std::pair<std::vector<Vec2f>, uint32> & possibleAxes) const;
		float findTimeOfImpact() const;
		Vec2f findImpactVector() const;

	private:

		mutable RigidBody m_rb1;
		mutable RigidBody m_rb2;
		const float m_frameTime;
		mutable bool m_tested;

		mutable bool m_axisFromShape1;
		mutable Vec2f m_axis;
		mutable float m_overlapDistance;
		mutable float m_timeOfImpact;
		mutable Vec2f m_impactLocation;
	};
}

#endif