#ifndef VIOLET_TEXT_COMPONENT_H
#define VIOLET_TEXT_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/graphics/color.h"
#include "violet/graphics/component/render_component_data.h"
#include "violet/task/thread.h"

#include <string>
#include <memory>

namespace vlt
{
	struct component_deserializer;
	struct font;
	struct serializer;

	struct VIOLET_API text_component final : public component_base<text_component, 0>, public render_component_data
	{
		std::string m_text;
		std::shared_ptr<font> m_font;
		color m_color;

		static tag get_tag_static();
		static thread get_thread_static();

		text_component(handle entity_id, component_deserializer & deserializer);
		text_component(text_component && other);
	};

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, text_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const text_component & component);
}

#endif