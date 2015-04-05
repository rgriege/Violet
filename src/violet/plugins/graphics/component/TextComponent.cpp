#include "violet/plugins/graphics/component/TextComponent.h"

#include "violet/core/serialization/Deserializer.h"
#include "violet/plugins/graphics/font/Font.h"
#include "violet/plugins/graphics/shader/Shader.h"

using namespace Violet;

const char * TextComponent::getLabel()
{
	return "text";
}

TextComponent::TextComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_text(deserializer.getString("str")),
	m_font(Font::getCache().fetch(deserializer.getString("font"))),
	m_size(deserializer.getUint("size")),
	m_shader(ShaderProgram::getCache().fetch(deserializer.getString("shader")))
{
}

TextComponent::TextComponent(TextComponent && other) :
	Component(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font)),
	m_size(other.m_size),
	m_shader(std::move(other.m_shader))
{
}