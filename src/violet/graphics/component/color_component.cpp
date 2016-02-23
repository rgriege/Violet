// ============================================================================

#include "violet/graphics/component/color_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/graphics/shader/shader.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Color_Component::metadata;

// ============================================================================

Color_Component::Color_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	Render_Component_Data(deserializer),
	color(deserializer)
{
}

// ----------------------------------------------------------------------------

Color_Component::Color_Component(const Handle entity_id, const Poly & poly, std::shared_ptr<Shader_Program> shader, const Color _color) :
	Render_Component_Data(poly, shader),
	color(_color)
{
}

// ----------------------------------------------------------------------------

Color_Component::Color_Component(Color_Component && other) :
	Render_Component_Data(std::move(other)),
	color(std::move(other.color))
{
	shader.swap(other.shader);
}

// ============================================================================

void vlt::install_color_component()
{
	Color_Component::metadata = init_component_metadata(Tag('c', 'o', 'l', 'r'), 0, sizeof(Color_Component));
	Scene::install_component<Color_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Color_Component & component)
{
	operator>>(deserializer, static_cast<Render_Component_Data &>(component));
	deserializer >> component.color;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Color_Component & component)
{
	operator<<(serializer, static_cast<const Render_Component_Data &>(component));
	serializer << component.color;
	return serializer;
}

// ============================================================================
