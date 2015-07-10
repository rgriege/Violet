// ============================================================================

#include "engine/physics/component/PhysicsComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

namespace PhysicsComponentNamespace
{
	const char * const ms_massLabel = "mass";
	const char * const ms_velocityLabel = "vel";

	float calculateMomentOfInertia(const Polygon & polygon, float mass);
	Deserializer & deserializeAllButPolygon(Deserializer & deserializer, PhysicsComponent & component);
}

using namespace PhysicsComponentNamespace;

// ============================================================================

Tag PhysicsComponent::getTag()
{
	return Tag('p', 'h', 'y', 's');
}

// ============================================================================

PhysicsComponent::PhysicsComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<PhysicsComponent>(owner),
	m_polygon(deserializer),
	m_mass(),
	m_velocity(),
	m_force(),
	m_momentOfInertia(),
	m_angularVelocity(),
	m_torque()
{
	deserializeAllButPolygon(deserializer, *this);
}

// ----------------------------------------------------------------------------

PhysicsComponent::PhysicsComponent(PhysicsComponent && other) :
	ComponentBase<PhysicsComponent>(std::move(other)),
	m_polygon(std::move(other.m_polygon)),
	m_mass(other.m_mass),
	m_velocity(std::move(other.m_velocity)),
	m_force(std::move(other.m_force)),
	m_momentOfInertia(other.m_momentOfInertia),
	m_angularVelocity(other.m_angularVelocity),
	m_torque(other.m_torque)
{
}

// ----------------------------------------------------------------------------

PhysicsComponent & PhysicsComponent::operator=(PhysicsComponent && other)
{
	m_polygon = std::move(other.m_polygon);
	m_mass = other.m_mass;
	m_velocity = other.m_velocity;
	m_force = other.m_force;
	m_momentOfInertia = other.m_momentOfInertia;
	m_angularVelocity = other.m_angularVelocity;
	m_torque = other.m_torque;
	return *this;
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, PhysicsComponent & component)
{
	deserializer >> component.m_polygon;
	return deserializeAllButPolygon(deserializer, component);
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const PhysicsComponent & component)
{
	serializer << component.m_polygon;
	serializer.writeFloat(ms_massLabel, component.m_mass);
	auto segment = serializer.createSegment(ms_velocityLabel);
	*segment << component.m_velocity;
	return serializer;
}

// ============================================================================

float PhysicsComponentNamespace::calculateMomentOfInertia(const Polygon & polygon, const float mass)
{
	float area = 0;
	float numerator = 0;
	float denominator = 0;
	const uint32 size = polygon.m_vertices.size();
	for (uint32 i = 0; i < size; ++i)
	{
		const Vec2f & vertex = polygon.m_vertices[i];
		const Vec2f & nextVertex = polygon.m_vertices[i < size - 1 ? i + 1 : 0];
		Vec2f perp = vertex.perpendicular();
		const float height = abs(nextVertex.dot(perp));
		const float squareArea = height * vertex.magnitude();
		area += 0.5f * squareArea;
		numerator += squareArea*(nextVertex.magSquared() + nextVertex.dot(vertex) + vertex.magSquared());
		denominator += squareArea;
	}
	return (mass / 6) * (numerator / denominator);
}

// ----------------------------------------------------------------------------

Deserializer & PhysicsComponentNamespace::deserializeAllButPolygon(Deserializer & deserializer, PhysicsComponent & component)
{
	component.m_mass = deserializer.getFloat(ms_massLabel);
	{
		auto velocitySegment = deserializer.enterSegment(ms_velocityLabel);
		*velocitySegment >> component.m_velocity;
	}
	component.m_force = Vec2f::ZERO;
	component.m_momentOfInertia = calculateMomentOfInertia(component.m_polygon, component.m_mass);
	component.m_angularVelocity = 0;
	component.m_torque = 0;
	return deserializer;
}

// ============================================================================
