#ifndef VIOLET_SYSTEM_H
#define VIOLET_SYSTEM_H

#include "violet/core/defines.h"

namespace vlt
{
	struct System_Metadata
	{
		u32 thread;
		// todo: replace label w/ this
		// Tag Tag;
	};

	struct VIOLET_API System
	{
		const char * const label;

		System(const char * label);
		virtual ~System() = default;

		virtual void update(r32 dt);
	};

	VIOLET_API void assign_system_to_thread(u32 thread);
};

#endif
