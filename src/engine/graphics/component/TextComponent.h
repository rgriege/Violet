#ifndef VIOLET_TextComponent_H
#define VIOLET_TextComponent_H

#include "engine/component/Component.h"

#include <string>
#include <memory>

namespace Violet
{
	class Deserializer;
	class Font;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API TextComponent : public ComponentBase<TextComponent>
	{
	public:

		static Tag getTag();

	public:

		TextComponent(const Entity & owner, Deserializer & deserializer);
		TextComponent(TextComponent && other);
		//TextComponent & operator=(TextComponent && other);

	public:

		std::string m_text;
		std::shared_ptr<Font> m_font;
		uint32 m_size;
		std::shared_ptr<ShaderProgram> m_shader;
	};

	Deserializer & operator>>(Deserializer & deserializer, TextComponent & component);
	Serializer & operator<<(Serializer & serializer, const TextComponent & component);
}

#endif