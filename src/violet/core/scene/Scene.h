#ifndef SCENE_FACTORY_H
#define SCENE_FACTORY_H

#include "violet/core/entity/Entity.h"

#include <memory>
#include <vector>

namespace Violet
{
	class Scene
	{
	public:

		static std::unique_ptr<Scene> create(const char * filename);

	private:

		Scene(std::vector<Entity> && entities);

	private:

		std::vector<Entity> m_entities;
	};
}

#endif