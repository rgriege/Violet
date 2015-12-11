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
		m_recycleList.pop();
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
Handle Violet::HandleManager<Handle>::create(const typename Handle::StorageType desiredId)
{
	const Handle::StorageType originalUsedListSize = m_usedList.size();
	if (desiredId >= originalUsedListSize)
	{
		m_usedList.resize(desiredId + 1);
		m_usedList[desiredId] = true;
		for (Handle::StorageType id = originalUsedListSize; id < desiredId; ++id)
			m_recycleList.emplace(id);
		return Handle(desiredId, 0);
	}
	else if (!m_usedList[desiredId])
	{
		m_usedList[desiredId] = true;
		return Handle(desiredId, 0);
	}
	else
	{
		const Handle result = create();
		Log::log(FormattedString<1024>().sprintf("Could not create handle %d, using %d instead", desiredId, result.getId()));
		return result;
	}
}

// ----------------------------------------------------------------------------

template <typename Handle>
void Violet::HandleManager<Handle>::free(const Handle entity)
{
	const Handle::StorageType id = entity.getId();
	if (id < m_usedList.size() && m_usedList[id])
	{
		m_usedList[id] = false;
		m_recycleList.push(id);
	}
	else
		assert(false);
}

// ----------------------------------------------------------------------------

template <typename Handle>
void Violet::HandleManager<Handle>::freeAll()
{
	m_usedList.clear();
	m_recycleList.emplace();
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
