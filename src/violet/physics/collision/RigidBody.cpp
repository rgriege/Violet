// ============================================================================

#include "violet/physics/collision/RigidBody.h"

#include "violet/transform/component/TransformComponent.h"
#include "violet/physics/component/PhysicsComponent.h"

using namespace Violet;

// ============================================================================

RigidBody::RigidBody(const TransformComponent & transform, const PhysicsComponent & physics) :
	m_center(transform.m_transform[0][2], transform.m_transform[1][2]),
	m_rotation(0),
	m_polygon(physics.m_polygon),
	m_mass(physics.m_mass),
	m_velocity(physics.m_velocity),
	m_force(physics.m_force),
	m_momentOfInertia(physics.m_momentOfInertia),
	m_angularVelocity(physics.m_angularVelocity),
	m_torque(physics.m_torque)
{
}

// ----------------------------------------------------------------------------

RigidBody::RigidBody(Vec2f && center, Polygon && polygon, float mass) :
	m_center(center),
	m_rotation(0),
	m_polygon(polygon),
	m_mass(mass),
	m_velocity(),
	m_force(),
	m_momentOfInertia(),
	m_angularVelocity(),
	m_torque()
{
}

// ----------------------------------------------------------------------------

const Vec2f & RigidBody::getCenter()
{
	return m_center;
}

// ----------------------------------------------------------------------------

const Vec2f & RigidBody::getVelocity()
{
	return m_velocity;
}

// ----------------------------------------------------------------------------

void RigidBody::translate(const Vec2f & translation)
{
	m_center += translation;
}

// ----------------------------------------------------------------------------

void RigidBody::rotate(const float radians)
{
	m_rotation += radians;
}

// ----------------------------------------------------------------------------

void RigidBody::applyImpulse(Vec2f impulse, const Vec2f & location)
{
	if (impulse.dot(location) < 0)
		impulse.invert();

	m_angularVelocity -= location.cross(impulse) / m_momentOfInertia;
	impulse /= m_mass;
	m_velocity += impulse;
}

// ----------------------------------------------------------------------------

FloatInterval RigidBody::project(const Vec2f & axis) const
{
	const Vec2f & unitAxis = axis.isUnit() ? axis : axis.getUnit();
	FloatInterval projection = m_polygon.project(unitAxis);
	const float dp = m_center.dot(unitAxis);
	projection.m_left += dp;
	projection.m_right += dp;
	return projection;
}

// ----------------------------------------------------------------------------

float RigidBody::maxRadius() const
{
	float result = 0;
	for (const auto & vertex : m_polygon.m_vertices)
	{
		const float dist = (vertex + m_center).magSquared();
		if (dist > result)
			result = dist;
	}
	return sqrt(result);
}

// ----------------------------------------------------------------------------

void RigidBody::findIntersectionAxes(const RigidBody & /*other*/, std::vector<Vec2f> & axes) const
{
	axes.emplace_back((m_polygon.m_vertices.front() - m_polygon.m_vertices.back()).perpendicular());
	for (uint32 i = 1, end = m_polygon.m_vertices.size(); i < end; ++i)
	{
		const Vec2f side = m_polygon.m_vertices[i] - m_polygon.m_vertices[i - 1];
		axes.push_back(side.perpendicular());
	}
}

// ----------------------------------------------------------------------------

// TODO: fails if any vertices are past the goal
void RigidBody::findClosestVertices(const Vec2f & axis, const Vec2f & goal, std::vector<Vec2f> & vertices) const
{
	const Vec2f localGoal = goal - m_center;
	float closestDist = abs(localGoal.dot(axis));
	uint32 closestIdx = 0;
	uint32 edgeIdx = 0;
	for (uint32 i = 0, len = m_polygon.m_vertices.size(); i < len; ++i)
	{
		const Vec2f & vertex = m_polygon.m_vertices[i];
		float const dist = abs((localGoal - vertex).dot(axis));
		if (dist < closestDist)
		{
			closestDist = dist;
			closestIdx = i;
			edgeIdx = 0;
		}
		else if (dist == closestDist && (i - closestIdx == 1 || i - closestIdx == m_polygon.m_vertices.size() - 1))
		{
			edgeIdx = i;
		}
	}

	vertices.emplace_back(m_center + m_polygon.m_vertices[closestIdx]);
	if (edgeIdx != 0)
		vertices.emplace_back(m_center + m_polygon.m_vertices[edgeIdx]);
}

// ============================================================================
