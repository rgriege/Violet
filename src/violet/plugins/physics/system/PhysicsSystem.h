#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "violet/core/math/Vec2.h"
#include "violet/core/system/System.h"
#include "violet/plugins/physics/component/PhysicsComponent.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_PHYSICS_API PhysicsSystem : public ComponentSystem<PhysicsComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~PhysicsSystem() override = default;
		virtual void update(float dt, Engine & engine);

	private:

		PhysicsSystem(float drag, Vec2f gravity);

	private:
		
		float m_drag;
		Vec2f m_gravity;
	};
}

#endif
 