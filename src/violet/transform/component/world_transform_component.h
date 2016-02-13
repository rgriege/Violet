#ifndef VIOLET_WORLD_TRANSFORM_COMPONENT_H
#define VIOLET_WORLD_TRANSFORM_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/math/m4.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API world_transform_component final : public component_base<world_transform_component, 1>
	{
		m4 transform;

		static tag get_tag_static();
		static thread get_thread_static();

		world_transform_component(handle entity_id);
		world_transform_component(handle entity_id, component_deserializer & deserializer);
		world_transform_component(handle entity_id, const m4 & transform);
	};

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, world_transform_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const world_transform_component & component);
}

#endif
