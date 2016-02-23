#ifndef VIOLET_MAPPED_DATA_COMPONENT_H
#define VIOLET_MAPPED_DATA_COMPONENT_H

#include <map>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Mapped_Data_Component final
	{
		std::map<std::string, std::string> data;

		static const Component_Metadata * metadata;

		Mapped_Data_Component(Handle entity_id);
		Mapped_Data_Component(Handle entity_id, Component_Deserializer & Deserializer);
		Mapped_Data_Component(Mapped_Data_Component && other);
	};

	VIOLET_API void install_mapped_data_component();

	VIOLET_API Serializer & operator<<(Serializer & serializer, const Mapped_Data_Component & component);
}

#endif
