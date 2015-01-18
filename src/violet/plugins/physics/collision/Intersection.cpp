#include "violet/plugins/physics/collision/Intersection.h"

#include <vector>
#include <algorithm>

using namespace Violet;

namespace IntersectionNamespace
{
	const float ms_impactTimeDiffThreshold = 0.01f;
	const float ms_overlapThreshold = 0.2f;
}

using namespace IntersectionNamespace;

#define ROTATION 0

Intersection::Intersection(RigidBody && rb1, RigidBody && rb2, const float frameTime) :
	m_rb1(rb1),
	m_rb2(rb2),
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
	return    m_rb1.project(Vec2f::X_AXIS).overlaps(m_rb2.project(Vec2f::X_AXIS))
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