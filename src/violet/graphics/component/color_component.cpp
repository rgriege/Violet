// ============================================================================

#include "violet/graphics/component/color_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/graphics/shader/shader.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

tag color_component::get_tag_static()
{
	return tag('c', 'o', 'l', 'r');
}

// ----------------------------------------------------------------------------

thread color_component::get_thread_static()
{
	return thread::Window;
}

// ============================================================================

color_component::color_component(const handle entity_id, component_deserializer & deserializer) :
	component_base<color_component, 0>(),
	render_component_data(deserializer),
	m_color(deserializer)
{
}

// ----------------------------------------------------------------------------

color_component::color_component(const handle entity_id, const poly & poly, std::shared_ptr<shader_program> shader, const color color) :
	component_base<color_component, 0>(),
	render_component_data(poly, shader),
	m_color(color)
{
}

// ----------------------------------------------------------------------------

color_component::color_component(color_component && other) :
	component_base<color_component, 0>(std::move(other)),
	render_component_data(std::move(other)),
	m_color(std::move(other.m_color))
{
	m_shader.swap(other.m_shader);
}

// ============================================================================

component_deserializer & vlt::operator>>(component_deserializer & deserializer, color_component & component)
{
	operator>>(deserializer, static_cast<render_component_data &>(component));
	deserializer >> component.m_color;
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const color_component & component)
{
	operator<<(serializer, static_cast<const render_component_data &>(component));
	serializer << component.m_color;
	return serializer;
}

// ============================================================================
