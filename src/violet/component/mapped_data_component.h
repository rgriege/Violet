#ifndef VIOLET_MAPPED_DATA_COMPONENT_H
#define VIOLET_MAPPED_DATA_COMPONENT_H

#include <map>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API mapped_data_component final
	{
		std::map<std::string, std::string> data;

		static const component_metadata * metadata;

		mapped_data_component(handle entity_id);
		mapped_data_component(handle entity_id, component_deserializer & deserializer);
		mapped_data_component(mapped_data_component && other);
	};

	VIOLET_API void install_mapped_data_component();

	VIOLET_API serializer & operator<<(serializer & serializer, const mapped_data_component & component);
}

#endif