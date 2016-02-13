#ifndef VIOLET_KEY_INPUT_COMPONENT_H
#define VIOLET_KEY_INPUT_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API key_input_component final : public component_base<key_input_component, 0>
	{
		static tag get_tag_static();
		static thread get_thread_static();

		key_input_component(handle entity_id);
		key_input_component(handle entity_id, component_deserializer & deserializer);
		key_input_component(key_input_component && other);
	};

	VIOLET_API serializer & operator<<(serializer & serializer, const key_input_component & component);
	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, key_input_component & component);
}

#endif