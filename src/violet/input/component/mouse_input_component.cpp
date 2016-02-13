// ============================================================================

#include "violet/input/component/mouse_input_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

tag mouse_input_component::get_tag_static()
{
	return tag('m', 'i', 'p', 't');
}

// ----------------------------------------------------------------------------

thread mouse_input_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

mouse_input_component::mouse_input_component(const handle entity_id, component_deserializer & deserializer) :
	component_base<mouse_input_component, 0>(),
	m_mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

mouse_input_component::mouse_input_component(const handle entity_id, poly && mesh) :
	component_base<mouse_input_component, 0>(),
	m_mesh(std::move(mesh))
{
}

// ----------------------------------------------------------------------------

mouse_input_component::mouse_input_component(mouse_input_component && other) :
	component_base<mouse_input_component, 0>(std::move(other)),
	m_mesh(std::move(other.m_mesh))
{
}

// ============================================================================

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
