#ifndef VIOLET_LOCAL_TRANSFORM_COMPONENT_H
#define VIOLET_LOCAL_TRANSFORM_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/math/m4.h"
#include "violet/task/thread.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Local_Transform_Component final
	{
		Handle parent_id;
		m4 transform;

		static const Component_Metadata * metadata;

		Local_Transform_Component(Handle entity_id, Component_Deserializer & deserializer);
		Local_Transform_Component(Handle entity_id, Handle parentId, const m4 & transform);
	};

	VIOLET_API void install_local_transform_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Local_Transform_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Local_Transform_Component & component);
}

#endif
