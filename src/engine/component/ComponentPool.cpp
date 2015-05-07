// ============================================================================

#include "engine/component/ComponentPool.h"

using namespace Violet;

// ============================================================================

Violet::ComponentPool::~ComponentPool()
{
}

// ----------------------------------------------------------------------------

Violet::ComponentPool::ComponentPool(ComponentPool && other) :
	m_typeId(other.m_typeId),
	m_componentSize(other.m_componentSize),
	m_data(std::move(other.m_data)),
	m_lookupMap(std::move(other.m_lookupMap))
{
}

// ----------------------------------------------------------------------------

Violet::ComponentPool & Violet::ComponentPool::operator=(ComponentPool && other)
{
	std::swap(m_typeId, other.m_typeId);
	std::swap(m_componentSize, other.m_componentSize);
	std::swap(m_data, other.m_data);
	std::swap(m_lookupMap, other.m_lookupMap);
	return *this;
}

// ----------------------------------------------------------------------------

Violet::Tag Violet::ComponentPool::getTypeId() const
{
	return m_typeId;
}

// ----------------------------------------------------------------------------

bool Violet::ComponentPool::has(const Entity entity) const
{
	return m_lookupMap.find(entity) != m_lookupMap.end();
}

// ----------------------------------------------------------------------------

bool Violet::ComponentPool::remove(const Entity entity)
{
	const auto it = m_lookupMap.find(entity);
	const bool found = it != m_lookupMap.end();
	if (found)
	{
		reinterpret_cast<BaseComponent *>(m_data.data() + it->second)->~BaseComponent();
		m_data.erase(m_data.begin() + it->second, m_data.begin() + it->second + m_componentSize);
		m_lookupMap.erase(entity);
	}

	return found;
}

// ----------------------------------------------------------------------------

void Violet::ComponentPool::clear()
{
	for (uint32 i = 0, end = m_data.size(); i < end; i += m_componentSize)
		reinterpret_cast<BaseComponent *>(&m_data[i])->~BaseComponent();
	m_data.clear();
	m_lookupMap.clear();
}

// ----------------------------------------------------------------------------

Violet::ComponentPool::ComponentPool(const Tag typeId, const uint32 componentSize) :
	m_typeId(typeId),
	m_componentSize(componentSize),
	m_data(),
	m_lookupMap()
{
}

// ============================================================================
