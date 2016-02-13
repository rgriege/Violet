#ifndef VIOLET_FREE_LIST_H
#define VIOLET_FREE_LIST_H

#include "violet/core/defines.h"

#include <queue>
#include <vector>

namespace vlt
{
	struct free_list
	{
		free_list();

		u32 reserve();
		bool reserve(u32 id);
		bool available(u32 id) const;
		std::vector<u32> getUsed() const;
		void free(u32 id);

	private:

		std::vector<bool> m_usedList;
		std::queue<u32> m_recycleList;
	};
}

#endif