#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include "violet/core/transform/TransformComponent.h"

#include <vector>
#include <map>

namespace Violet
{
	class ComponentFactory;
	class Deserializer;
	class Entity;

	class TransformSystem
	{
	public:

		static bool init(ComponentFactory & factory);
		static void update(float dt);
		static void create(Entity & entity, Deserializer & deserializer);
		static TransformComponent & fetch(const Entity & entity);

	private:

		std::vector<TransformComponent> m_components;
		std::map<uint32, uint32> m_entityComponentMap;
	};
}

#endif