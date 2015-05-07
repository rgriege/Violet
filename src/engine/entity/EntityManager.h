#ifndef VIOLET_EntityManager_H
#define VIOLET_EntityManager_H

#include "engine/entity/Entity.h"

#include <queue>
#include <vector>

namespace Violet
{
	class Deserializer;
	class Engine;

	class VIOLET_API EntityManager
	{
	public:

		EntityManager();
		~EntityManager();

		EntityManager(EntityManager && other);
		EntityManager & operator=(EntityManager && other);

		Entity create();
		Entity create(uint32 desiredId);
		void destroy(Entity entity);
		std::vector<Entity> getEntities() const;

	private:

		std::vector<bool> m_usedList;
		std::queue<uint32> m_recycleList;
		std::vector<uint8> m_versions;
	};
}

#endif