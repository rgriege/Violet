#ifndef VIOLET_HandleManager_H
#define VIOLET_HandleManager_H

#include <queue>
#include <vector>

namespace Violet
{
	template <typename Handle>
	class HandleManager
	{
	public:

		HandleManager();
		~HandleManager();

		HandleManager(HandleManager && other);
		HandleManager & operator=(HandleManager && other);

		Handle create();
		Handle create(typename Handle::StorageType desiredId);
		void free(Handle handle);
		void freeAll();

		std::vector<Handle> getUsed() const;

	private:

		std::vector<bool> m_usedList;
		std::queue<typename Handle::StorageType> m_recycleList;
	};
}

#include "violet/handle/HandleManager.inl"

#endif