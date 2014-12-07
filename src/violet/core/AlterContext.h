#ifndef ALTER_CONTEXT_H
#define ALTER_CONTEXT_H

#include "violet/core/system/System.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace Violet
{
	class Entity;

	class AlterContext
	{
	public:

		AlterContext(std::vector<std::unique_ptr<System>> & systems) :
			m_systems(systems)
		{
		}

		template <typename SystemType>
		typename SystemType::ComponentType & fetch(const Entity & entity)
		{
			auto it = std::find_if(std::begin(m_systems), std::end(m_systems), [](std::unique_ptr<System> const & system) { return system->getLabel() == SystemType::getStaticLabel(); });
			return dynamic_cast<SystemType *>(it->get())->fetch(entity);
		}

	private:

		std::vector<std::unique_ptr<System>> & m_systems;
	};
}

#endif