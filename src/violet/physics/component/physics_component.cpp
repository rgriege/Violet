// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/physics/component/physics_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Physics_Component::metadata;

// ============================================================================

r32 calculate_moment_of_inertia(const Poly & poly, r32 mass);
Component_Deserializer & deserialize_all_but_polygon(Component_Deserializer & deserializer, Physics_Component & component);

// ============================================================================

Physics_Component::Physics_Component(const Handle entity_id, Component_Deserializer & deserializer) :
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

Physics_Component::Physics_Component(Physics_Component && other) :
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
	Physics_Component::metadata = init_component_metadata(Tag('p', 'h', 'y', 's'), 0, sizeof(Physics_Component));
	Scene::install_component<Physics_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Physics_Component & component)
{
	deserializer >> component.m_polygon;
	return deserialize_all_but_polygon(deserializer, component);
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Physics_Component & component)
{
	serializer << component.m_polygon;
	serializer.write_r32("mass", component.m_mass);
	auto segment = serializer.create_segment("vel");
	*segment << component.m_velocity;
	return serializer;
}

// ============================================================================

r32 calculate_moment_of_inertia(const Poly & poly, const r32 mass)
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

Component_Deserializer & deserialize_all_but_polygon(Component_Deserializer & deserializer, Physics_Component & component)
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
