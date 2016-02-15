// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const component_metadata * mouse_input_component::metadata;

// ============================================================================

mouse_input_component::mouse_input_component(const handle entity_id, component_deserializer & deserializer) :
	m_mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

mouse_input_component::mouse_input_component(const handle entity_id, poly && mesh) :
	m_mesh(std::move(mesh))
{
}

// ----------------------------------------------------------------------------

mouse_input_component::mouse_input_component(mouse_input_component && other) :
	m_mesh(std::move(other.m_mesh))
{
}

// ============================================================================

void vlt::install_mouse_input_component()
{
	mouse_input_component::metadata = init_component_metadata(tag('m', 'i', 'p', 't'), 0, sizeof(mouse_input_component));
	scene::install_component<mouse_input_component>();
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const mouse_input_component & component)
{
	return serializer << component.m_mesh;
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, mouse_input_component & component)
{
	deserializer >> component.m_mesh;
	return deserializer;
}

// ============================================================================
