#ifndef VIOLET_TextComponent_H
#define VIOLET_TextComponent_H

#include "violet/component/Component.h"
#include "violet/graphics/component/RenderComponentData.h"

#include <string>
#include <memory>

namespace Violet
{
	class Deserializer;
	class Font;
	class Serializer;

	class VIOLET_API TextComponent : public ComponentBase<TextComponent>, public RenderComponentData
	{
	public:

		static Tag getStaticTag();

	public:

		TextComponent(const Entity & owner, Deserializer & deserializer);
		TextComponent(TextComponent && other);

	private:

		TextComponent(const TextComponent &) = delete;
		TextComponent & operator=(const TextComponent &) = delete;

	public:

		std::string m_text;
		std::shared_ptr<Font> m_font;
		uint32 m_size;
	};

	Deserializer & operator>>(Deserializer & deserializer, TextComponent & component);
	Serializer & operator<<(Serializer & serializer, const TextComponent & component);
}

#endif