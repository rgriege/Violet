#ifndef VIOLET_COLOR_COMPONENT_H
#define VIOLET_COLOR_COMPONENT_H

#include <memory>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/graphics/color.h"
#include "violet/graphics/component/render_component_data.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;
	struct Shader_Program;

	struct VIOLET_API Color_Component final : public Render_Component_Data
	{
		Color color;

		static const Component_Metadata * metadata;

		Color_Component(Handle entity_id, Component_Deserializer & deserializer);
		Color_Component(Handle entity_id, const Poly & poly, std::shared_ptr<Shader_Program> shader, Color color);
		Color_Component(const Color_Component &) = delete;
		Color_Component & operator=(const Color_Component &) = delete;
		Color_Component(Color_Component && other);
	};

	VIOLET_API void install_color_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Color_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Color_Component & component);
}

#endif
