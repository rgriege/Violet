#ifndef VIOLET_COMPONENT_METADATA_H
#define VIOLET_COMPONENT_METADATA_H

#include "violet/core/defines.h"
#include "violet/core/tag.h"
#include "violet/core/types.h"

namespace vlt
{
	struct Component_Metadata
	{
		Tag tag;
		u32 thread;
		u32 flag;
		u32 version;
		u32 size;
	};

	VIOLET_API const Component_Metadata * init_component_metadata(Tag tag, u32 version, u32 size);
	VIOLET_API void assign_component_to_thread(Tag tag, u32 thread);
	VIOLET_API const Component_Metadata * get_component_metadata(Tag tag);
}

#endif
