#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <map>

#include "violet/core/entity/Entity.h"
#include "violet/core/math/Vec2.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/plugins/physics/component/RigidBodyComponent.h"

namespace Violet
{
	class PhysicsSystem
	{
	public:

		class Settings
		{
		public:

			float drag;
			Vec2f gravity;
		};

		static bool init(Settings & settings);

		static void update(float dt);

		static void create(Entity & entity, Deserializer & deserializer);

	private:
		 
		void moveBodies(float time);
		void detectCollisions();
		void applyGravity();

	private:
		
		std::vector<RigidBodyComponent> m_components;
		std::map<uint32, uint32> m_entityComponentMap;
		float m_drag;
		Vec2f m_gravity;
	};
}

#endif
 