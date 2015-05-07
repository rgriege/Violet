// ============================================================================

#include "engine/template/TupleUtilities.h"

// ============================================================================

namespace ComponentManagerNamespace
{
	template <typename... ComponentTypes>
	void advance(Violet::Entity & entity, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & iterators, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & ends);

	template <uint32 Index, typename... ComponentTypes>
	void advance(Violet::Entity & entity, uint32 & count, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & iterators, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & ends);
}

// ============================================================================

template <typename... ComponentTypes>
Violet::ComponentManager::Iterator<ComponentTypes...>::Iterator(ComponentManager & manager, const bool begin) :
	m_iterators(std::make_tuple(manager.getPool<ComponentTypes>().begin<ComponentTypes>()...)),
	m_ends(std::make_tuple(manager.getPool<ComponentTypes>().end<ComponentTypes>()...)),
	m_entity(Entity::INVALID)
{
	if (begin)
		++*this;
	else
		m_iterators = m_ends;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::ComponentManager::Iterator<ComponentTypes...> & Violet::ComponentManager::Iterator<ComponentTypes...>::operator++()
{
	m_entity = Entity(m_entity.getId() + 1, m_entity.getVersion());
	ComponentManagerNamespace::advance(m_entity, m_iterators, m_ends);
	return *this;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
std::tuple<ComponentTypes&...> Violet::ComponentManager::Iterator<ComponentTypes...>::operator*()
{
	return std::forward_as_tuple(*Violet::get<ComponentPoolIterator<ComponentTypes>>(m_iterators)...);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool Violet::ComponentManager::Iterator<ComponentTypes...>::operator==(const Iterator<ComponentTypes...> & other) const
{
	return m_entity == other.m_entity;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool Violet::ComponentManager::Iterator<ComponentTypes...>::operator!=(const Iterator<ComponentTypes...> & other) const
{
	return !(*this == other);
}

// ============================================================================

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...>::View(ComponentManager & manager) :
	m_manager(manager)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::ComponentManager::Iterator<ComponentTypes...> Violet::ComponentManager::View<ComponentTypes...>::begin()
{
	return Iterator<ComponentTypes...>(m_manager, true);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::ComponentManager::Iterator<ComponentTypes...> Violet::ComponentManager::View<ComponentTypes...>::end()
{
	return Iterator<ComponentTypes...>(m_manager, false);
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & Violet::ComponentManager::create(const Entity entity, Args&&... args)
{
	return getPool<ComponentType>().create<ComponentType, Args...>(entity, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::ComponentManager::has(const Entity entity) const
{
	return getPool<ComponentType>()->has(entity);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentManager::get(const Entity entity)
{
	return getPool<ComponentType>().get<ComponentType>(entity);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...> Violet::ComponentManager::getView()
{
	return View<ComponentTypes...>(*this);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::ComponentManager::remove(const Entity entity)
{
	return getPool<ComponentType>().remove(entity);
}

// ============================================================================

template <typename ComponentType>
Violet::ComponentPool & Violet::ComponentManager::getPool()
{
	auto it = std::find_if(m_pools.begin(), m_pools.end(), [](ComponentPool & pool) { return pool.getTypeId() == ComponentType::getTypeId(); });
	bool const added = it == m_pools.end();
	if (added)
		m_pools.emplace_back(ComponentPool::create<ComponentType>());
	return added ? m_pools.back() : *it;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const Violet::ComponentPool * Violet::ComponentManager::getPool() const
{
	auto it = std::find_if(m_pools.begin(), m_pools.end(), [](const ComponentPool & pool) { return pool.getTypeId() == ComponentType::getTypeId(); });
	return it == m_pools.end() ? nullptr : &*it;
}

// ============================================================================

template <typename... ComponentTypes>
void ComponentManagerNamespace::advance(Violet::Entity & entity, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & iterators, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & ends)
{
	uint32 count = 0;
	advance<0>(entity, count, iterators, ends);
}

// ----------------------------------------------------------------------------

template <uint32 Index, typename... ComponentTypes>
void ComponentManagerNamespace::advance(Violet::Entity & entity, uint32 & count, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & iterators, std::tuple<Violet::ComponentPoolIterator<ComponentTypes>...> & ends)
{
	if (count != sizeof...(ComponentTypes))
	{
		std::get<Index>(iterators).advanceTo(entity, std::get<Index>(ends));
		if (std::get<Index>(iterators) != std::get<Index>(ends))
		{
			if (std::get<Index>(iterators)->getEntity() != entity)
			{
				count = 1;
				entity = std::get<Index>(iterators)->getEntity();
			}
			else
				++count;

			advance<(Index + 1) % sizeof...(ComponentTypes)>(entity, count, iterators, ends);
		}
		else
		{
			count = sizeof...(ComponentTypes);
			entity = Violet::Entity::INVALID;
		}
	}
}

// ============================================================================
