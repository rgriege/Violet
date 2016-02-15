// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/physics/component/physics_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const component_metadata * physics_component::metadata;

// ============================================================================

r32 calculate_moment_of_inertia(const poly & poly, r32 mass);
component_deserializer & deserialize_all_but_polygon(component_deserializer & deserializer, physics_component & component);

// ============================================================================

physics_component::physics_component(const handle entity_id, component_deserializer & deserializer) :
	m_polygon(deserializer),
	m_mass(),
	m_velocity(),
	m_force(),
	m_momentOfInertia(),
	m_angularVelocity(),
	m_torque()
{
	deserialize_all_but_polygon(deserializer, *this);
}

// ----------------------------------------------------------------------------

physics_component::physics_component(physics_component && other) :
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

void vlt::install_physics_component()
{
	physics_component::metadata = init_component_metadata(tag('p', 'h', 'y', 's'), 0, sizeof(physics_component));
	scene::install_component<physics_component>();
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, physics_component & component)
{
	deserializer >> component.m_polygon;
	return deserialize_all_but_polygon(deserializer, component);
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const physics_component & component)
{
	serializer << component.m_polygon;
	serializer.write_r32("mass", component.m_mass);
	auto segment = serializer.create_segment("vel");
	*segment << component.m_velocity;
	return serializer;
}

// ============================================================================

r32 calculate_moment_of_inertia(const poly & poly, const r32 mass)
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

component_deserializer & deserialize_all_but_polygon(component_deserializer & deserializer, physics_component & component)
{
	component.m_mass = deserializer.get_r32("mass");
	{
		auto velocitySegment = deserializer.enter_segment("vel");
		*velocitySegment >> component.m_velocity;
	}
	component.m_force = v2::Zero;
	component.m_momentOfInertia = calculate_moment_of_inertia(component.m_polygon, component.m_mass);
	component.m_angularVelocity = 0;
	component.m_torque = 0;
	return deserializer;
}

// ============================================================================
