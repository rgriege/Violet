// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/graphics/component/text_component.h"
#include "violet/graphics/font/font.h"
#include "violet/graphics/shader/shader.h"

using namespace vlt;

// ============================================================================

const component_metadata * text_component::metadata;

// ============================================================================

text_component::text_component(const handle entity_id, component_deserializer & deserializer) :
	render_component_data(deserializer),
	m_text(deserializer.get_string("str")),
	m_font(),
	color()
{
	const char * filename = deserializer.get_string("font");
	const u32 size = deserializer.get_u32("size");
	m_font = font::get_cache().fetch(filename, size);
	deserializer >> color;
}

// ----------------------------------------------------------------------------

text_component::text_component(const handle entity_id, const text_component & other) :
	render_component_data(other),
	m_text(other.m_text),
	m_font(other.m_font),
	color(other.color)
{
}

// ----------------------------------------------------------------------------

text_component::text_component(text_component && other) :
	render_component_data(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font)),
	color(other.color)
{
}

// ============================================================================

void vlt::install_text_component()
{
	text_component::metadata = init_component_metadata(tag('t', 'e', 'x', 't'), 0, sizeof(text_component));
	scene::install_component<text_component>();
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, text_component & component)
{
	operator>>(deserializer, static_cast<render_component_data &>(component));
	component.m_text = deserializer.get_string("str");
	const char * filename = deserializer.get_string("font");
	const u32 size = deserializer.get_u32("size");
	component.m_font = font::get_cache().fetch(filename, size);
	deserializer >> component.color;
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const text_component & component)
{
	operator<<(serializer, static_cast<const render_component_data &>(component));
	serializer.write_string("str", component.m_text.c_str());
	serializer.write_string("font", component.m_font->get_filename());
	serializer.write_u32("size", component.m_font->get_size());
	serializer << component.color;
	return serializer;
}

// ============================================================================
