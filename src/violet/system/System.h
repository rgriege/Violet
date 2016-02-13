#ifndef VIOLET_SYSTEM_H
#define VIOLET_SYSTEM_H

#include "violet/core/defines.h"

namespace vlt
{
	struct engine;

	struct VIOLET_API system
	{
		const char * const label;

		system(const char * label);
		virtual ~system() = default;

		virtual void update(r32 dt);
	};
};

#endif