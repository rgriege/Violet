// ============================================================================

#include <assert.h>

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"
#include "violet/transform/component/world_transform_component.h"

using namespace vlt;

// ============================================================================

const component_metadata * world_transform_component::metadata;

// ============================================================================

world_transform_component::world_transform_component(const handle entity_id) :
	world_transform_component(entity_id, m4::Identity)
{
}

// ----------------------------------------------------------------------------

world_transform_component::world_transform_component(const handle entity_id, component_deserializer & deserializer) :
	world_transform_component(entity_id, m4::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

world_transform_component::world_transform_component(const handle entity_id, const m4 & transform) :
	transform(transform)
{
}

// ============================================================================

void vlt::install_world_transform_component()
{
	world_transform_component::metadata = init_component_metadata(tag('w', 't', 'f', 'm'), 1, sizeof(world_transform_component));
	scene::install_component<world_transform_component>();
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, world_transform_component & component)
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

serializer & vlt::operator<<(serializer & serializer, const world_transform_component & component)
{
	return serializer << component.transform;
}

// ============================================================================
