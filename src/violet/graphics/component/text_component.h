#ifndef VIOLET_TEXT_COMPONENT_H
#define VIOLET_TEXT_COMPONENT_H

#include <string>
#include <memory>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/graphics/color.h"
#include "violet/graphics/component/render_component_data.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Font;
	struct Serializer;

	struct VIOLET_API Text_Component final : public Render_Component_Data
	{
		std::string text;
		std::shared_ptr<Font> font;
		Color color;

		static const Component_Metadata * metadata;

		Text_Component(Handle entity_id, Component_Deserializer & deserializer);
		Text_Component(const Handle entity_id, const Text_Component & other);
		Text_Component(Text_Component && other);
	};

	VIOLET_API void install_text_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Text_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Text_Component & component);
}

#endif
