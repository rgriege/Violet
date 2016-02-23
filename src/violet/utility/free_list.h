#ifndef VIOLET_FREE_LIST_H
#define VIOLET_FREE_LIST_H

#include <queue>
#include <vector>

#include "violet/core/defines.h"

namespace vlt
{
	struct Free_List
	{
		std::vector<bool> used_list;
		std::queue<u32> recycle_list;

		Free_List();

		u32 reserve();
		bool reserve(u32 id);
		bool available(u32 id) const;
		std::vector<u32> get_used() const;
		void free(u32 id);
	};
}

#endif
