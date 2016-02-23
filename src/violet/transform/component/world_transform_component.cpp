// ============================================================================

#include <assert.h>

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/transform/component/world_transform_component.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * World_Transform_Component::metadata;

// ============================================================================

World_Transform_Component::World_Transform_Component(const Handle entity_id) :
	World_Transform_Component(entity_id, m4::Identity)
{
}

// ----------------------------------------------------------------------------

World_Transform_Component::World_Transform_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	World_Transform_Component(entity_id, m4::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

World_Transform_Component::World_Transform_Component(const Handle entity_id, const m4 & _transform) :
	transform(_transform)
{
}

// ============================================================================

void vlt::install_world_transform_component()
{
	World_Transform_Component::metadata = init_component_metadata(Tag('w', 't', 'f', 'm'), 1, sizeof(World_Transform_Component));
	Scene::install_component<World_Transform_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, World_Transform_Component & component)
{
	switch (deserializer.get_version())
	{
	case 0:
		m3 mat2d;
		deserializer >> mat2d;
		component.transform = from2d(mat2d);
		break;

	case 1:
		deserializer >> component.transform;
		break;

	default:
		assert(false);
		break;
	}

	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const World_Transform_Component & component)
{
	return serializer << component.transform;
}

// ============================================================================
