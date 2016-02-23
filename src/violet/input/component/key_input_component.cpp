// ============================================================================

#include "violet/component/scene.h"
#include "violet/input/component/key_input_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Key_Input_Component::metadata;

// ============================================================================

Key_Input_Component::Key_Input_Component(const Handle entity_id)
{
}

// ----------------------------------------------------------------------------

Key_Input_Component::Key_Input_Component(const Handle entity_id, Component_Deserializer & /*deserializer*/)
{
}

// ============================================================================

void vlt::install_key_input_component()
{
	Key_Input_Component::metadata = init_component_metadata(Tag('k', 'i', 'p', 't'), 0, sizeof(Key_Input_Component));
	Scene::install_component<Key_Input_Component>();
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Key_Input_Component & component)
{
	return serializer;
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Key_Input_Component & component)
{
	return deserializer;
}

// ============================================================================
