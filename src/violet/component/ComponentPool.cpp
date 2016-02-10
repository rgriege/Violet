// ============================================================================

#include "violet/component/ComponentPool.h"

#include <assert.h>

using namespace Violet;

// ============================================================================

ComponentPool::FuncTable::FuncTable(Load _load, Save _save, Destroy _destroy) :
	load(_load),
	save(_save),
	destroy(_destroy)
{
}

// ============================================================================

ComponentPool::ComponentPool(ComponentPool && other) :
	m_componentTag(other.m_componentTag),
	m_componentSize(other.m_componentSize),
	m_componentVersion(other.m_componentVersion),
	m_funcTable(std::move(other.m_funcTable)),
	m_componentData(std::move(other.m_componentData)),
	m_ids(std::move(other.m_ids))
{
	other.m_ids.emplace_back(EntityId::ms_invalid);
}

// ----------------------------------------------------------------------------

ComponentPool & ComponentPool::operator=(ComponentPool && other)
{
	assert(m_componentTag == other.m_componentTag);
	assert(m_componentSize == other.m_componentSize);
	assert(m_componentVersion == other.m_componentVersion);
	std::swap(m_funcTable, other.m_funcTable);
	std::swap(m_componentData, other.m_componentData);
	std::swap(m_ids, other.m_ids);
	return *this;
}

// ----------------------------------------------------------------------------

ComponentPool::~ComponentPool()
{
	for (uint32 i = 0, end = size(); i < end; ++i)
		m_funcTable->destroy(get(i));
}

// ----------------------------------------------------------------------------

Tag ComponentPool::getComponentTag() const
{
	return m_componentTag;
}

// ----------------------------------------------------------------------------

void ComponentPool::load(ComponentDeserializer & deserializer)
{
	while (deserializer)
		m_funcTable->load(*this, deserializer);
}

// ----------------------------------------------------------------------------

void ComponentPool::save(Serializer & serailizer) const
{
	serailizer.writeUint("version", m_componentVersion);
	for (uint32 i = 0, end = size(); i < end; ++i)
	{
		serailizer.writeUint("id", m_ids[i].getId());
		m_funcTable->save(serailizer, get(i));
	}
}

// ----------------------------------------------------------------------------

uint32 ComponentPool::save(Serializer & serailizer, const std::vector<EntityId> & entityIds) const
{
	uint32 count = 0;
	serailizer.writeUint("version", m_componentVersion);
	for (uint32 i = 0, end = size(); i < end; ++i)
	{
		const EntityId id = m_ids[i];
		if (std::binary_search(entityIds.cbegin(), entityIds.cend(), id))
		{
			serailizer.writeUint("id", id.getId());
			m_funcTable->save(serailizer, get(i));
			++count;
		}
	}
	return count;
}

// ----------------------------------------------------------------------------

bool ComponentPool::has(const EntityId entityId) const
{
	return getIndex(entityId) != size();
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
	const uint32 idIndex = getIndex(entityId);
	const bool found = idIndex != size();
	if (found)
	{
		const uint32 componentIndex = idIndex * m_componentSize;
		m_funcTable->destroy(&m_componentData[componentIndex]);
		m_componentData.erase(m_componentData.begin() + componentIndex, m_componentData.begin() + componentIndex + m_componentSize);
		m_ids.erase(m_ids.begin() + idIndex);
	}

	return found;
}

// ----------------------------------------------------------------------------

void ComponentPool::clear()
{
	for (uint32 i = 0, end = size(); i < end; ++i)
		m_funcTable->destroy(get(i));
	m_componentData.clear();
	m_ids.clear();
	m_ids.emplace_back(EntityId::ms_invalid);
}

// ============================================================================

ComponentPool::ComponentPool(const Tag typeId, const uint32 componentSize, const uint32 componentVersion, std::unique_ptr<FuncTable> && ftable) :
	m_componentTag(typeId),
	m_componentSize(componentSize),
	m_componentVersion(componentVersion),
	m_funcTable(std::move(ftable)),
	m_componentData(),
	m_ids()
{
	m_ids.emplace_back(EntityId::ms_invalid);
}

// ----------------------------------------------------------------------------

uint32 ComponentPool::getIndex(const EntityId entityId) const
{
	assert(entityId.isValid());

	const auto & idIt = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	return idIt != m_ids.end() && *idIt == entityId ? static_cast<uint32>(std::distance(m_ids.begin(), idIt)) : size();
}

// ----------------------------------------------------------------------------

Component * ComponentPool::get(const uint32 index)
{
	return reinterpret_cast<Component*>(m_componentData.data() + index * m_componentSize);
}

// ----------------------------------------------------------------------------

const Component * ComponentPool::get(const uint32 index) const
{
	return reinterpret_cast<const Component*>(m_componentData.data() + index * m_componentSize);
}

// ----------------------------------------------------------------------------

std::pair<void *, bool>  ComponentPool::insert(const EntityId entityId)
{
	const auto & idIt = std::lower_bound(m_ids.begin(), m_ids.end(), entityId);
	bool const maxId = idIt == m_ids.end();
	const uint32 idIndex = static_cast<uint32>(std::distance(m_ids.begin(), idIt));
	const uint32 componentIndex = idIndex * m_componentSize;
	if (!maxId && *idIt == entityId)
		return std::make_pair(&m_componentData[componentIndex], true);

	m_ids.insert(idIt, entityId);
	auto const componentIt = m_componentData.insert(m_componentData.begin() + componentIndex, m_componentSize, 0);
	return std::make_pair(&*componentIt, false);
}

// ============================================================================
