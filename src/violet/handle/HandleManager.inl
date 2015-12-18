// ============================================================================

#include <assert.h>

// ============================================================================

template <typename Handle>
Violet::HandleManager<Handle>::HandleManager() :
	m_usedList(),
	m_recycleList()
{
}

// ----------------------------------------------------------------------------

template <typename Handle>
Violet::HandleManager<Handle>::~HandleManager()
{
}

// ----------------------------------------------------------------------------

template <typename Handle>
Violet::HandleManager<Handle>::HandleManager(HandleManager && other) :
	m_usedList(std::move(other.m_usedList)),
	m_recycleList(std::move(other.m_recycleList))
{
}

// ----------------------------------------------------------------------------

template <typename Handle>
Violet::HandleManager<Handle> & Violet::HandleManager<Handle>::operator=(HandleManager && other)
{
	std::swap(m_usedList, other.m_usedList);
	std::swap(m_recycleList, other.m_recycleList);
	return *this;
}

// ----------------------------------------------------------------------------

template <typename Handle>
Handle Violet::HandleManager<Handle>::create()
{
	Handle::StorageType id;
	if (!m_recycleList.empty())
	{
		id = m_recycleList.front();
		m_recycleList.pop_front();
		m_usedList[id] = true;
	}
	else
	{
		m_usedList.push_back(true);
		id = m_usedList.size() - 1;
	}
	return Handle(id, 0);
}

// ----------------------------------------------------------------------------

template <typename Handle>
bool Violet::HandleManager<Handle>::used(const Handle handle) const
{
	const Handle::StorageType id = handle.getId();
	return id < m_usedList.size() && m_usedList[id];
}

// ----------------------------------------------------------------------------

template <typename Handle>
void Violet::HandleManager<Handle>::free(const Handle entity)
{
	const Handle::StorageType id = entity.getId();
	if (id < m_usedList.size() && m_usedList[id])
	{
		m_usedList[id] = false;
		m_recycleList.emplace_back(id);
		std::sort(m_recycleList.begin(), m_recycleList.end());
	}
	else
		assert(false);
}

// ----------------------------------------------------------------------------

template <typename Handle>
void Violet::HandleManager<Handle>::freeAll()
{
	m_usedList.clear();
	m_recycleList = std::deque<typename Handle::StorageType>();
}

// ----------------------------------------------------------------------------

template <typename Handle>
uint32 Violet::HandleManager<Handle>::getUsedCount() const
{
	return m_usedList.size() - m_recycleList.size();
}

// ----------------------------------------------------------------------------

template <typename Handle>
std::vector<Handle> Violet::HandleManager<Handle>::getUsed() const
{
	std::vector<Handle> result;
	result.reserve(getUsedCount());
	for (uint32 i = 0, end = m_usedList.size(); i < end; ++i)
		if (m_usedList[i])
			result.emplace_back(Handle(i, 0));
	return result;
}

// ============================================================================
