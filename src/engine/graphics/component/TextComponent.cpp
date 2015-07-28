// ============================================================================

#include "engine/graphics/component/TextComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"
#include "engine/graphics/font/Font.h"
#include "engine/graphics/shader/Shader.h"

using namespace Violet;

// ============================================================================

Tag TextComponent::getTag()
{
	return Tag('t', 'e', 'x', 't');
}

// ============================================================================

TextComponent::TextComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<TextComponent>(owner),
	RenderComponentData(deserializer),
	m_text(deserializer.getString("str")),
	m_font(Font::getCache().fetch(deserializer.getString("font"))),
	m_size(deserializer.getUint("size"))
{
}

// ----------------------------------------------------------------------------

TextComponent::TextComponent(TextComponent && other) :
	ComponentBase<TextComponent>(std::move(other)),
	RenderComponentData(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font)),
	m_size(other.m_size)
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, TextComponent & component)
{
	operator>>(deserializer, static_cast<RenderComponentData &>(component));
	component.m_text = deserializer.getString("str");
	component.m_font = Font::getCache().fetch(deserializer.getString("font"));
	component.m_size = deserializer.getUint("size");
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const TextComponent & component)
{
	operator<<(serializer, static_cast<const RenderComponentData &>(component));
	serializer.writeString("str", component.m_text.c_str());
	serializer.writeString("font", component.m_font->getFilename());
	serializer.writeUint("size", component.m_size);
	return serializer;
}

// ============================================================================
