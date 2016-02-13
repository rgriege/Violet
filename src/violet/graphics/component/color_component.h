#ifndef VIOLET_COLOR_COMPONENT_H
#define VIOLET_COLOR_COMPONENT_H

#include <memory>

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/graphics/color.h"
#include "violet/graphics/component/render_component_data.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;
	struct shader_program;

	struct VIOLET_API color_component final : public component_base<color_component, 0>, public render_component_data
	{
		color m_color;

		static tag get_tag_static();
		static thread get_thread_static();

		color_component(handle entity_id, component_deserializer & deserializer);
		color_component(handle entity_id, const poly & poly, std::shared_ptr<shader_program> shader, color color);
		color_component(const color_component &) = delete;
		color_component & operator=(const color_component &) = delete;
		color_component(color_component && other);
	};

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, color_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const color_component & component);
}

#endif