// ============================================================================

#include "violet/physics/component/physics_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

namespace PhysicsComponentNamespace
{
	const char * const ms_massLabel = "mass";
	const char * const ms_velocityLabel = "vel";

	r32 calculateMomentOfInertia(const poly & poly, r32 mass);
	component_deserializer & deserializeAllButPolygon(component_deserializer & deserializer, physics_component & component);
}

using namespace PhysicsComponentNamespace;

// ============================================================================

tag physics_component::get_tag_static()
{
	return tag('p', 'h', 'y', 's');
}

// ----------------------------------------------------------------------------

thread physics_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

physics_component::physics_component(const handle entity_id, component_deserializer & deserializer) :
	component_base<physics_component, 0>(),
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

physics_component::physics_component(physics_component && other) :
	component_base<physics_component, 0>(std::move(other)),
	m_polygon(std::move(other.m_polygon)),
	m_mass(other.m_mass),
	m_velocity(std::move(other.m_velocity)),
	m_force(std::move(other.m_force)),
	m_momentOfInertia(other.m_momentOfInertia),
	m_angularVelocity(other.m_angularVelocity),
	m_torque(other.m_torque)
{
}

// ============================================================================

component_deserializer & vlt::operator>>(component_deserializer & deserializer, physics_component & component)
{
	deserializer >> component.m_polygon;
	return deserializeAllButPolygon(deserializer, component);
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const physics_component & component)
{
	serializer << component.m_polygon;
	serializer.write_r32(ms_massLabel, component.m_mass);
	auto segment = serializer.create_segment(ms_velocityLabel);
	*segment << component.m_velocity;
	return serializer;
}

// ============================================================================

r32 PhysicsComponentNamespace::calculateMomentOfInertia(const poly & poly, const r32 mass)
{
	r32 area = 0;
	r32 numerator = 0;
	r32 denominator = 0;
	const u32 size = poly.vertices.size();
	for (u32 i = 0; i < size; ++i)
	{
		const v2 & vertex = poly.vertices[i];
		const v2 & nextVertex = poly.vertices[i < size - 1 ? i + 1 : 0];
		v2 perp = vertex.perpendicular();
		const r32 height = abs(nextVertex.dot(perp));
		const r32 squareArea = height * vertex.magnitude();
		area += 0.5f * squareArea;
		numerator += squareArea*(nextVertex.mag_squared() + nextVertex.dot(vertex) + vertex.mag_squared());
		denominator += squareArea;
	}
	return (mass / 6) * (numerator / denominator);
}

// ----------------------------------------------------------------------------

component_deserializer & PhysicsComponentNamespace::deserializeAllButPolygon(component_deserializer & deserializer, physics_component & component)
{
	component.m_mass = deserializer.get_r32(ms_massLabel);
	{
		auto velocitySegment = deserializer.enter_segment(ms_velocityLabel);
		*velocitySegment >> component.m_velocity;
	}
	component.m_force = v2::Zero;
	component.m_momentOfInertia = calculateMomentOfInertia(component.m_polygon, component.m_mass);
	component.m_angularVelocity = 0;
	component.m_torque = 0;
	return deserializer;
}

// ============================================================================
