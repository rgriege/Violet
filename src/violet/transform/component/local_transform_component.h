#ifndef VIOLET_LOCAL_TRANSFORM_COMPONENT_H
#define VIOLET_LOCAL_TRANSFORM_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/math/m4.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API local_transform_component final : public component_base<local_transform_component, 1>
	{
		handle parent_id;
		m4 transform;

		static tag get_tag_static();
		static thread get_thread_static();

		local_transform_component(handle entity_id, component_deserializer & deserializer);
		local_transform_component(handle entity_id, handle parentId, const m4 & transform);
	};

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, local_transform_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const local_transform_component & component);
}

#endif
