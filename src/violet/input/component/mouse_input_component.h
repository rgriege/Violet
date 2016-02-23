#ifndef VIOLET_MOUSE_INPUT_COMPONENT_H
#define VIOLET_MOUSE_INPUT_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/poly.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Mouse_Input_Component final
	{
		Poly mesh;

		static const Component_Metadata * metadata;

		Mouse_Input_Component(Handle entity_id, Component_Deserializer & deserializer);
		Mouse_Input_Component(Handle entity_id, Poly && mesh);
		Mouse_Input_Component(Mouse_Input_Component && other);
	};

	VIOLET_API void install_mouse_input_component();

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Mouse_Input_Component & component);
	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Mouse_Input_Component & component);
}

#endif
