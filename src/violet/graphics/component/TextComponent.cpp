// ============================================================================

#include "violet/graphics/component/TextComponent.h"

#include "violet/component/ComponentDeserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/graphics/font/Font.h"
#include "violet/graphics/shader/Shader.h"

using namespace Violet;

// ============================================================================

Tag TextComponent::getStaticTag()
{
	return Tag('t', 'e', 'x', 't');
}

// ----------------------------------------------------------------------------

Thread TextComponent::getStaticThread()
{
	return Thread::Window;
}

// ============================================================================

TextComponent::TextComponent(const EntityId entityId, ComponentDeserializer & deserializer) :
	ComponentBase<TextComponent>(entityId),
	RenderComponentData(deserializer),
	m_text(deserializer.getString("str")),
	m_font(),
	m_color()
{
	const char * filename = deserializer.getString("font");
	const uint32 size = deserializer.getUint("size");
	m_font = Font::getCache().fetch(filename, size);
	deserializer >> m_color;
}

// ----------------------------------------------------------------------------

TextComponent::TextComponent(TextComponent && other) :
	ComponentBase<TextComponent>(std::move(other)),
	RenderComponentData(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font)),
	m_color(other.m_color)
{
}

// ============================================================================

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, TextComponent & component)
{
	operator>>(deserializer, static_cast<RenderComponentData &>(component));
	component.m_text = deserializer.getString("str");
	const char * filename = deserializer.getString("font");
	const uint32 size = deserializer.getUint("size");
	component.m_font = Font::getCache().fetch(filename, size);
	deserializer >> component.m_color;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const TextComponent & component)
{
	operator<<(serializer, static_cast<const RenderComponentData &>(component));
	serializer.writeString("str", component.m_text.c_str());
	serializer.writeString("font", component.m_font->getFilename());
	serializer.writeUint("size", component.m_font->getSize());
	serializer << component.m_color;
	return serializer;
}

// ============================================================================
