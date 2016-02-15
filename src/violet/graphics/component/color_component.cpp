// ============================================================================

#include "violet/graphics/component/color_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/graphics/shader/shader.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const component_metadata * color_component::metadata;

// ============================================================================

color_component::color_component(const handle entity_id, component_deserializer & deserializer) :
	render_component_data(deserializer),
	color(deserializer)
{
}

// ----------------------------------------------------------------------------

color_component::color_component(const handle entity_id, const poly & poly, std::shared_ptr<shader_program> shader, const Color color) :
	render_component_data(poly, shader),
	color(color)
{
}

// ----------------------------------------------------------------------------

color_component::color_component(color_component && other) :
	render_component_data(std::move(other)),
	color(std::move(other.color))
{
	m_shader.swap(other.m_shader);
}

// ============================================================================

void vlt::install_color_component()
{
	color_component::metadata = init_component_metadata(tag('c', 'o', 'l', 'r'), 0, sizeof(color_component));
	scene::install_component<color_component>();
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, color_component & component)
{
	operator>>(deserializer, static_cast<render_component_data &>(component));
	deserializer >> component.color;
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const color_component & component)
{
	operator<<(serializer, static_cast<const render_component_data &>(component));
	serializer << component.color;
	return serializer;
}

// ============================================================================
