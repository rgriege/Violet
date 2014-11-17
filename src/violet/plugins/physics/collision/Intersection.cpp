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

Intersection::Intersection(Shape & shape1, Shape & shape2, const float frameTime) :
	m_shape1(shape1.copy()),
	m_shape2(shape2.copy()),
	m_frameTime(frameTime),
	m_tested(false),
	m_axisFromShape1(true),
	m_axis(),
	m_impulseScalar(),
	m_timeOfImpact(),
	m_impactLocation()
{
	m_shape1->translate(m_shape1->getVelocity() * m_frameTime);
	m_shape2->translate(m_shape2->getVelocity() * m_frameTime);
}

Intersection::Intersection(Intersection && other) :
	m_shape1(std::move(other.m_shape1)),
	m_shape2(std::move(other.m_shape2)),
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

Vec2f Intersection::getIntersectionAxis() const
{
	if (!m_tested)
		exists();

	return m_axis;
}

float Intersection::getImpulseScalar() const
{
	if (!m_tested)
		exists();

	return m_impulseScalar;
}

float Intersection::getTimeOfImpact() const
{
	if (!m_tested)
		exists();

	return m_timeOfImpact;
}

Vec2f Intersection::getImpactLocation() const
{
	if (!m_tested)
		exists();

	return m_impactLocation;
}

bool Intersection::boundsOverlap() const
{
	return    m_shape1->project(Vec2f::X_AXIS).overlaps(m_shape2->project(Vec2f::X_AXIS))
		   && m_shape1->project(Vec2f::Y_AXIS).overlaps(m_shape2->project(Vec2f::Y_AXIS));
}

std::pair<std::vector<Vec2f>, uint32> Intersection::findPossibleIntersectionAxes() const
{
	std::vector<Vec2f> intersectionAxes;
	m_shape1->findIntersectionAxes(*m_shape1, intersectionAxes);
	uint32 shape1Axes = intersectionAxes.size();
	m_shape2->findIntersectionAxes(*m_shape2, intersectionAxes);

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
		float overlap = abs(m_shape1->project(axis).overlap(m_shape2->project(axis)));
		if (overlap == 0)
			return;

		if (overlap < minimumOverlap)
		{
			minimumOverlap = overlap;
			intersectionAxisIndex = i;
		}
	}

	m_axis = possibleAxes.first[intersectionAxisIndex];
	m_impulseScalar = minimumOverlap;
	m_axisFromShape1 = intersectionAxisIndex < possibleAxes.second;
	const Vec2f centerToCenter = m_shape2->getCenter() - m_shape1->getCenter();
	if (   (m_axisFromShape1 && m_axis.dot(centerToCenter) < 0)
		|| (!m_axisFromShape1 && m_axis.dot(centerToCenter) > 0))
		m_axis.invert();
}

float Intersection::findTimeOfImpact() const
{
	float impactTime = m_frameTime;
	float dt = -impactTime / 2;
	float overlap = m_impulseScalar;
	while (abs(dt) > ms_impactTimeDiffThreshold && abs(overlap) > ms_overlapThreshold)
	{
		impactTime += dt;
		m_shape1->translate(m_shape1->getVelocity() * dt);
		m_shape2->translate(m_shape2->getVelocity() * dt);
		overlap = m_shape1->project(m_axis).overlap(m_shape2->project(m_axis));
		dt = (overlap > 0 ? -1 : 1) * abs(dt) / 2;
	}

	return impactTime;
}

Vec2f Intersection::findImpactVector() const
{
	std::vector<Vec2f> closestVertices;

	if (m_axisFromShape1)
		m_shape2->findClosestVertices(m_axis, m_shape1->getCenter(), closestVertices);
	else
		m_shape1->findClosestVertices(m_axis, m_shape2->getCenter(), closestVertices);

	if (closestVertices.size() == 1)
		return closestVertices[0];

	if (m_axisFromShape1)
		m_shape1->findClosestVertices(m_axis, m_shape2->getCenter(), closestVertices);
	else
		m_shape2->findClosestVertices(m_axis, m_shape1->getCenter(), closestVertices);

	const Vec2f perp = m_axis.perpendicular();
	std::sort(begin(closestVertices), end(closestVertices), [&](const Vec2f & lhs, const Vec2f & rhs){ return lhs.dot(perp) < rhs.dot(perp); });
	return (closestVertices[1] + closestVertices[2]) / 2.f;
}