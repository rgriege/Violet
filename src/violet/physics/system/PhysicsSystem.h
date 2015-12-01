#ifndef VIOLET_PhysicsSystem_H
#define VIOLET_PhysicsSystem_H

#include "violet/event/Event.h"
#include "violet/math/Vec2.h"
#include "violet/system/System.h"
#include "violet/physics/component/PhysicsComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API PhysicsSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		PhysicsSystem(PhysicsSystem && other);

		virtual ~PhysicsSystem() override = default;
		virtual void update(float dt, const Engine & engine) override;

	private:

		PhysicsSystem(float drag, Vec2f gravity);

	private:
		
		float m_drag;
		Vec2f m_gravity;
	};

	DEFINE_EVENT(CollisionEvent, void(const Entity & e1, const Entity & e2));
}

#endif
 