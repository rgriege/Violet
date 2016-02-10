// ============================================================================

#include "violet/physics/collision/Intersection.h"

#include <vector>
#include <algorithm>

using namespace Violet;

// ============================================================================

namespace IntersectionNamespace
{
	const float ms_impactTimeDiffThreshold = 0.01f;
	const float ms_overlapThreshold = 0.2f;

	void getEdgePerpendiculars(const Vector<Vec2f> & vertices, std::vector<Vec2f> & perpendiculars);
}

using namespace IntersectionNamespace;

#define ROTATION 0

// ============================================================================

bool Intersection::test(const Vec2f & start1, const Vec2f & end1, const Vec2f & start2, const Vec2f & end2)
{
	Vec2f intersection;
	return test(start1, end1, start2, end2, intersection);
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Vec2f & start1, const Vec2f & end1, const Vec2f & start2, const Vec2f & end2, Vec2f & intersection)
{
	/*
	 * (Uppercase = vector, lowercase = scalar)
	 * The two lines PR and QS intersect if there exists scalars t & u such that
	 * P + tR = Q + uS
	 *
	 * (P + tR) x S = (Q + uS) x S
	 * t = (Q - P) X S / (R x S)
	 * u = (Q - P) X R / (R X S)
	 */

	const Vec2f & p = start1;
	const Vec2f & r = end1 - start1;
	const Vec2f & q = start2;
	const Vec2f & s = end2 - start2;

	const Vec2f & qmp = start2 - start1;
	const float rxs = r.cross(s);

	if (rxs == 0)
	{
		// parallel or collinear
		// treating collinear lines as not intersecting
		return false;
	}
	else
	{
		const float t = qmp.cross(s) / rxs;
		const float u = qmp.cross(r) / rxs;

		intersection = p + t * r;
		return 0 < u && u < 1 && 0 < t && t < 1;
	}
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Polygon & poly, const Vec2f & start, const Vec2f & end, const bool hollow)
{
	Vec2f intersection;
	return test(poly, start, end, intersection, hollow);
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Polygon & poly, const Vec2f & start, const Vec2f & end, Vec2f & intersection, const bool hollow)
{
	if (test(poly.m_vertices.back(), poly.m_vertices.front(), start, end, intersection))
		return true;

	for (uint32 i = 1, last = poly.m_vertices.size(); i < last; ++i)
	{
		if (test(poly.m_vertices[i - 1], poly.m_vertices[i], start, end, intersection))
			return true;
	}

	return !hollow && poly.contains(start);
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Polygon & poly1, const Polygon & poly2)
{
	return test(poly1, poly2, poly2.getCenter() - poly1.getCenter());
}

// ----------------------------------------------------------------------------

bool Intersection::test(const Polygon & poly1, const Polygon & poly2, const Vec2f & centerToCenter)
{
	if (!poly1.project(Vec2f::X_AXIS).overlaps(poly2.project(Vec2f::X_AXIS).slide(centerToCenter.dot(Vec2f::X_AXIS)))
		|| !poly1.project(Vec2f::Y_AXIS).overlaps(poly2.project(Vec2f::Y_AXIS).slide(centerToCenter.dot(Vec2f::Y_AXIS))))
		return false;

	std::vector<Vec2f> intersectionAxes;
	getEdgePerpendiculars(poly1.m_vertices, intersectionAxes);
	getEdgePerpendiculars(poly2.m_vertices, intersectionAxes);

	for (auto & axis : intersectionAxes)
		axis.normalize();

	for (uint32 i = 0, size = intersectionAxes.size(); i < size; ++i)
	{
		const Vec2f & axis = intersectionAxes[i];
		float overlap = abs(poly1.project(axis).overlap(poly2.project(axis).slide(centerToCenter.dot(axis))));
		if (overlap == 0)
			return false;
	}

	return true;
}

// ============================================================================

