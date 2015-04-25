#include "violet/core/utility/FreeList.h"

using namespace Violet;

FreeList::FreeList() :
	m_usedList(),
	m_recycleList()
{
}

uint32 FreeList::reserve()
{
	if (!m_recycleList.empty())
	{
		const uint32 id = m_recycleList.front();
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

bool FreeList::reserve(const uint32 id)
{
	if (available(id))
	{
		const uint32 size = m_usedList.size();
		if (id >= size)
		{
			m_usedList.resize(id);
			for (uint32 i = size; i < id; ++i)
				m_recycleList.push(i);
		}
		m_usedList[id] = true;
		return true;
	}
	else
		return false;
}

bool FreeList::available(const uint32 id) const
{
	return id >= m_usedList.size() ? true : m_usedList[id];
}

void FreeList::free(const uint32 id)
{
	if (id < m_usedList.size())
	{
		m_usedList[id] = false;
		m_recycleList.push(id);
	}
}