// ============================================================================

#include "violet/graphics/component/TextComponent.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/graphics/font/Font.h"
#include "violet/graphics/shader/Shader.h"

using namespace Violet;

// ============================================================================

Tag TextComponent::getStaticTag()
{
	return Tag('t', 'e', 'x', 't');
}

// ============================================================================

TextComponent::TextComponent(Entity & owner, Deserializer & deserializer) :
	ComponentBase<TextComponent>(owner),
	RenderComponentData(deserializer),
	m_text(deserializer.getString("str")),
	m_font()
{
	const char * filename = deserializer.getString("font");
	const uint32 size = deserializer.getUint("size");
	m_font = Font::getCache().fetch(filename, size);
}

// ----------------------------------------------------------------------------

TextComponent::TextComponent(TextComponent && other) :
	ComponentBase<TextComponent>(std::move(other)),
	RenderComponentData(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, TextComponent & component)
{
	operator>>(deserializer, static_cast<RenderComponentData &>(component));
	component.m_text = deserializer.getString("str");
	const char * filename = deserializer.getString("font");
	const uint32 size = deserializer.getUint("size");
	component.m_font = Font::getCache().fetch(filename, size);
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const TextComponent & component)
{
	operator<<(serializer, static_cast<const RenderComponentData &>(component));
	serializer.writeString("str", component.m_text.c_str());
	serializer.writeString("font", component.m_font->getFilename());
	serializer.writeUint("size", component.m_font->getSize());
	return serializer;
}

// ============================================================================
