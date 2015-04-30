#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "engine/entity/Entity.h"
#include "engine/utility/Factory.h"
#include "engine/utility/FreeList.h"

#include <queue>

namespace Violet
{
	class Deserializer;
	class Engine;

	class VIOLET_API EntityFactory : public Factory<const char *, void(Deserializer &, Engine &)>
	{
	public:

		EntityFactory();
		~EntityFactory();

		Entity createNew();
		void free(const Entity & entity);
		std::vector<uint32> getEntities() const;

	private:

		void createFromComponentList(Deserializer & deserializer, Engine & engine);

	private:

		FreeList m_freeList;
	};
}

#endif