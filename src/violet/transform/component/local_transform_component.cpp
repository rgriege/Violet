// ============================================================================

#include "violet/transform/component/local_transform_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/serialization/serializer.h"

#include <assert.h>

using namespace vlt;

// ============================================================================

tag local_transform_component::get_tag_static()
{
	return tag('l', 't', 'f', 'm');
}

// ----------------------------------------------------------------------------

thread local_transform_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

local_transform_component::local_transform_component(const handle entity_id, component_deserializer & deserializer) :
	local_transform_component(entity_id, handle::Invalid, m4::Identity)
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

local_transform_component::local_transform_component(const handle entity_id, const handle parentId, const m4 & transform) :
	component_base<local_transform_component, 1>(),
	parent_id(parentId),
	transform(transform)
{
}

// ============================================================================

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
