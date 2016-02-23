#include "violet/utility/free_list.h"

using namespace vlt;

Free_List::Free_List() :
	used_list(),
	recycle_list()
{
}

u32 Free_List::reserve()
{
	if (!recycle_list.empty())
	{
		const u32 id = recycle_list.front();
		recycle_list.pop();
		used_list[id] = true;
		return id;
	}
	else
	{
		used_list.push_back(true);
		return used_list.back();
	}
}

bool Free_List::reserve(const u32 id)
{
	if (available(id))
	{
		const u32 size = used_list.size();
		if (id >= size)
		{
			used_list.resize(id + 1);
			for (u32 i = size; i < id; ++i)
				recycle_list.push(i);
		}
		used_list[id] = true;
		return true;
	}
	else
		return false;
}

bool Free_List::available(const u32 id) const
{
	return id >= used_list.size() ? true : used_list[id];
}

std::vector<u32> Free_List::get_used() const
{
	std::vector<u32> result;
	u32 id = 0;
	for (auto const & used : used_list)
	{
		if (used)
			result.emplace_back(id);
		++id;
	}

	return result;
}

void Free_List::free(const u32 id)
{
	if (id < used_list.size())
	{
		used_list[id] = false;
		recycle_list.push(id);
	}
}
