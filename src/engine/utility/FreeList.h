#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "engine/Defines.h"

#include <queue>
#include <vector>

namespace Violet
{
	class FreeList
	{
	public:

		FreeList();

		uint32 reserve();
		bool reserve(uint32 id);
		bool available(uint32 id) const;
		std::vector<uint32> getUsed() const;
		void free(uint32 id);

	private:

		std::vector<bool> m_usedList;
		std::queue<uint32> m_recycleList;
	};
}

#endif