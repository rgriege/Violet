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

namespace Violet
{
	namespace detail
	{
		template <typename T>
		void loadComponent(ComponentPool & pool, ComponentDeserializer & deserializer)
		{
			pool.create<T>(deserializer.getEntityId("id"), deserializer);
		}

		template <typename T>
		void saveComponent(Serializer & serializer, const T * component)
		{
			serializer << *component;
		}

		template <typename T>
		void deleteComponent(T * component)
		{
			component->~T();
		}
	}
}

template <typename ComponentType>
Violet::ComponentPool Violet::ComponentPool::create()
{
	FuncTable::Load load = (FuncTable::Load) &Violet::detail::loadComponent<ComponentType>;
	FuncTable::Save save = (FuncTable::Save) &Violet::detail::saveComponent<ComponentType>;
	FuncTable::Destroy destroy = (FuncTable::Destroy) &Violet::detail::deleteComponent<ComponentType>;
	auto ftable = std::make_unique<FuncTable>(load, save, destroy);
	return ComponentPool(ComponentType::getStaticTag(), sizeof(ComponentType), ComponentType::getStaticVersion(), std::move(ftable));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & Violet::ComponentPool::create(const EntityId entityId, Args && ... args)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	auto result = insert(entityId);
	assert(!result.second);

	ComponentType * component = new (result.first) ComponentType(entityId, std::forward<Args>(args)...);
	return *component;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const EntityId entityId)
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	const uint32 index = getIndex(entityId);
	return index != size() ? static_cast<ComponentType*>(get(index)) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const ComponentType * Violet::ComponentPool::get(const EntityId entityId) const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	const uint32 index = getIndex(entityId);
	return index != size() ? static_cast<const ComponentType*>(get(index)) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::iterator<ComponentType> Violet::ComponentPool::begin()
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return iterator<ComponentType>(static_cast<ComponentType*>(get(0)), &m_ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::const_iterator<ComponentType> Violet::ComponentPool::begin() const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return const_iterator<ComponentType>(static_cast<const ComponentType*>(get(0)), &m_ids[0]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::iterator<ComponentType> Violet::ComponentPool::end()
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return iterator<ComponentType>(static_cast<ComponentType*>(get(size())), &m_ids[size()]);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::const_iterator<ComponentType> Violet::ComponentPool::end() const
{
	assert(ComponentType::getStaticTag() == m_componentTag);
	return const_iterator<ComponentType>(static_cast<const ComponentType*>(get(size())), &m_ids[size()]);
}

// ============================================================================
