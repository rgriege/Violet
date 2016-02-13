#ifndef VIOLET_MOUSE_INPUT_COMPONENT_H
#define VIOLET_MOUSE_INPUT_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/math/poly.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API mouse_input_component final : public component_base<mouse_input_component, 0>
	{
		poly m_mesh;

		static tag get_tag_static();
		static thread get_thread_static();

		mouse_input_component(handle entity_id, component_deserializer & deserializer);
		mouse_input_component(handle entity_id, poly && mesh);
		mouse_input_component(mouse_input_component && other);
	};

	VIOLET_API serializer & operator<<(serializer & serializer, const mouse_input_component & component);
	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, mouse_input_component & component);
}

#endif