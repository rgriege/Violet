// ============================================================================

#include "violet/physics/collision/intersection.h"

#include <algorithm>
#include <vector>

using namespace vlt;

// ============================================================================

namespace IntersectionNamespace
{
	const r32 ms_impactTimeDiffThreshold = 0.01f;
	const r32 ms_overlapThreshold = 0.2f;

	void getEdgePerpendiculars(const Vector<v2> & vertices, std::vector<v2> & perpendiculars);
}

using namespace IntersectionNamespace;

#define ROTATION 0

// ============================================================================

bool Intersection::test(const v2 & start1, const v2 & end1, const v2 & start2, const v2 & end2)
{
	v2 Intersection;
	return test(start1, end1, start2, end2, Intersection);
}

// ----------------------------------------------------------------------------

bool Intersection::test(const v2 & start1, const v2 & end1, const v2 & start2, const v2 & end2, v2 & intersection)
{
	/*
	 * (Uppercase = Vector, lowercase = scalar)
	 * The two lines PR and QS intersect if there exists scalars t & u such that
	 * P + tR = Q + uS
	 *
	 * (P + tR) x S = (Q + uS) x S
	 * t = (Q - P) X S / (R x S)
	 * u = (Q - P) X R / (R X S)
	 */

	const v2 & p = start1;
	const v2 & r = end1 - start1;
	const v2 & q = start2;
	const v2 & s = end2 - start2;

	const v2 & qmp = start2 - start1;
	const r32 rxs = r.cross(s);

	if (rxs == 0)
	{
		// parallel or collinear
		// treating collinear lines as not intersecting
		return false;
	}
	else
	{
		const r32 t = qmp.cross(s) / rxs;
		const r32 u = qmp.cross(r) / rxs;

		intersection = p + t * r;
		return 0 < u && u < 1 && 0 < t && t < 1;
	}
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Poly & poly, const v2 & start, const v2 & end, const bool hollow)
{
	v2 intersection;
	return test(poly, start, end, intersection, hollow);
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Poly & poly, const v2 & start, const v2 & end, v2 & intersection, const bool hollow)
{
	if (test(poly.vertices.back(), poly.vertices.front(), start, end, intersection))
		return true;

	for (u32 i = 1, last = poly.vertices.size(); i < last; ++i)
	{
		if (test(poly.vertices[i - 1], poly.vertices[i], start, end, intersection))
			return true;
	}

	return !hollow && poly.contains(start);
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Poly & poly1, const Poly & poly2)
{
	return test(poly1, poly2, poly2.get_center() - poly1.get_center());
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Poly & poly1, const Poly & poly2, const v2 & centerToCenter)
{
	if (!poly1.project(v2::X_Axis).overlaps(poly2.project(v2::X_Axis).slide(centerToCenter.dot(v2::X_Axis)))
		|| !poly1.project(v2::Y_Axis).overlaps(poly2.project(v2::Y_Axis).slide(centerToCenter.dot(v2::Y_Axis))))
		return false;

	std::vector<v2> intersectionAxes;
	getEdgePerpendiculars(poly1.vertices, intersectionAxes);
	getEdgePerpendiculars(poly2.vertices, intersectionAxes);

	for (auto & axis : intersectionAxes)
		axis.normalize();

	for (u32 i = 0, size = intersectionAxes.size(); i < size; ++i)
	{
		const v2 & axis = intersectionAxes[i];
		r32 overlap = abs(poly1.project(axis).overlap(poly2.project(axis).slide(centerToCenter.dot(axis))));
		if (overlap == 0)
			return false;
	}

	return true;
}

// ============================================================================

Intersection::Intersection(Rigid_Body && rb1, Rigid_Body && rb2, const r32 frameTime) :
	m_rb1(std::move(rb1)),
	m_rb2(std::move(rb2)),
	m_frameTime(frameTime),
	m_tested(false),
	m_axisFromShape1(true),
	m_axis(),
	m_overlapDistance(),
	m_timeOfImpact(),
	m_impactLocation()
{
	m_rb1.translate(m_rb1.getVelocity() * m_frameTime);
	m_rb2.translate(m_rb2.getVelocity() * m_frameTime);
}

Intersection::Intersection(Intersection && other) :
	m_rb1(std::move(other.m_rb1)),
	m_rb2(std::move(other.m_rb2)),
	m_frameTime(std::move(other.m_frameTime)),
	m_tested(std::move(other.m_tested)),
	m_axisFromShape1(std::move(other.m_axisFromShape1)),
	m_axis(std::move(other.m_axis)),
	m_timeOfImpact(std::move(other.m_timeOfImpact)),
	m_impactLocation(std::move(other.m_impactLocation))
{
}

