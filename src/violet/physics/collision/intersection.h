#ifndef VIOLET_INTERSECTION_H
#define VIOLET_INTERSECTION_H

#include "violet/physics/collision/rigid_body.h"

namespace vlt
{
	struct VIOLET_API Intersection
	{
	public:

		static bool test(const v2 & start1, const v2 & end1, const v2 & start2, const v2 & end2);
		static bool test(const v2 & start1, const v2 & end1, const v2 & start2, const v2 & end2, v2 & intersection);
		static bool test(const Poly & poly, const v2 & start, const v2 & end, bool hollow = false);
		static bool test(const Poly & poly, const v2 & start, const v2 & end, v2 & intersection, bool hollow = false);
		static bool test(const Poly & poly1, const Poly & poly2);
		static bool test(const Poly & poly1, const Poly & poly2, const v2 & center_to_center);

	public:

		Intersection(Rigid_Body && rb1, Rigid_Body && rb2, r32 frameTime);
		Intersection(Intersection && other);
		Intersection(const Intersection &) = delete;

		bool exists() const;
		v2 const & getIntersectionAxis() const;
		r32 getOverlapDistance() const;
		r32 getTimeOfImpact() const;
		v2 const & getImpactLocation() const;

	private:
		
		bool boundsOverlap() const;
		std::pair<std::vector<v2>, u32> findPossibleIntersectionAxes() const;
		void findIntersectionAxis(const std::pair<std::vector<v2>, u32> & possibleAxes) const;
		r32 findTimeOfImpact() const;
		v2 findImpactVector() const;

	private:

		mutable Rigid_Body m_rb1;
		mutable Rigid_Body m_rb2;
		const r32 m_frameTime;
		mutable bool m_tested;

		mutable bool m_axisFromShape1;
		mutable v2 m_axis;
		mutable r32 m_overlapDistance;
		mutable r32 m_timeOfImpact;
		mutable v2 m_impactLocation;
	};
}

#endif
