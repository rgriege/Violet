// ============================================================================

#include <algorithm>
#include <assert.h>

#include "violet/handle/handle_manager.h"

using namespace vlt;

// ============================================================================

handle_manager::handle_manager() :
	m_usedList(),
	m_recycleList()
{
}

// ----------------------------------------------------------------------------

handle_manager::~handle_manager()
{
}

// ----------------------------------------------------------------------------

handle_manager::handle_manager(handle_manager && other) :
	m_usedList(std::move(other.m_usedList)),
	m_recycleList(std::move(other.m_recycleList))
{
}

// ----------------------------------------------------------------------------

handle_manager & handle_manager::operator=(handle_manager && other)
{
	std::swap(m_usedList, other.m_usedList);
	std::swap(m_recycleList, other.m_recycleList);
	return *this;
}

// ----------------------------------------------------------------------------

handle handle_manager::create()
{
	u16 id;
	if (!m_recycleList.empty())
	{
		id = m_recycleList.front();
		m_recycleList.pop_front();
		m_usedList[id] = true;
	}
	else
	{
		m_usedList.push_back(true);
		id = static_cast<u16>(m_usedList.size() - 1);
	}
	return handle(id, 0);
}

// ----------------------------------------------------------------------------


bool handle_manager::used(const handle handle) const
{
	const u16 id = handle.id;
	return id < m_usedList.size() && m_usedList[id];
}

// ----------------------------------------------------------------------------


void handle_manager::free(const handle entity)
{
	const u16 id = entity.id;
	if (id < m_usedList.size() && m_usedList[id])
	{
		m_usedList[id] = false;
		m_recycleList.insert(std::lower_bound(m_recycleList.begin(), m_recycleList.end(), id), id);
		std::sort(m_recycleList.begin(), m_recycleList.end());
	}
	else
		assert(false);
}

// ----------------------------------------------------------------------------


void handle_manager::freeAll()
{
	m_usedList.clear();
	m_recycleList.clear();
}

// ----------------------------------------------------------------------------


u32 handle_manager::getUsedCount() const
{
	return m_usedList.size() - m_recycleList.size();
}

// ----------------------------------------------------------------------------


std::vector<handle> handle_manager::getUsed() const
{
	std::vector<handle> result;
	result.reserve(getUsedCount());
	for (u32 i = 0, end = m_usedList.size(); i < end; ++i)
		if (m_usedList[i])
			result.emplace_back(handle(i, 0));
	return result;
}

// ============================================================================
