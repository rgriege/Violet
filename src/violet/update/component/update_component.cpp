// ============================================================================

#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/update/component/update_component.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Update_Component::metadata;

// ============================================================================

Update_Component::Update_Component(const Handle entity_id)
{
}

// ----------------------------------------------------------------------------

Update_Component::Update_Component(const Handle entity_id, Component_Deserializer & /*deserializer*/)
{
}

// ----------------------------------------------------------------------------

Update_Component::Update_Component(Update_Component && other)
{
}

// ============================================================================

void vlt::install_update_component()
{
	Update_Component::metadata = init_component_metadata(Tag('u', 'p', 'd', 't'), 0, sizeof(Update_Component));
	Scene::install_component<Update_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Update_Component & /*component*/)
{
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Update_Component & /*component*/)
{
	return serializer;
}

// ============================================================================
