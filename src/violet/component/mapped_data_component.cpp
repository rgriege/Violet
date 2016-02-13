// ============================================================================

#include "violet/component/mapped_data_component.h"

#include "violet/component/component_deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

tag mapped_data_component::get_tag_static()
{
	return tag('m', 'a', 'p', 'd');
}

// ----------------------------------------------------------------------------

thread mapped_data_component::get_thread_static()
{
	return thread::Any;
}

// ============================================================================

mapped_data_component::mapped_data_component(const handle entity_id) :
	component_base<mapped_data_component, 0>(),
	data()
{
}

// ----------------------------------------------------------------------------

mapped_data_component::mapped_data_component(const handle entity_id, component_deserializer & deserializer) :
	component_base<mapped_data_component, 0>(),
	data()
{
	const u32 n = deserializer.get_u32("n");
	for (u32 i = 0; i < n; ++i)
	{
		const char * const key = deserializer.get_string("key");
		data[key] = deserializer.get_string("value");
	}
}

// ----------------------------------------------------------------------------

mapped_data_component::mapped_data_component(mapped_data_component && other) :
	component_base<mapped_data_component, 0>(std::move(other)),
	data(std::move(other.data))
{
}

// ============================================================================

serializer & vlt::operator<<(serializer & serializer, const mapped_data_component & component)
{
	serializer.write_u32("n", component.data.size());
	for (auto const & kvp : component.data)
	{
		serializer.write_string("key", kvp.first.c_str());
		serializer.write_string("value", kvp.second.c_str());
	}
	return serializer;
}

// ============================================================================
