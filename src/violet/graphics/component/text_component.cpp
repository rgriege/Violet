// ============================================================================

#include "violet/graphics/component/text_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/graphics/font/font.h"
#include "violet/graphics/shader/shader.h"

using namespace vlt;

// ============================================================================

tag text_component::get_tag_static()
{
	return tag('t', 'e', 'x', 't');
}

// ----------------------------------------------------------------------------

thread text_component::get_thread_static()
{
	return thread::Window;
}

// ============================================================================

text_component::text_component(const handle entity_id, component_deserializer & deserializer) :
	component_base<text_component, 0>(),
	render_component_data(deserializer),
	m_text(deserializer.get_string("str")),
	m_font(),
	m_color()
{
	const char * filename = deserializer.get_string("font");
	const u32 size = deserializer.get_u32("size");
	m_font = font::get_cache().fetch(filename, size);
	deserializer >> m_color;
}

// ----------------------------------------------------------------------------

text_component::text_component(text_component && other) :
	component_base<text_component, 0>(std::move(other)),
	render_component_data(std::move(other)),
	m_text(std::move(other.m_text)),
	m_font(std::move(other.m_font)),
	m_color(other.m_color)
{
}

// ============================================================================

component_deserializer & vlt::operator>>(component_deserializer & deserializer, text_component & component)
{
	operator>>(deserializer, static_cast<render_component_data &>(component));
	component.m_text = deserializer.get_string("str");
	const char * filename = deserializer.get_string("font");
	const u32 size = deserializer.get_u32("size");
	component.m_font = font::get_cache().fetch(filename, size);
	deserializer >> component.m_color;
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const text_component & component)
{
	operator<<(serializer, static_cast<const render_component_data &>(component));
	serializer.write_string("str", component.m_text.c_str());
	serializer.write_string("font", component.m_font->get_filename());
	serializer.write_u32("size", component.m_font->get_size());
	serializer << component.m_color;
	return serializer;
}

// ============================================================================
