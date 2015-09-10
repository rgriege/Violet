#ifndef VIOLET_PhysicsComponent_H
#define VIOLET_PhysicsComponent_H

#include "violet/component/Component.h"

#include "violet/math/Polygon.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API PhysicsComponent : public ComponentBase<PhysicsComponent>
	{
	public:

		static Tag getStaticTag();

	public:

		PhysicsComponent(const Entity & owner, Deserializer & deserializer);
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

	Deserializer & operator>>(Deserializer & deserializer, PhysicsComponent & component);
	Serializer & operator<<(Serializer & serializer, const PhysicsComponent & component);
}

#endif