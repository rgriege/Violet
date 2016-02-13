// ============================================================================

#include "violet/input/component/key_input_component.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

tag key_input_component::get_tag_static()
{
	return tag('k', 'i', 'p', 't');
}

// ----------------------------------------------------------------------------

thread key_input_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

key_input_component::key_input_component(const handle entity_id) :
	component_base<key_input_component, 0>()
{
}

// ----------------------------------------------------------------------------

key_input_component::key_input_component(const handle entity_id, component_deserializer & /*deserializer*/) :
	key_input_component(entity_id)
{
}

// ----------------------------------------------------------------------------

key_input_component::key_input_component(key_input_component && other) :
	component_base<key_input_component, 0>(std::move(other))
{
}

// ============================================================================

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
