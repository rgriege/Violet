#ifndef VIOLET_MOUSE_INPUT_COMPONENT_H
#define VIOLET_MOUSE_INPUT_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/poly.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API mouse_input_component final
	{
		poly m_mesh;

		static const component_metadata * metadata;

		mouse_input_component(handle entity_id, component_deserializer & deserializer);
		mouse_input_component(handle entity_id, poly && mesh);
		mouse_input_component(mouse_input_component && other);
	};

	VIOLET_API void install_mouse_input_component();

	VIOLET_API serializer & operator<<(serializer & serializer, const mouse_input_component & component);
	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, mouse_input_component & component);
}

#endif