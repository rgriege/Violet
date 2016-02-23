#ifndef VIOLET_KEY_INPUT_COMPONENT_H
#define VIOLET_KEY_INPUT_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Key_Input_Component final
	{
		static const Component_Metadata * metadata;

		Key_Input_Component(Handle entity_id);
		Key_Input_Component(Handle entity_id, Component_Deserializer & deserializer);
		Key_Input_Component(Key_Input_Component && other);
	};

	VIOLET_API void install_key_input_component();

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Key_Input_Component & component);
	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Key_Input_Component & component);
}

#endif