bool Intersection::exists() const
{
	if (m_tested)
		return !m_axis.is_zero();

	m_tested = true;

	if (!boundsOverlap())
		return false;
	
	const std::pair<std::vector<v2>, u32> intersectionAxes = findPossibleIntersectionAxes();
	findIntersectionAxis(intersectionAxes);
	if (m_axis.is_zero())
		return false;

	m_timeOfImpact = findTimeOfImpact();
	m_impactLocation = findImpactVector();

	return true;
}

v2 const & Intersection::getIntersectionAxis() const
{
	if (!m_tested)
		exists();

	return m_axis;
}

r32 Intersection::getOverlapDistance() const
{
	if (!m_tested)
		exists();

	return m_overlapDistance;
}

r32 Intersection::getTimeOfImpact() const
{
	if (!m_tested)
		exists();

	return m_timeOfImpact;
}

v2 const & Intersection::getImpactLocation() const
{
	if (!m_tested)
		exists();

	return m_impactLocation;
}

bool Intersection::boundsOverlap() const
{
	return	m_rb1.project(v2::X_Axis).overlaps(m_rb2.project(v2::X_Axis))
		   && m_rb1.project(v2::Y_Axis).overlaps(m_rb2.project(v2::Y_Axis));
}

std::pair<std::vector<v2>, u32> Intersection::findPossibleIntersectionAxes() const
{
	std::vector<v2> intersectionAxes;
	m_rb1.findIntersectionAxes(m_rb1, intersectionAxes);
	u32 shape1Axes = intersectionAxes.size();
	m_rb2.findIntersectionAxes(m_rb2, intersectionAxes);

	for (auto & axis : intersectionAxes)
		axis.normalize();

	return std::make_pair(intersectionAxes, shape1Axes);
}

void Intersection::findIntersectionAxis(const std::pair<std::vector<v2>, u32> & possibleAxes) const
{
	r32 minimumOverlap = std::numeric_limits<r32>::max();
	u32 intersectionAxisIndex = 0;
	for (u32 i = 0, size = possibleAxes.first.size(); i < size; ++i)
	{
		const v2 & axis = possibleAxes.first[i];
		r32 overlap = abs(m_rb1.project(axis).overlap(m_rb2.project(axis)));
		if (overlap == 0)
			return;

		if (overlap < minimumOverlap)
		{
			minimumOverlap = overlap;
			intersectionAxisIndex = i;
		}
	}

	m_axis = possibleAxes.first[intersectionAxisIndex];
	m_overlapDistance = minimumOverlap;
	m_axisFromShape1 = intersectionAxisIndex < possibleAxes.second;
	const v2 centerToCenter = m_rb2.get_center() - m_rb1.get_center();
	if (   (m_axisFromShape1 && m_axis.dot(centerToCenter) < 0)
		|| (!m_axisFromShape1 && m_axis.dot(centerToCenter) > 0))
		m_axis.invert();
}

r32 Intersection::findTimeOfImpact() const
{
	r32 impactTime = m_frameTime;
	r32 dt = -impactTime / 2;
	r32 overlap = m_overlapDistance;
	while (abs(dt) > ms_impactTimeDiffThreshold && abs(overlap) > ms_overlapThreshold)
	{
		impactTime += dt;
		m_rb1.translate(m_rb1.getVelocity() * dt);
		m_rb2.translate(m_rb2.getVelocity() * dt);
		overlap = m_rb1.project(m_axis).overlap(m_rb2.project(m_axis));
		dt = (overlap > 0 ? -1 : 1) * abs(dt) / 2;
	}

	return impactTime;
}

v2 Intersection::findImpactVector() const
{
	std::vector<v2> closestVertices;

	if (m_axisFromShape1)
		m_rb2.findClosestVertices(m_axis, m_rb1.get_center(), closestVertices);
	else
		m_rb1.findClosestVertices(m_axis, m_rb2.get_center(), closestVertices);

	if (closestVertices.size() == 1)
		return closestVertices[0];

	if (m_axisFromShape1)
		m_rb1.findClosestVertices(m_axis, m_rb2.get_center(), closestVertices);
	else
		m_rb2.findClosestVertices(m_axis, m_rb1.get_center(), closestVertices);

	const v2 perp = m_axis.perpendicular();
	std::sort(begin(closestVertices), end(closestVertices), [&](const v2 & lhs, const v2 & rhs){ return lhs.dot(perp) < rhs.dot(perp); });
	return (closestVertices[1] + closestVertices[2]) / 2.f;
}

// ============================================================================

void IntersectionNamespace::getEdgePerpendiculars(const Vector<v2> & vertices, std::vector<v2> & perpendiculars)
{
	perpendiculars.emplace_back((vertices.front() - vertices.back()).perpendicular());
	for (u32 i = 1, end = vertices.size(); i < end; ++i)
	{
		const v2 side = vertices[i] - vertices[i - 1];
		perpendiculars.push_back(side.perpendicular());
	}
}

// ============================================================================
