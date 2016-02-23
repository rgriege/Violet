#ifndef VIOLET_VERSIONED_HANDLE_MANAGER_H
#define VIOLET_VERSIONED_HANDLE_MANAGER_H

#include "violet/handle/handle_manager.h"

namespace vlt
{
	struct Versioned_Handle_Manager
	{
		Handle_Manager manager;
		std::vector<u16> versions;

		Versioned_Handle_Manager();
		Versioned_Handle_Manager(Versioned_Handle_Manager && other) = default;
		Versioned_Handle_Manager & operator=(Versioned_Handle_Manager && other) = default;

		Handle create();
		bool used(Handle Handle) const;
		void free(Handle Handle);
		void free_all();

		std::vector<Handle> get_used() const;
		u16 get_version(u16 id) const;
	};
}

#endif