Intersection::Intersection(RigidBody && rb1, RigidBody && rb2, const float frameTime) :
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
		return !m_axis.isZero();

	m_tested = true;

	if (!boundsOverlap())
		return false;
	
	const std::pair<std::vector<Vec2f>, uint32> intersectionAxes = findPossibleIntersectionAxes();
	findIntersectionAxis(intersectionAxes);
	if (m_axis.isZero())
		return false;

	m_timeOfImpact = findTimeOfImpact();
	m_impactLocation = findImpactVector();

	return true;
}

Vec2f const & Intersection::getIntersectionAxis() const
{
	if (!m_tested)
		exists();

	return m_axis;
}

float Intersection::getOverlapDistance() const
{
	if (!m_tested)
		exists();

	return m_overlapDistance;
}

float Intersection::getTimeOfImpact() const
{
	if (!m_tested)
		exists();

	return m_timeOfImpact;
}

Vec2f const & Intersection::getImpactLocation() const
{
	if (!m_tested)
		exists();

	return m_impactLocation;
}

bool Intersection::boundsOverlap() const
{
	return	m_rb1.project(Vec2f::X_AXIS).overlaps(m_rb2.project(Vec2f::X_AXIS))
		   && m_rb1.project(Vec2f::Y_AXIS).overlaps(m_rb2.project(Vec2f::Y_AXIS));
}

std::pair<std::vector<Vec2f>, uint32> Intersection::findPossibleIntersectionAxes() const
{
	std::vector<Vec2f> intersectionAxes;
	m_rb1.findIntersectionAxes(m_rb1, intersectionAxes);
	uint32 shape1Axes = intersectionAxes.size();
	m_rb2.findIntersectionAxes(m_rb2, intersectionAxes);

	for (auto & axis : intersectionAxes)
		axis.normalize();

	return std::make_pair(intersectionAxes, shape1Axes);
}

void Intersection::findIntersectionAxis(const std::pair<std::vector<Vec2f>, uint32> & possibleAxes) const
{
	float minimumOverlap = std::numeric_limits<float>::max();
	uint32 intersectionAxisIndex = 0;
	for (uint32 i = 0, size = possibleAxes.first.size(); i < size; ++i)
	{
		const Vec2f & axis = possibleAxes.first[i];
		float overlap = abs(m_rb1.project(axis).overlap(m_rb2.project(axis)));
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
	const Vec2f centerToCenter = m_rb2.getCenter() - m_rb1.getCenter();
	if (   (m_axisFromShape1 && m_axis.dot(centerToCenter) < 0)
		|| (!m_axisFromShape1 && m_axis.dot(centerToCenter) > 0))
		m_axis.invert();
}

float Intersection::findTimeOfImpact() const
{
	float impactTime = m_frameTime;
	float dt = -impactTime / 2;
	float overlap = m_overlapDistance;
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

Vec2f Intersection::findImpactVector() const
{
	std::vector<Vec2f> closestVertices;

	if (m_axisFromShape1)
		m_rb2.findClosestVertices(m_axis, m_rb1.getCenter(), closestVertices);
	else
		m_rb1.findClosestVertices(m_axis, m_rb2.getCenter(), closestVertices);

	if (closestVertices.size() == 1)
		return closestVertices[0];

	if (m_axisFromShape1)
		m_rb1.findClosestVertices(m_axis, m_rb2.getCenter(), closestVertices);
	else
		m_rb2.findClosestVertices(m_axis, m_rb1.getCenter(), closestVertices);

	const Vec2f perp = m_axis.perpendicular();
	std::sort(begin(closestVertices), end(closestVertices), [&](const Vec2f & lhs, const Vec2f & rhs){ return lhs.dot(perp) < rhs.dot(perp); });
	return (closestVertices[1] + closestVertices[2]) / 2.f;
}

// ============================================================================

void IntersectionNamespace::getEdgePerpendiculars(const Vector<Vec2f> & vertices, std::vector<Vec2f> & perpendiculars)
{
	perpendiculars.emplace_back((vertices.front() - vertices.back()).perpendicular());
	for (uint32 i = 1, end = vertices.size(); i < end; ++i)
	{
		const Vec2f side = vertices[i] - vertices[i - 1];
		perpendiculars.push_back(side.perpendicular());
	}
}

// ============================================================================
