#include "violet/plugins/physics/component/PhysicsComponent.h"

#include "violet/core/serialization/Deserializer.h"
#include "violet/core/serialization/Serializer.h"

using namespace Violet;

namespace PhysicsComponentNamespace
{
	const char * const ms_massLabel = "mass";
	const char * const ms_velocityLabel = "vel";

	Vec2f deserializeVelocity(Deserializer & deserializer);
	float calculateMomentOfInertia(const Polygon & polygon, float mass);
}

using namespace PhysicsComponentNamespace;

const char * PhysicsComponent::getLabel()
{
	return "phys";
}

PhysicsComponent::PhysicsComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_polygon(deserializer),
	m_mass(deserializer.getFloat(ms_massLabel)),
	m_velocity(deserializeVelocity(deserializer)),
	m_force(),
	m_momentOfInertia(calculateMomentOfInertia(m_polygon, m_mass)),
	m_angularVelocity(),
	m_torque()
{
}

PhysicsComponent::PhysicsComponent(PhysicsComponent && other) :
	Component(std::move(other)),
	m_polygon(std::move(other.m_polygon)),
	m_mass(other.m_mass),
	m_velocity(std::move(other.m_velocity)),
	m_force(std::move(other.m_force)),
	m_momentOfInertia(other.m_momentOfInertia),
	m_angularVelocity(other.m_angularVelocity),
	m_torque(other.m_torque)
{
}

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

Vec2f PhysicsComponentNamespace::deserializeVelocity(Deserializer & deserializer)
{
	auto velocitySegment = deserializer.enterSegment(ms_velocityLabel);
	return Vec2f(*velocitySegment);
}

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

Serializer & Violet::operator<<(Serializer & serializer, const PhysicsComponent & component)
{
	serializer << component.m_polygon;
	serializer.writeFloat(ms_massLabel, component.m_mass);
	auto segment = serializer.createSegment(ms_velocityLabel);
	*segment << component.m_velocity;
	return serializer;
}
