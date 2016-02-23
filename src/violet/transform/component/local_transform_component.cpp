// ============================================================================

#include <assert.h>

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/transform/component/local_transform_component.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Local_Transform_Component::metadata;

// ============================================================================

Local_Transform_Component::Local_Transform_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	Local_Transform_Component(entity_id, Handle::Invalid, m4::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

Local_Transform_Component::Local_Transform_Component(const Handle entity_id, const Handle _parent_id, const m4 & _transform) :
	parent_id(_parent_id),
	transform(_transform)
{
}

// ============================================================================

void vlt::install_local_transform_component()
{
	Local_Transform_Component::metadata = init_component_metadata(Tag('l', 't', 'f', 'm'), 1, sizeof(Local_Transform_Component));
	Scene::install_component<Local_Transform_Component>();
}

// ----------------------------------------------------------------------------

Component_Deserializer & vlt::operator>>(Component_Deserializer & deserializer, Local_Transform_Component & component)
{
	component.parent_id = deserializer.get_entity_id("parentId");

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

Serializer & vlt::operator<<(Serializer & serializer, const Local_Transform_Component & component)
{
	serializer.write_u32("parentId", component.parent_id.id);
	return serializer << component.transform;
}

// ============================================================================
