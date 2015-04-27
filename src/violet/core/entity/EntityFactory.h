#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "violet/core/entity/Entity.h"
#include "violet/core/utility/Factory.h"
#include "violet/core/utility/FreeList.h"

#include <queue>

namespace Violet
{
	class Deserializer;
	class SceneInitContext;

	class VIOLET_API EntityFactory : public Factory<const char *, void(Deserializer &, SceneInitContext &)>
	{
	public:

		EntityFactory();
		~EntityFactory();

		Entity createNew();
		void free(const Entity & entity);
		std::vector<uint32> getEntities() const;

	private:

		void createFromComponentList(Deserializer & deserializer, SceneInitContext & initContext);

	private:

		FreeList m_freeList;
	};
}

#endif