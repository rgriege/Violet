#ifndef VIOLET_UPDATE_COMPONENT_H
#define VIOLET_UPDATE_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API update_component final : public component_base<update_component, 0>
	{
		static tag get_tag_static();
		static thread get_thread_static();

		update_component(handle entity_id);
		update_component(handle entity_id, component_deserializer & deserializer);
		update_component(update_component && other);
	};

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, update_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const update_component & component);
}

#endif