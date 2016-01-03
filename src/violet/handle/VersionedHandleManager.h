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
		bool used(Handle handle) const;
		void free(Handle handle);
		void freeAll();

		std::vector<Handle> getUsed() const;
        typename Handle::StorageType getVersion(typename Handle::StorageType id) const;

	private:

		HandleManager<Handle> m_manager;
		std::vector<typename Handle::StorageType> m_versions;
	};
}

#include "violet/handle/VersionedHandleManager.inl"

#endif
