// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Mouse_Input_Component::metadata;

// ============================================================================

Mouse_Input_Component::Mouse_Input_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	mesh(deserializer)
{
}

// ----------------------------------------------------------------------------

Mouse_Input_Component::Mouse_Input_Component(const Handle entity_id, Poly && _mesh) :
	mesh(std::move(_mesh))
{
}

// ----------------------------------------------------------------------------

Mouse_Input_Component::Mouse_Input_Component(Mouse_Input_Component && other) :
	mesh(std::move(other.mesh))
{
}

// ============================================================================

void vlt::install_mouse_input_component()
{
	Mouse_Input_Component::metadata = init_component_metadata(Tag('m', 'i', 'p', 't'), 0, sizeof(Mouse_Input_Component));
	Scene::install_component<Mouse_Input_Component>();
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Mouse_Input_Component & component)
{
	return serializer << component.mesh;
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Mouse_Input_Component & component)
{
	deserializer >> component.mesh;
	return deserializer;
}

// ============================================================================
