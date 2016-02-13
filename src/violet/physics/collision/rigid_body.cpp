// ============================================================================

#include "violet/physics/collision/rigid_body.h"

#include "violet/transform/component/world_transform_component.h"
#include "violet/physics/component/physics_component.h"

using namespace vlt;

// ============================================================================

rigid_body::rigid_body(const world_transform_component & transform, const physics_component & physics) :
	m_center(transform.transform[0][2], transform.transform[1][2]),
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

rigid_body::rigid_body(v2 && center, poly && poly, r32 mass) :
	m_center(center),
	m_rotation(0),
	m_polygon(poly),
	m_mass(mass),
	m_velocity(),
	m_force(),
	m_momentOfInertia(),
	m_angularVelocity(),
	m_torque()
{
}

// ----------------------------------------------------------------------------

const v2 & rigid_body::get_center()
{
	return m_center;
}

// ----------------------------------------------------------------------------

const v2 & rigid_body::getVelocity()
{
	return m_velocity;
}

// ----------------------------------------------------------------------------

void rigid_body::translate(const v2 & translation)
{
	m_center += translation;
}

// ----------------------------------------------------------------------------

void rigid_body::rotate(const r32 radians)
{
	m_rotation += radians;
}

// ----------------------------------------------------------------------------

void rigid_body::applyImpulse(v2 impulse, const v2 & location)
{
	if (impulse.dot(location) < 0)
		impulse.invert();

	m_angularVelocity -= location.cross(impulse) / m_momentOfInertia;
	impulse /= m_mass;
	m_velocity += impulse;
}

// ----------------------------------------------------------------------------

interval rigid_body::project(const v2 & axis) const
{
	const v2 & unitAxis = axis.is_unit() ? axis : axis.get_unit();
	interval projection = m_polygon.project(unitAxis);
	const r32 dp = m_center.dot(unitAxis);
	projection.left += dp;
	projection.right += dp;
	return projection;
}

// ----------------------------------------------------------------------------

r32 rigid_body::maxRadius() const
{
	r32 result = 0;
	for (const auto & vertex : m_polygon.vertices)
	{
		const r32 dist = (vertex + m_center).mag_squared();
		if (dist > result)
			result = dist;
	}
	return sqrt(result);
}

// ----------------------------------------------------------------------------

void rigid_body::findIntersectionAxes(const rigid_body & /*other*/, std::vector<v2> & axes) const
{
	axes.emplace_back((m_polygon.vertices.front() - m_polygon.vertices.back()).perpendicular());
	for (u32 i = 1, end = m_polygon.vertices.size(); i < end; ++i)
	{
		const v2 side = m_polygon.vertices[i] - m_polygon.vertices[i - 1];
		axes.push_back(side.perpendicular());
	}
}

// ----------------------------------------------------------------------------

// TODO: fails if any vertices are past the goal
void rigid_body::findClosestVertices(const v2 & axis, const v2 & goal, std::vector<v2> & vertices) const
{
	const v2 localGoal = goal - m_center;
	r32 closestDist = abs(localGoal.dot(axis));
	u32 closestIdx = 0;
	u32 edgeIdx = 0;
	for (u32 i = 0, len = m_polygon.vertices.size(); i < len; ++i)
	{
		const v2 & vertex = m_polygon.vertices[i];
		r32 const dist = abs((localGoal - vertex).dot(axis));
		if (dist < closestDist)
		{
			closestDist = dist;
			closestIdx = i;
			edgeIdx = 0;
		}
		else if (dist == closestDist && (i - closestIdx == 1 || i - closestIdx == m_polygon.vertices.size() - 1))
		{
			edgeIdx = i;
		}
	}

	vertices.emplace_back(m_center + m_polygon.vertices[closestIdx]);
	if (edgeIdx != 0)
		vertices.emplace_back(m_center + m_polygon.vertices[edgeIdx]);
}

// ============================================================================
