#ifndef VIOLET_SYSTEM_H
#define VIOLET_SYSTEM_H

#include "violet/core/defines.h"

namespace vlt
{
	struct system_metadata
	{
		u32 thread;
		// todo: replace label w/ this
		// tag tag;
	};

	struct VIOLET_API system
	{
		const char * const label;

		system(const char * label);
		virtual ~system() = default;

		virtual void update(r32 dt);
	};

	VIOLET_API void assign_system_to_thread(u32 thread);
};

#endif