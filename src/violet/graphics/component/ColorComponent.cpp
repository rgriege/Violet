// ============================================================================

#include "violet/graphics/component/ColorComponent.h"

#include "violet/component/ComponentDeserializer.h"
#include "violet/graphics/shader/Shader.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag ColorComponent::getStaticTag()
{
	return Tag('c', 'o', 'l', 'r');
}

// ----------------------------------------------------------------------------

Thread ColorComponent::getStaticThread()
{
	return Thread::Window;
}

// ============================================================================

ColorComponent::ColorComponent(const EntityId entityId, ComponentDeserializer & deserializer) :
	ComponentBase<ColorComponent>(entityId),
	RenderComponentData(deserializer),
	m_color(deserializer)
{
}

// ----------------------------------------------------------------------------

ColorComponent::ColorComponent(const EntityId entityId, const Polygon & poly, std::shared_ptr<ShaderProgram> shader, const Color color) :
	ComponentBase<ColorComponent>(entityId),
	RenderComponentData(poly, shader),
	m_color(color)
{
}

// ----------------------------------------------------------------------------

ColorComponent::ColorComponent(ColorComponent && other) :
	ComponentBase<ColorComponent>(std::move(other)),
	RenderComponentData(std::move(other)),
	m_color(std::move(other.m_color))
{
	m_shader.swap(other.m_shader);
}

// ============================================================================

ComponentDeserializer & Violet::operator>>(ComponentDeserializer & deserializer, ColorComponent & component)
{
	operator>>(deserializer, static_cast<RenderComponentData &>(component));
	deserializer >> component.m_color;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const ColorComponent & component)
{
	operator<<(serializer, static_cast<const RenderComponentData &>(component));
	serializer << component.m_color;
	return serializer;
}

// ============================================================================
