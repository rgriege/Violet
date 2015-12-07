// ============================================================================

#include "violet/handle/HandleManager.h"

#include "violet/log/Log.h"
#include "violet/utility/FormattedString.h"

#include <assert.h>

using namespace Violet;

// ============================================================================

HandleManager::HandleManager() :
	m_usedList(),
	m_recycleList(),
	m_versions()
{
}

// ----------------------------------------------------------------------------

HandleManager::~HandleManager()
{
}

// ----------------------------------------------------------------------------

HandleManager::HandleManager(HandleManager && other) :
	m_usedList(std::move(other.m_usedList)),
	m_recycleList(std::move(other.m_recycleList)),
	m_versions(std::move(other.m_versions))
{
}

// ----------------------------------------------------------------------------

HandleManager & HandleManager::operator=(HandleManager && other)
{
	std::swap(m_usedList, other.m_usedList);
	std::swap(m_recycleList, other.m_recycleList);
	std::swap(m_versions, other.m_versions);
	return *this;
}

// ----------------------------------------------------------------------------

Handle HandleManager::create()
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
	return Handle(id, version);
}

// ----------------------------------------------------------------------------

Handle HandleManager::create(const uint32 desiredId)
{
	const uint32 originalUsedListSize = m_usedList.size();
	if (desiredId >= originalUsedListSize)
	{
		m_usedList.resize(desiredId + 1);
		m_usedList[desiredId] = true;
		m_versions.resize(desiredId + 1);
		m_versions[desiredId] = 0;
		for (uint32 id = originalUsedListSize; id < desiredId; ++id)
			m_recycleList.emplace(id);
		return Handle(desiredId, 0);
	}
	else if (!m_usedList[desiredId])
	{
		m_usedList[desiredId] = true;
		return Handle(desiredId, ++m_versions[desiredId]);
	}
	else
	{
		const Handle result = create();
		Log::log(FormattedString<1024>().sprintf("Could not create handle %d, using %d instead", desiredId, result.getId()));
		return result;
	}
}

// ----------------------------------------------------------------------------

void HandleManager::free(const Handle entity)
{
	const uint32 id = entity.getId();
	if (id < m_usedList.size() && m_usedList[id])
	{
		m_usedList[id] = false;
		m_recycleList.push(id);
		++m_versions[id];
	}
	else
		assert(false);
}

// ----------------------------------------------------------------------------

void HandleManager::freeAll()
{
	m_usedList.clear();
	m_recycleList.emplace();
	m_versions.clear();
}

// ============================================================================
