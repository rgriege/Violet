#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include "engine/component/Component.h"

#include "engine/math/Polygon.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API PhysicsComponent : public Component
	{
	public:

		static const char * getLabel();
	public:

		PhysicsComponent(const Entity & entity, Deserializer & deserializer);
		PhysicsComponent(PhysicsComponent && other);
		PhysicsComponent & operator=(PhysicsComponent && other);

	public:

		Polygon m_polygon;
		float m_mass;
		Vec2f m_velocity;
		Vec2f m_force;
		float m_momentOfInertia;
		float m_angularVelocity;
		float m_torque;
	};

	Serializer & operator<<(Serializer & serializer, const PhysicsComponent & component);
}

#endif