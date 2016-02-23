#ifndef VIOLET_PhysicsComponent_H
#define VIOLET_PhysicsComponent_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/poly.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Physics_Component
	{
		Poly m_polygon;
		r32 m_mass;
		v2 m_velocity;
		v2 m_force;
		r32 m_momentOfInertia;
		r32 m_angularVelocity;
		r32 m_torque;

		static const Component_Metadata * metadata;

		Physics_Component(Handle entity_id, Component_Deserializer & deserializer);
		Physics_Component(Physics_Component && other);
	};

	VIOLET_API void install_physics_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Physics_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Physics_Component & component);
}

#endif
