#ifndef VIOLET_MAPPED_DATA_COMPONENT_H
#define VIOLET_MAPPED_DATA_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/task/thread.h"

#include <map>

namespace vlt
{
	struct component_deserializer;

	struct VIOLET_API mapped_data_component final : public component_base<mapped_data_component, 0>
	{
		std::map<std::string, std::string> data;

		static tag get_tag_static();
		static thread get_thread_static();

		mapped_data_component(handle entity_id);
		mapped_data_component(handle entity_id, component_deserializer & deserializer);
		mapped_data_component(mapped_data_component && other);
	};

	VIOLET_API serializer & operator<<(serializer & serializer, const mapped_data_component & component);
}

#endif