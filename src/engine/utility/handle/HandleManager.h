#ifndef VIOLET_HandleManager_H
#define VIOLET_HandleManager_H

#include "engine/utility/handle/Handle.h"

#include <queue>
#include <vector>

namespace Violet
{
	class VIOLET_API HandleManager
	{
	public:

		HandleManager();
		~HandleManager();

		HandleManager(HandleManager && other);
		HandleManager & operator=(HandleManager && other);

		Handle create();
		Handle create(uint32 desiredId);
		void free(Handle handle);

	private:

		std::vector<bool> m_usedList;
		std::queue<uint32> m_recycleList;
		std::vector<uint8> m_versions;
	};
}

#endif