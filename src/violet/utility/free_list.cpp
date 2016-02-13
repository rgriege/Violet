#include "violet/utility/free_list.h"

using namespace vlt;

free_list::free_list() :
	m_usedList(),
	m_recycleList()
{
}

u32 free_list::reserve()
{
	if (!m_recycleList.empty())
	{
		const u32 id = m_recycleList.front();
		m_recycleList.pop();
		m_usedList[id] = true;
		return id;
	}
	else
	{
		m_usedList.push_back(true);
		return m_usedList.back();
	}
}

bool free_list::reserve(const u32 id)
{
	if (available(id))
	{
		const u32 size = m_usedList.size();
		if (id >= size)
		{
			m_usedList.resize(id + 1);
			for (u32 i = size; i < id; ++i)
				m_recycleList.push(i);
		}
		m_usedList[id] = true;
		return true;
	}
	else
		return false;
}

bool free_list::available(const u32 id) const
{
	return id >= m_usedList.size() ? true : m_usedList[id];
}

std::vector<u32> free_list::getUsed() const
{
	std::vector<u32> result;
	u32 id = 0;
	for (auto const & used : m_usedList)
	{
		if (used)
			result.emplace_back(id);
		++id;
	}

	return result;
}

void free_list::free(const u32 id)
{
	if (id < m_usedList.size())
	{
		m_usedList[id] = false;
		m_recycleList.push(id);
	}
}