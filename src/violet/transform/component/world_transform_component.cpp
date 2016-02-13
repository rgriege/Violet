// ============================================================================

#include "violet/transform/component/world_transform_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/serialization/serializer.h"

#include <assert.h>

using namespace vlt;

// ============================================================================

tag world_transform_component::get_tag_static()
{
	return tag('w', 't', 'f', 'm');
}

// ----------------------------------------------------------------------------

thread world_transform_component::get_thread_static()
{
	return thread::Any;
}

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
	component_base<world_transform_component, 1>(),
	transform(transform)
{
}

// ============================================================================

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
