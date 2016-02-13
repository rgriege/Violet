// ============================================================================

#include "violet/update/component/update_component.h"

#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

tag update_component::get_tag_static()
{
	return tag('u', 'p', 'd', 't');
}

// ----------------------------------------------------------------------------

thread update_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

update_component::update_component(const handle entity_id) :
	component_base<update_component, 0>()
{
}

// ----------------------------------------------------------------------------

update_component::update_component(const handle entity_id, component_deserializer & /*deserializer*/) :
	component_base<update_component, 0>()
{
}

// ----------------------------------------------------------------------------

update_component::update_component(update_component && other) :
	component_base<update_component, 0>(std::move(other))
{
}

// ============================================================================

component_deserializer & vlt::operator>>(component_deserializer & deserializer, update_component & /*component*/)
{
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const update_component & /*component*/)
{
	return serializer;
}

// ============================================================================
