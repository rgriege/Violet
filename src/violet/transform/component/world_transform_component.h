#ifndef VIOLET_WORLD_TRANSFORM_COMPONENT_H
#define VIOLET_WORLD_TRANSFORM_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/m4.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API World_Transform_Component final
	{
		m4 transform;

		static const Component_Metadata * metadata;

		World_Transform_Component(Handle entity_id);
		World_Transform_Component(Handle entity_id, Component_Deserializer & deserializer);
		World_Transform_Component(Handle entity_id, const m4 & transform);
	};

	VIOLET_API void install_world_transform_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, World_Transform_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const World_Transform_Component & component);
}

#endif
