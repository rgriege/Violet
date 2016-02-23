#ifndef VIOLET_HANDLE_MANAGER_H
#define VIOLET_HANDLE_MANAGER_H

#include <deque>
#include <vector>

#include "violet/core/defines.h"
#include "violet/core/handle.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API Handle_Manager
	{
		std::vector<bool> used_list;
		std::deque<u16> recycle_list;

		Handle_Manager();
		Handle_Manager(Handle_Manager && other) = default;
		Handle_Manager & operator=(Handle_Manager && other) = default;

		Handle create();
		bool used(Handle handle) const;
		void free(Handle handle);
		void free_all();

		u32 get_used_count() const;
		std::vector<Handle> get_used() const;
	};
}

#endif
