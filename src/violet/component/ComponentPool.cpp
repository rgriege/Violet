// ============================================================================

#include "violet/component/ComponentPool.h"

using namespace Violet;

// ============================================================================

ComponentPool::~ComponentPool()
{
	if (!m_data.empty())
	{
		for (uint32 i = 0, end = getLastDataIndex(); i < end; i += m_componentSize)
			get<Component>(i)->~Component();
	}
}

// ----------------------------------------------------------------------------

ComponentPool::ComponentPool(ComponentPool && other) :
	m_componentTag(other.m_componentTag),
	m_componentSize(other.m_componentSize),
	m_data(std::move(other.m_data)),
	m_lookupMap(std::move(other.m_lookupMap))
{
}

// ----------------------------------------------------------------------------

ComponentPool & ComponentPool::operator=(ComponentPool && other)
{
	assert(m_componentTag == other.m_componentTag);
	assert(m_componentSize == other.m_componentSize);
	std::swap(m_data, other.m_data);
	std::swap(m_lookupMap, other.m_lookupMap);
	return *this;
}

// ----------------------------------------------------------------------------

Tag ComponentPool::getComponentTag() const
{
	return m_componentTag;
}

// ----------------------------------------------------------------------------

bool ComponentPool::has(const Handle entityId) const
{
	return m_lookupMap.find(entityId) != m_lookupMap.end();
}

// ----------------------------------------------------------------------------

uint32 ComponentPool::size() const
{
	return m_data.size() / m_componentSize;
}

// ----------------------------------------------------------------------------

bool ComponentPool::remove(const Handle entityId)
{
	assert(entityId.isValid());
	const auto it = m_lookupMap.find(entityId);
	const bool found = it != m_lookupMap.end();
	if (found)
	{
		get<Component>(it->second)->~Component();
		m_data.erase(m_data.begin() + it->second, m_data.begin() + it->second + m_componentSize);
		m_lookupMap.erase(entityId);
		for (auto it = m_lookupMap.upper_bound(entityId), end = m_lookupMap.end(); it != end; ++it)
			it->second -= m_componentSize;
	}

	return found;
}

// ----------------------------------------------------------------------------

void ComponentPool::clear()
{
	for (uint32 i = 0, end = getLastDataIndex(); i < end; i += m_componentSize)
		get<Component>(i)->~Component();
	m_data.clear();
	m_data.resize(8);
	*reinterpret_cast<uint32 *>(&m_data[0]) = ((Handle::ms_invalid.getId() << 8) | Handle::ms_invalid.getVersion());
	m_lookupMap.clear();
}

// ============================================================================

ComponentPool::ComponentPool(const Tag typeId, const uint32 componentSize) :
	m_componentTag(typeId),
	m_componentSize(componentSize),
	m_data(),
	m_lookupMap()
{
#if _ITERATOR_DEBUG_LEVEL > 0
	m_data.reserve(m_componentSize * 100);
#endif
	m_data.resize(8);
	*reinterpret_cast<uint32 *>(&m_data[4]) = ((Handle::ms_invalid.getId() << 8) | Handle::ms_invalid.getVersion());
}

// ----------------------------------------------------------------------------

uint32 ComponentPool::getLastDataIndex() const
{
	return m_data.size() - 8;
}

// ----------------------------------------------------------------------------

std::pair<void *, bool>  ComponentPool::getLocation(const Handle entityId)
{
	const auto lookupEntry = m_lookupMap.lower_bound(entityId);
	bool const maxId = lookupEntry == m_lookupMap.end();
	if (!maxId && lookupEntry->first.getId() == entityId.getId())
		return std::make_pair(&m_data[lookupEntry->second], true);

	auto const componentIt = m_data.insert(m_data.begin() + (maxId ? getLastDataIndex() : lookupEntry->second), m_componentSize, 0);
#if _ITERATOR_DEBUG_LEVEL > 0
	assert(m_data.size() <= m_componentSize * 100);
#endif
	for (auto it = lookupEntry, end = m_lookupMap.end(); it != end; ++it)
		it->second += m_componentSize;
	m_lookupMap[entityId] = std::distance(m_data.begin(), componentIt);
	return std::make_pair(&*componentIt, false);
}

// ============================================================================
