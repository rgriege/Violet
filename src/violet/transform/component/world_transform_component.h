#ifndef VIOLET_WORLD_TRANSFORM_COMPONENT_H
#define VIOLET_WORLD_TRANSFORM_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/m4.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API world_transform_component final
	{
		m4 transform;

		static const component_metadata * metadata;

		world_transform_component(handle entity_id);
		world_transform_component(handle entity_id, component_deserializer & deserializer);
		world_transform_component(handle entity_id, const m4 & transform);
	};

	VIOLET_API void install_world_transform_component();

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, world_transform_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const world_transform_component & component);
}

#endif
