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
	m_text(),
	m_font(),
	m_size(),
	m_shader()
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

TextComponent::TextComponent(TextComponent && other) :
	ComponentBase<TextComponent>(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font)),
	m_size(other.m_size),
	m_shader(std::move(other.m_shader))
{
}

// ----------------------------------------------------------------------------

//TextComponent & TextComponent::operator=(TextComponent && other)
//{
//	Component<TextComponent>::operator=(std::move(other));
//	std::swap(m_text, other.m_text);
//	std::swap(m_font, other.m_font);
//	std::swap(m_size, other.m_size);
//	std::swap(m_shader, other.m_shader);
//	return *this;
//}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, TextComponent & component)
{
	component.m_text = deserializer.getString("str");
	component.m_font = Font::getCache().fetch(deserializer.getString("font"));
	component.m_size = deserializer.getUint("size");
	component.m_shader = ShaderProgram::getCache().fetch(deserializer.getString("shader"));
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const TextComponent & component)
{
	serializer.writeString("str", component.m_text.c_str());
	serializer.writeString("font", component.m_font->getFilename());
	serializer.writeUint("size", component.m_size);
	serializer.writeString("shader", component.m_shader->getName().c_str());
	return serializer;
}

// ============================================================================
