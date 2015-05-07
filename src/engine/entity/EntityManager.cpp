// ============================================================================

#include "engine/entity/EntityManager.h"

#include <iostream>

using namespace Violet;

// ============================================================================

EntityManager::EntityManager() :
	m_usedList(),
	m_recycleList(),
	m_versions()
{
}

// ----------------------------------------------------------------------------

EntityManager::~EntityManager()
{
}

// ----------------------------------------------------------------------------

EntityManager::EntityManager(EntityManager && other) :
	m_usedList(other.m_usedList),
	m_recycleList(other.m_recycleList),
	m_versions(other.m_versions)
{
}

// ----------------------------------------------------------------------------

EntityManager & EntityManager::operator=(EntityManager && other)
{
	std::swap(m_usedList, other.m_usedList);
	std::swap(m_recycleList, other.m_recycleList);
	std::swap(m_versions, other.m_versions);
	return *this;
}

// ----------------------------------------------------------------------------

Entity EntityManager::create()
{
	uint32 id, version;
	if (!m_recycleList.empty())
	{
		id = m_recycleList.front();
		m_recycleList.pop();
		m_usedList[id] = true;
		version = m_versions[id];
	}
	else
	{
		m_usedList.push_back(true);
		m_versions.push_back(0);
		id = m_usedList.size() - 1;
		version = m_versions.back();
	}
	return Entity(id, version);
}

// ----------------------------------------------------------------------------

Entity EntityManager::create(const uint32 desiredId)
{
	if (desiredId >= m_usedList.size())
	{
		m_usedList.resize(desiredId + 1);
		m_usedList[desiredId] = true;
		m_versions.resize(desiredId + 1);
		m_versions[desiredId] = 0;
		return Entity(desiredId, 0);
	}
	else if (!m_usedList[desiredId])
	{
		m_usedList[desiredId] = true;
		return Entity(desiredId, ++m_versions[desiredId]);
	}
	else
	{
		const Entity result = create();
		std::cout << "Could not create entity with id " << desiredId << ", using " << result.getId() << " instead" << std::endl;
		return result;
	}
}

// ----------------------------------------------------------------------------

void EntityManager::destroy(const Entity entity)
{
	const uint32 id = entity.getId();
	if (id < m_usedList.size())
	{
		m_usedList[id] = false;
		m_recycleList.push(id);
		++m_versions[id];
	}
}

// ----------------------------------------------------------------------------

std::vector<Entity> EntityManager::getEntities() const
{
	std::vector<Entity> result;
	result.reserve(m_usedList.size() - m_recycleList.size());
	for (uint32 i = 0, end = m_usedList.size(); i < end; ++i)
		if (m_usedList[i])
			result.emplace_back(i, m_versions[i]);
	return result;
}

// ============================================================================
