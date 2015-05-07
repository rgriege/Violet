// ============================================================================

#include <assert.h>

// ============================================================================

template <typename ComponentType>
Violet::ComponentPool::Iterator<ComponentType>::Iterator(ComponentType * ptr) :
	m_ptr(ptr)
{
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::Iterator<ComponentType> & Violet::ComponentPool::Iterator<ComponentType>::operator++()
{
	++m_ptr;
	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::Iterator<ComponentType> & Violet::ComponentPool::Iterator<ComponentType>::advanceTo(const Entity entity, const Iterator<ComponentType> & end)
{
	while (*this != end && m_ptr->getEntity() < entity)
		++m_ptr;

	return *this;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType & Violet::ComponentPool::Iterator<ComponentType>::operator*()
{
	return *m_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentPool::Iterator<ComponentType>::operator->()
{
	return m_ptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::ComponentPool::Iterator<ComponentType>::operator!=(const Iterator<ComponentType> & other) const
{
	return m_ptr != other.m_ptr;
}

// ============================================================================

template <typename ComponentType>
Violet::ComponentPool Violet::ComponentPool::create()
{
	return ComponentPool(ComponentType::getTypeId(), sizeof(ComponentType));
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & Violet::ComponentPool::create(const Entity entity, Args&&... args)
{
	assert(ComponentType::getTypeId() == m_typeId);
	const auto lookupEntry = m_lookupMap.lower_bound(entity);
	bool const maxId = lookupEntry == m_lookupMap.end();
	if (!maxId && lookupEntry->first.getId() == entity.getId())
		return *get<ComponentType>(lookupEntry->second);

	auto const componentIt = m_data.insert(m_data.begin() + (maxId ? m_data.size() : lookupEntry->second), m_componentSize, 0);
	ComponentType * component = new (&*componentIt) ComponentType(entity, std::forward<Args>(args)...);
	m_lookupMap[entity] = std::distance(m_data.begin(), componentIt);
	return *component;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const Entity entity)
{
	assert(ComponentType::getTypeId() == m_typeId);
	auto const it = m_lookupMap.find(entity);
	return it != m_lookupMap.end() ? get<ComponentType>(it->second) : nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::Iterator<ComponentType> Violet::ComponentPool::begin()
{
	assert(ComponentType::getTypeId() == m_typeId);
	return Iterator<ComponentType>(get<ComponentType>(0));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::ComponentPool::Iterator<ComponentType> Violet::ComponentPool::end()
{
	assert(ComponentType::getTypeId() == m_typeId);
	return Iterator<ComponentType>(get<ComponentType>(m_data.size()));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::ComponentPool::get(const uint32 index)
{
	return reinterpret_cast<ComponentType*>(m_data.data() + index);
}

// ============================================================================
