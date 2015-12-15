#ifndef VIOLET_VersionedHandleManager_H
#define VIOLET_VersionedHandleManager_H

#include "violet/handle/HandleManager.h"

namespace Violet
{
	template <typename Handle>
	class VersionedHandleManager
	{
	public:

		VersionedHandleManager();
		~VersionedHandleManager();

		VersionedHandleManager(VersionedHandleManager && other);
		VersionedHandleManager & operator=(VersionedHandleManager && other);

		Handle create();
		Handle create(typename Handle::StorageType desiredId);
		void free(Handle handle);
		void freeAll();

		std::vector<Handle> getUsed() const;

	private:

		HandleManager<Handle> m_manager;
		std::vector<typename Handle::StorageType> m_versions;
	};
}

#include "violet/handle/VersionedHandleManager.inl"

#endif