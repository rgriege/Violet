// ============================================================================

#include "violet/component/ComponentPool.h"

using namespace Violet;

// ============================================================================

ComponentPool::~ComponentPool()
{
	if (!m_data.empty())
	{
		for (uint32 i = 0, end = size(); i < end; ++i)
			get<Component>(i)->~Component();
	}
}

// ----------------------------------------------------------------------------

ComponentPool::ComponentPool(ComponentPool && other) :
	m_componentTag(other.m_componentTag),
	m_componentSize(other.m_componentSize),
	m_data(std::move(other.m_data)),
	m_ids(std::move(other.m_ids))
{
}

// ----------------------------------------------------------------------------

ComponentPool & ComponentPool::operator=(ComponentPool && other)
{
	assert(m_componentTag == other.m_componentTag);
	assert(m_componentSize == other.m_componentSize);
	std::swap(m_data, other.m_data);
	std::swap(m_ids, other.m_ids);
	return *this;
}

// ----------------------------------------------------------------------------

Tag ComponentPool::getComponentTag() const
{
	return m_componentTag;
}

// ----------------------------------------------------------------------------

bool ComponentPool::has(const EntityId entityId) const
{
	auto const & it = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	return it != m_ids.end() && *it == entityId;
}

// ----------------------------------------------------------------------------

uint32 ComponentPool::size() const
{
	return m_ids.size() - 1;
}

// ----------------------------------------------------------------------------

bool ComponentPool::remove(const EntityId entityId)
{
	assert(entityId.isValid());
	auto const & it = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	const bool found = it != m_ids.end() && *it == entityId;
	if (found)
	{
		const uint32 index = it - m_ids.begin();
		get<Component>(index)->~Component();
		const uint32 componentDataIndex = index * m_componentSize;
		m_data.erase(m_data.begin() + componentDataIndex, m_data.begin() + componentDataIndex + m_componentSize);
		m_ids.erase(it);
	}

	return found;
}

// ----------------------------------------------------------------------------

void ComponentPool::clear()
{
	for (uint32 i = 0, end = size(); i < end; ++i)
		get<Component>(i)->~Component();
	m_data.clear();
	m_ids.clear();
	m_ids.emplace_back(EntityId::ms_invalid);
}

// ============================================================================

ComponentPool::ComponentPool(const Tag typeId, const uint32 componentSize) :
	m_componentTag(typeId),
	m_componentSize(componentSize),
	m_data(),
	m_ids()
{
	m_ids.emplace_back(EntityId::ms_invalid);
}

// ----------------------------------------------------------------------------

std::pair<void *, bool>  ComponentPool::getLocation(const EntityId entityId)
{
	const auto & it = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	const bool maxId = it == m_ids.end();
	const uint32 componentDataIndex = (it - m_ids.begin()) * m_componentSize;
	if (!maxId && *it == entityId)
		return std::make_pair(&m_data[componentDataIndex], true);

	const auto & componentIt = m_data.insert(m_data.begin() + (maxId ? size() * m_componentSize : componentDataIndex), m_componentSize, 0);
	m_ids.insert(it, entityId);
	return std::make_pair(&*componentIt, false);
}

// ============================================================================
