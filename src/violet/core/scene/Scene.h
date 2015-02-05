#ifndef SCENE_FACTORY_H
#define SCENE_FACTORY_H

#include "violet/core/entity/Entity.h"

#include <memory>
#include <vector>

namespace Violet
{
	class VIOLET_API Scene
	{
	public:

		static std::unique_ptr<Scene> create(const char * filename);

	private:

		Scene();

		Entity & createEntity(uint32 id);

	private:

		std::unique_ptr<std::vector<Entity>> m_entities;
	};
}

#endif