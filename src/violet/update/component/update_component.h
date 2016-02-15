#ifndef VIOLET_UPDATE_COMPONENT_H
#define VIOLET_UPDATE_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API update_component final
	{
		static const component_metadata * metadata;

		update_component(handle entity_id);
		update_component(handle entity_id, component_deserializer & deserializer);
		update_component(update_component && other);
	};

	VIOLET_API void install_update_component();

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, update_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const update_component & component);
}

#endif