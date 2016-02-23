#ifndef VIOLET_UPDATE_COMPONENT_H
#define VIOLET_UPDATE_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Update_Component final
	{
		static const Component_Metadata * metadata;

		Update_Component(Handle entity_id);
		Update_Component(Handle entity_id, Component_Deserializer & deserializer);
		Update_Component(Update_Component && other);
	};

	VIOLET_API void install_update_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Update_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Update_Component & component);
}

#endif
