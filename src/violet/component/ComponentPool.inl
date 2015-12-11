// ============================================================================

#include "violet/serialization/Deserializer.h"

#include <assert.h>

// ============================================================================

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const>::Iterator(Pointer ptr) :
	m_ptr(ptr)
{
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const> & Violet::ComponentPool::Iterator<ComponentType, is_const>::operator++()
{
	++m_ptr;
	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const> & Violet::ComponentPool::Iterator<ComponentType, is_const>::advanceTo(const EntityId entityId)
{
	while (*this && m_ptr->getEntityId() < entityId)
		++m_ptr;

	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
typename Violet::ComponentPool::Iterator<ComponentType, is_const>::Reference Violet::ComponentPool::Iterator<ComponentType, is_const>::operator*()
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
typename Violet::ComponentPool::Iterator<ComponentType, is_const>::Pointer Violet::ComponentPool::Iterator<ComponentType, is_const>::operator->()
{
	return m_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const>::operator bool() const
{
	return m_ptr->getEntityId().isValid();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
bool Violet::ComponentPool::Iterator<ComponentType, is_const>::operator!=(const Iterator<ComponentType, is_const> & other) const
{
	return m_ptr != other.m_ptr;
}

// ============================================================================

template <typename ComponentType>
Violet::ComponentPool Violet::ComponentPool::create()
{
	return ComponentPool(ComponentType::getStaticTag(), sizeof(ComponentType));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void Violet::ComponentPool::load(Deserializer & deserializer, const std::unordered_map<uint32, EntityId> & idMap)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	while (deserializer)
	{
		const uint32 requestedEntityId = deserializer.getUint("id");
		const auto it = idMap.find(requestedEntityId);
		const EntityId & entityId = it == idMap.end() ? EntityId(requestedEntityId, 0) : it->second;
		create<ComponentType>(entityId, deserializer);
	}
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void Violet::ComponentPool::save(Serializer & serailizer) const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	for (auto it = begin<ComponentType>(), endIt = end<ComponentType>(); it != endIt; ++it)
	{
		serailizer.writeUint("id", it->getEntityId().getId());
		serailizer << *it;
	}
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
uint32 Violet::ComponentPool::save(Serializer & serailizer, const std::vector<EntityId> & entityIds) const
{
	uint32 count = 0;
	assert(ComponentType::getStaticTag() == m_componentTag);
	for (auto it = begin<ComponentType>(), endIt = end<ComponentType>(); it != endIt; ++it)
	{
		if (std::binary_search(entityIds.cbegin(), entityIds.cend(), it->getEntityId()))
		{
			serailizer.writeUint("id", it->getEntityId().getId());
			serailizer << *it;
			++count;
		}
	}
	return count;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename... Args>
ComponentType & Violet::ComponentPool::create(const EntityId entityId, Args && ... args)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	auto result = getLocation(entityId);
	if (result.second)
		return *static_cast<ComponentType*>(result.first);

	ComponentType * component = new (result.first) ComponentType(entityId, std::forward<Args>(args)...);
	return *component;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const EntityId entityId)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	auto const it = m_lookupMap.find(entityId);
	return it != m_lookupMap.end() ? get<ComponentType>(it->second) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * Violet::ComponentPool::get(const EntityId entityId) const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	auto const it = m_lookupMap.find(entityId);
	return it != m_lookupMap.end() ? get<ComponentType>(it->second) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::iterator<ComponentType> Violet::ComponentPool::begin()
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return iterator<ComponentType>(get<ComponentType>(0));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::const_iterator<ComponentType> Violet::ComponentPool::begin() const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return const_iterator<ComponentType>(get<ComponentType>(0));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::iterator<ComponentType> Violet::ComponentPool::end()
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return iterator<ComponentType>(get<ComponentType>(getLastDataIndex()));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::const_iterator<ComponentType> Violet::ComponentPool::end() const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return const_iterator<ComponentType>(get<ComponentType>(getLastDataIndex()));
}

// ============================================================================

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const uint32 index)
{
	return reinterpret_cast<ComponentType*>(m_data.data() + index);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * Violet::ComponentPool::get(const uint32 index) const
{
	return reinterpret_cast<const ComponentType*>(m_data.data() + index);
}

// ============================================================================
