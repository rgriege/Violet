#ifndef VIOLET_COMPONENT_METADATA_H
#define VIOLET_COMPONENT_METADATA_H

#include "violet/core/defines.h"
#include "violet/core/tag.h"
#include "violet/core/types.h"

namespace vlt
{
	struct component_metadata
	{
		tag tag;
		u32 thread;
		u32 flag;
		u32 version;
		u32 size;
	};

	VIOLET_API const component_metadata * init_component_metadata(tag t, u32 version, u32 size);
	VIOLET_API void assign_component_to_thread(tag t, u32 thread);
	VIOLET_API const component_metadata * get_component_metadata(tag t);
}

#endif
