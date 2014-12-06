#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include <map>

#include "violet/core/math/Vec2.h"
#include "violet/plugins/physics/component/PhysicsComponent.h"

namespace Violet
{
	class ComponentFactory;
	class Deserializer;
	class Entity;

	class PhysicsSystem
	{
	public:

		class Settings
		{
		public:

			float drag;
			Vec2f gravity;
		};

		static bool init(ComponentFactory & factory, Settings & settings);
		static void update(float dt);
		static void create(Entity & entity, Deserializer & deserializer);
		static PhysicsComponent & fetch(const Entity & entity);

	private:
		
		std::vector<PhysicsComponent> m_components;
		std::map<uint32, uint32> m_entityComponentMap;
		float m_drag;
		Vec2f m_gravity;
	};
}

#endif
 