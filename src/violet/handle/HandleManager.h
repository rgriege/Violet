#ifndef VIOLET_HandleManager_H
#define VIOLET_HandleManager_H

#include <deque>
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
		bool used(Handle handle) const;
		void free(Handle handle);
		void freeAll();

		uint32 getUsedCount() const;
		std::vector<Handle> getUsed() const;

	private:

		std::vector<bool> m_usedList;
		std::deque<typename Handle::StorageType> m_recycleList;
	};
}

#include "violet/handle/HandleManager.inl"

#endif