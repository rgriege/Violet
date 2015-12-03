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

		PhysicsComponent(Entity & owner, Deserializer & deserializer);
		PhysicsComponent(PhysicsComponent && other);

	public:

		Polygon m_polygon;
		float m_mass;
		Vec2f m_velocity;
		Vec2f m_force;
		float m_momentOfInertia;
		float m_angularVelocity;
		float m_torque;
	};

	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, PhysicsComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const PhysicsComponent & component);
}

#endif
