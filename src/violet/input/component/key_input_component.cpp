// ============================================================================

#include "violet/component/scene.h"
#include "violet/input/component/key_input_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const component_metadata * key_input_component::metadata;

// ============================================================================

key_input_component::key_input_component(const handle entity_id)
{
}

// ----------------------------------------------------------------------------

key_input_component::key_input_component(const handle entity_id, component_deserializer & /*deserializer*/)
{
}

// ----------------------------------------------------------------------------

key_input_component::key_input_component(key_input_component && other)
{
}

// ============================================================================

void vlt::install_key_input_component()
{
	key_input_component::metadata = init_component_metadata(tag('k', 'i', 'p', 't'), 0, sizeof(key_input_component));
	scene::install_component<key_input_component>();
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const key_input_component & component)
{
	return serializer;
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, key_input_component & component)
{
	return deserializer;
}

// ============================================================================
