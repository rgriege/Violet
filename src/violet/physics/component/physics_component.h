#ifndef VIOLET_PhysicsComponent_H
#define VIOLET_PhysicsComponent_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/poly.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API physics_component
	{
		poly m_polygon;
		r32 m_mass;
		v2 m_velocity;
		v2 m_force;
		r32 m_momentOfInertia;
		r32 m_angularVelocity;
		r32 m_torque;

		static const component_metadata * metadata;

		physics_component(handle entity_id, component_deserializer & deserializer);
		physics_component(physics_component && other);
	};

	VIOLET_API void install_physics_component();

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, physics_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const physics_component & component);
}

#endif
