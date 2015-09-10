#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

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
		static std::unique_ptr<System> init(Deserializer & deserializer);

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
}

#endif
 