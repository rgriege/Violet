#ifndef VIOLET_VERSIONED_HANDLE_MANAGER_H
#define VIOLET_VERSIONED_HANDLE_MANAGER_H

#include "violet/handle/handle_manager.h"

namespace vlt
{
	struct versioned_handle_manager
	{
		versioned_handle_manager();
		~versioned_handle_manager();

		versioned_handle_manager(versioned_handle_manager && other);
		versioned_handle_manager & operator=(versioned_handle_manager && other);

		handle create();
		bool used(handle handle) const;
		void free(handle handle);
		void freeAll();

		std::vector<handle> getUsed() const;
		u16 get_version(u16 id) const;

	private:

		handle_manager m_manager;
		std::vector<u16> m_versions;
	};
}

#endif
