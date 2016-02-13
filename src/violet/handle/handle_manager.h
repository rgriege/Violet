#ifndef VIOLET_HANDLE_MANAGER_H
#define VIOLET_HANDLE_MANAGER_H

#include <deque>
#include <vector>

#include "violet/core/defines.h"
#include "violet/core/handle.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API handle_manager
	{
		handle_manager();
		~handle_manager();

		handle_manager(handle_manager && other);
		handle_manager & operator=(handle_manager && other);

		handle create();
		bool used(handle handle) const;
		void free(handle handle);
		void freeAll();

		u32 getUsedCount() const;
		std::vector<handle> getUsed() const;

	private:

		std::vector<bool> m_usedList;
		std::deque<u16> m_recycleList;
	};
}

#endif