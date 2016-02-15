#ifndef VIOLET_KEY_INPUT_COMPONENT_H
#define VIOLET_KEY_INPUT_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API key_input_component final
	{
		static const component_metadata * metadata;

		key_input_component(handle entity_id);
		key_input_component(handle entity_id, component_deserializer & deserializer);
		key_input_component(key_input_component && other);
	};

	VIOLET_API void install_key_input_component();

	VIOLET_API serializer & operator<<(serializer & serializer, const key_input_component & component);
	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, key_input_component & component);
}

#endif