#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "engine/math/Vec2.h"
#include "engine/system/System.h"
#include "engine/physics/component/PhysicsComponent.h"

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
		virtual void update(float dt, Engine & engine);

	private:

		PhysicsSystem(float drag, Vec2f gravity);

	private:
		
		float m_drag;
		Vec2f m_gravity;
	};
}

#endif
 