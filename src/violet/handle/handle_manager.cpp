// ============================================================================

#include <algorithm>
#include <assert.h>

#include "violet/handle/handle_manager.h"

using namespace vlt;

// ============================================================================

Handle_Manager::Handle_Manager() :
	used_list(),
	recycle_list()
{
}

Handle Handle_Manager::create()
{
	u16 id;
	if (!recycle_list.empty())
	{
		id = recycle_list.front();
		recycle_list.pop_front();
		used_list[id] = true;
	}
	else
	{
		used_list.push_back(true);
		id = static_cast<u16>(used_list.size() - 1);
	}
	return Handle(id, 0);
}

// ----------------------------------------------------------------------------


bool Handle_Manager::used(const Handle handle) const
{
	const u16 id = handle.id;
	return id < used_list.size() && used_list[id];
}

// ----------------------------------------------------------------------------


void Handle_Manager::free(const Handle handle)
{
	const u16 id = handle.id;
	if (id < used_list.size() && used_list[id])
	{
		used_list[id] = false;
		recycle_list.insert(std::lower_bound(recycle_list.begin(), recycle_list.end(), id), id);
		std::sort(recycle_list.begin(), recycle_list.end());
	}
	else
		assert(false);
}

// ----------------------------------------------------------------------------


void Handle_Manager::free_all()
{
	used_list.clear();
	recycle_list.clear();
}

// ----------------------------------------------------------------------------


u32 Handle_Manager::get_used_count() const
{
	return used_list.size() - recycle_list.size();
}

// ----------------------------------------------------------------------------


std::vector<Handle> Handle_Manager::get_used() const
{
	std::vector<Handle> result;
	result.reserve(get_used_count());
	for (u32 i = 0, end = used_list.size(); i < end; ++i)
		if (used_list[i])
			result.emplace_back(Handle(i, 0));
	return result;
}

// ============================================================================
