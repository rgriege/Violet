// ============================================================================

#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/update/component/update_component.h"

using namespace vlt;

// ============================================================================

const component_metadata * update_component::metadata;

// ============================================================================

update_component::update_component(const handle entity_id)
{
}

// ----------------------------------------------------------------------------

update_component::update_component(const handle entity_id, component_deserializer & /*deserializer*/)
{
}

// ----------------------------------------------------------------------------

update_component::update_component(update_component && other)
{
}

// ============================================================================

void vlt::install_update_component()
{
	update_component::metadata = init_component_metadata(tag('u', 'p', 'd', 't'), 0, sizeof(update_component));
	scene::install_component<update_component>();
}

// ----------------------------------------------------------------------------

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
