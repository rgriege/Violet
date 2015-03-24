#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include "violet/core/component/Component.h"

#include "violet/plugins/graphics/Config.h"

#include <string>
#include <memory>

namespace Violet
{
	class Deserializer;
	class Font;
	class ShaderProgram;

	class VIOLET_GRAPHICS_API TextComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		TextComponent(const Entity & entity, Deserializer & deserializer);
		TextComponent(TextComponent && other);
		TextComponent(const TextComponent &) = delete;

	public:

		std::string m_text;
		std::shared_ptr<Font> m_font;
		std::shared_ptr<ShaderProgram> m_shader;
	};
}

#endif