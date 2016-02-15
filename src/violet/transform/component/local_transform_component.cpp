// ============================================================================

#include <assert.h>

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/transform/component/local_transform_component.h"

using namespace vlt;

// ============================================================================

const component_metadata * local_transform_component::metadata;

// ============================================================================

local_transform_component::local_transform_component(const handle entity_id, component_deserializer & deserializer) :
	local_transform_component(entity_id, handle::Invalid, m4::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

local_transform_component::local_transform_component(const handle entity_id, const handle parentId, const m4 & transform) :
	parent_id(parentId),
	transform(transform)
{
}

// ============================================================================

void vlt::install_local_transform_component()
{
	local_transform_component::metadata = init_component_metadata(tag('l', 't', 'f', 'm'), 1, sizeof(local_transform_component));
	scene::install_component<local_transform_component>();
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, local_transform_component & component)
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

serializer & vlt::operator<<(serializer & serializer, const local_transform_component & component)
{
	serializer.write_u32("parentId", component.parent_id.id);
	return serializer << component.transform;
}

// ============================================================================
