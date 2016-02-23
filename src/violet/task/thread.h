#ifndef VIOLET_THREAD_H
#define VIOLET_THREAD_H

#include "violet/core/defines.h"

namespace vlt
{
	enum task_type
	{
		read,
		write,
		del,
		last = del,
		cnt = last + 1
	};

	VIOLET_API void init_thread_pool(u32 cnt);
	VIOLET_API void cleanup_thread_pool();

	typedef void(*task_fn)(void*);
	VIOLET_API void add_task(task_fn fn, void * Memory, u32 Thread, task_type type);
	VIOLET_API task_type get_current_stage();
	VIOLET_API void complete_frame_stage();
	VIOLET_API void finish_all_tasks();
}

#endif
