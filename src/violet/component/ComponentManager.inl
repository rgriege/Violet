// ============================================================================

#include "violet/template/TupleUtilities.h"

#include <algorithm>

// ============================================================================

namespace Violet
{
    namespace detail
    {
        template <typename ComponentType, typename... ComponentTypes>
        struct BeginHelper
        {
            static std::tuple<ComponentPool::Iterator<ComponentType, true>, ComponentPool::Iterator<ComponentTypes, true>...> begin(const Violet::ComponentManager & manager)
            {
                return std::tuple_cat(BeginHelper<ComponentType>::begin(manager), BeginHelper<ComponentTypes...>::begin(manager));
            }
        };

        template <typename ComponentType>
        struct BeginHelper<ComponentType>
        {
            static std::tuple<ComponentPool::Iterator<ComponentType, true>> begin(const Violet::ComponentManager & manager)
            {
                const ComponentPool * pool = manager.getPool<ComponentType>();
                return std::make_tuple(pool->begin<ComponentType>());
            }
        };

        template <typename ComponentType, typename... ComponentTypes>
        struct EndHelper
        {
            static std::tuple<ComponentPool::Iterator<ComponentType, true>, ComponentPool::Iterator<ComponentTypes, true>...> end(const Violet::ComponentManager & manager)
            {
                return std::tuple_cat(EndHelper<ComponentType>::end(manager), EndHelper<ComponentTypes...>::end(manager));
            }
        };

        template <typename ComponentType>
        struct EndHelper<ComponentType>
        {
            static std::tuple<ComponentPool::Iterator<ComponentType, true>> end(const Violet::ComponentManager & manager)
            {
                const ComponentPool * pool = manager.getPool<ComponentType>();
                return std::make_tuple(pool->end<ComponentType>());
            }
        };
    }
}

// ============================================================================

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...>::Entity::Entity(component_tuple components, const EntityId id) :
	m_components(components),
	m_id(id)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
template <typename ComponentType>
const ComponentType & Violet::ComponentManager::View<ComponentTypes...>::Entity::get() const
{
	return Violet::get<const ComponentType &>(m_components);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::EntityId Violet::ComponentManager::View<ComponentTypes...>::Entity::getId() const
{
	return m_id;
}

// ============================================================================

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...>::Iterator::Iterator(iterator_tuple iterators) :
	m_iterators(iterators),
	m_entityId()
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename Violet::ComponentManager::View<ComponentTypes...>::Iterator & Violet::ComponentManager::View<ComponentTypes...>::Iterator::operator++()
{
	m_entityId = m_entityId.isValid() ? EntityId(m_entityId.getId() + 1, m_entityId.getVersion()) : EntityId(0, 0);
	advance();
	return *this;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename Violet::ComponentManager::View<ComponentTypes...>::Entity Violet::ComponentManager::View<ComponentTypes...>::Iterator::operator*()
{
	return Entity(std::forward_as_tuple(*Violet::get<ComponentPool::Iterator<ComponentTypes, true>>(m_iterators)...), m_entityId);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool Violet::ComponentManager::View<ComponentTypes...>::Iterator::operator==(const Iterator & other) const
{
	return m_entityId == other.m_entityId;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
bool Violet::ComponentManager::View<ComponentTypes...>::Iterator::operator!=(const Iterator & other) const
{
	return !(*this == other);
}

// ============================================================================

template <typename... ComponentTypes>
void Violet::ComponentManager::View<ComponentTypes...>::Iterator::advance()
{
	uint32 count = 0;
	advance<0>(count);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
template <uint32 Index>
void Violet::ComponentManager::View<ComponentTypes...>::Iterator::advance(uint32 & count)
{
	if (count != sizeof...(ComponentTypes))
	{
		std::get<Index>(m_iterators).advanceTo(m_entityId);
		if (std::get<Index>(m_iterators))
		{
			if (std::get<Index>(m_iterators).getEntityId() != m_entityId)
			{
				count = 1;
				m_entityId = std::get<Index>(m_iterators).getEntityId();
			}
			else
				++count;

			advance<(Index + 1) % sizeof...(ComponentTypes)>(count);
		}
		else
		{
			count = sizeof...(ComponentTypes);
			m_entityId = Violet::EntityId::ms_invalid;
		}
	}
}

// ============================================================================

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...>::View(const ComponentManager & manager) :
	m_manager(manager)
{
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename Violet::ComponentManager::View<ComponentTypes...>::Iterator Violet::ComponentManager::View<ComponentTypes...>::begin()
{
    Iterator iterator(detail::BeginHelper<ComponentTypes...>::begin(m_manager));
	return ++iterator;
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
typename Violet::ComponentManager::View<ComponentTypes...>::Iterator Violet::ComponentManager::View<ComponentTypes...>::end()
{
    return Iterator(detail::EndHelper<ComponentTypes...>::end(m_manager));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & Violet::ComponentManager::createComponent(const EntityId entityId, Args &&... args)
{
    ComponentPool * pool = getPool<ComponentType>();
	return pool->create<ComponentType, Args...>(entityId, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool * Violet::ComponentManager::getPool()
{
	return getPool(ComponentType::getStaticTag());
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const Violet::ComponentPool * Violet::ComponentManager::getPool() const
{
	return getPool(ComponentType::getStaticTag());
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::ComponentManager::hasComponent(const EntityId entityId) const
{
	return getPool<ComponentType>()->has(entityId);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentManager::getComponent(const EntityId entityId)
{
	return getPool<ComponentType>()->get<ComponentType>(entityId);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * Violet::ComponentManager::getComponent(const EntityId entityId) const
{
	return getPool<ComponentType>()->get<ComponentType>(entityId);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...> Violet::ComponentManager::getEntityView() const
{
	return View<ComponentTypes...>(*this);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::ComponentManager::remove(const EntityId entityId)
{
	return getPool<ComponentType>()->remove(entityId);
}

// ============================================================================

template <typename ComponentType>
void Violet::ComponentManager::installComponent()
{
	installComponent(ComponentType::getStaticTag(), &ComponentManager::createPool<ComponentType>, &ComponentManager::createComponents<ComponentType>, &ComponentManager::savePool<ComponentType>, ComponentType::getStaticThread());
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void Violet::ComponentManager::uninstallComponent()
{
	uninstallComponent(ComponentType::getStaticTag());
}

// ============================================================================

template <typename ComponentType>
Violet::ComponentPool Violet::ComponentManager::createPool()
{
	return ComponentPool::create<ComponentType>();
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void Violet::ComponentManager::createComponents(ComponentPool & pool, ComponentDeserializer & deserializer)
{
	pool.load<ComponentType>(deserializer);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
uint32 Violet::ComponentManager::savePool(const ComponentPool & pool, Serializer & serializer, const std::vector<EntityId> & entityIds)
{
	return pool.save<ComponentType>(serializer, entityIds);
}

// ============================================================================
