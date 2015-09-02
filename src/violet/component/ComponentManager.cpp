// ============================================================================

#include "engine/component/ComponentManager.h"

using namespace Violet;

// ============================================================================

ComponentManager::ComponentManager(ComponentManager && other) :
	m_pools(std::move(other.m_pools))
{
}

// ----------------------------------------------------------------------------

ComponentManager & ComponentManager::operator=(ComponentManager && other)
{
	std::swap(m_pools, other.m_pools);
	return *this;
}

// ----------------------------------------------------------------------------

bool ComponentManager::removeAll(Entity entity)
{
	bool found = false;
	for (auto & pool : m_pools)
		found |= pool.remove(entity);

	return found;
}

// ----------------------------------------------------------------------------

void ComponentManager::clear()
{
	for (auto & pool : m_pools)
		pool.clear();
}

// ============================================================================
