// ============================================================================

#include "violet/component/ComponentDeserializer.h"

#include <algorithm>
#include <assert.h>

// ============================================================================

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const>::Iterator(Pointer ptr, const EntityId * idPtr) :
	m_ptr(ptr),
	m_idPtr(idPtr)
{
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const> & Violet::ComponentPool::Iterator<ComponentType, is_const>::operator++()
{
	++m_ptr;
	++m_idPtr;
	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const> & Violet::ComponentPool::Iterator<ComponentType, is_const>::advanceTo(const EntityId entityId)
{
	while (*this && *m_idPtr < entityId)
	{
		++m_idPtr;
		++m_ptr;
	}

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
Violet::EntityId Violet::ComponentPool::Iterator<ComponentType, is_const>::getEntityId() const
{
	return *m_idPtr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType, bool is_const>
Violet::ComponentPool::Iterator<ComponentType, is_const>::operator bool() const
{
	return m_idPtr->isValid();
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
void Violet::ComponentPool::load(ComponentDeserializer & deserializer)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	while (deserializer)
		create<ComponentType>(deserializer.getEntityId("id"), deserializer);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
void Violet::ComponentPool::save(Serializer & serializer) const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	serializer.writeUint("version", ComponentType::getStaticVersion());
	for (uint32 i = 0, end = m_ids.size(); i < end; ++i)
	{
		serializer.writeUint("id", m_ids[i].getId());
		serializer << *get<ComponentType>(i);
	}
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
uint32 Violet::ComponentPool::save(Serializer & serializer, const std::vector<EntityId> & entityIds) const
{
	uint32 count = 0;
	assert(ComponentType::getStaticTag() == m_componentTag);
	serializer.writeUint("version", ComponentType::getStaticVersion());
	for (uint32 i = 0, end = m_ids.size(); i < end; ++i)
	{
		if (std::binary_search(entityIds.cbegin(), entityIds.cend(), m_ids[i]))
		{
			serializer.writeUint("id", m_ids[i].getId());
			serializer << *get<ComponentType>(i);
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
	assert(!result.second);

	ComponentType * component = new (result.first) ComponentType(entityId, std::forward<Args>(args)...);
	return *component;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const EntityId entityId)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	const auto & it = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	return it != m_ids.end() && *it == entityId ? get<ComponentType>(it - m_ids.begin()) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * Violet::ComponentPool::get(const EntityId entityId) const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	const auto & it = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	return it != m_ids.end() && *it == entityId ? get<ComponentType>(it - m_ids.begin()) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::iterator<ComponentType> Violet::ComponentPool::begin()
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return iterator<ComponentType>(get<ComponentType>(0), &m_ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::const_iterator<ComponentType> Violet::ComponentPool::begin() const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return const_iterator<ComponentType>(get<ComponentType>(0), &m_ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::iterator<ComponentType> Violet::ComponentPool::end()
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return iterator<ComponentType>(get<ComponentType>(size()), &m_ids[size()]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::const_iterator<ComponentType> Violet::ComponentPool::end() const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return const_iterator<ComponentType>(get<ComponentType>(size()), &m_ids[size()]);
}

// ============================================================================

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const uint32 index)
{
	return reinterpret_cast<ComponentType*>(m_data.data() + index * m_componentSize);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * Violet::ComponentPool::get(const uint32 index) const
{
	return reinterpret_cast<const ComponentType*>(m_data.data() + index * m_componentSize);
}

// ============================================================================
