// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/graphics/component/text_component.h"
#include "violet/graphics/font/font.h"
#include "violet/graphics/shader/shader.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Text_Component::metadata;

// ============================================================================

Text_Component::Text_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	Render_Component_Data(deserializer),
	text(deserializer.get_string("str")),
	font(),
	color()
{
	const char * filename = deserializer.get_string("Font");
	const u32 size = deserializer.get_u32("size");
	font = Font::get_cache().fetch(filename, size);
	deserializer >> color;
}

// ----------------------------------------------------------------------------

Text_Component::Text_Component(const Handle entity_id, const Text_Component & other) :
	Render_Component_Data(other),
	text(other.text),
	font(other.font),
	color(other.color)
{
}

// ----------------------------------------------------------------------------

Text_Component::Text_Component(Text_Component && other) :
	Render_Component_Data(std::move(other)),
	text(std::move(other.text)),
	font(std::move(other.font)),
	color(other.color)
{
}

// ============================================================================

void vlt::install_text_component()
{
	Text_Component::metadata = init_component_metadata(Tag('t', 'e', 'x', 't'), 0, sizeof(Text_Component));
	Scene::install_component<Text_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Text_Component & component)
{
	operator>>(deserializer, static_cast<Render_Component_Data &>(component));
	component.text = deserializer.get_string("str");
	const char * filename = deserializer.get_string("font");
	const u32 size = deserializer.get_u32("size");
	component.font = Font::get_cache().fetch(filename, size);
	deserializer >> component.color;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Text_Component & component)
{
	operator<<(serializer, static_cast<const Render_Component_Data &>(component));
	serializer.write_string("str", component.text.c_str());
	serializer.write_string("font", component.font->filename.c_str());
	serializer.write_u32("size", component.font->size);
	serializer << component.color;
	return serializer;
}

// ============================================================================
