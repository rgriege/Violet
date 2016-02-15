// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/mapped_data_component.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const component_metadata * mapped_data_component::metadata;

// ============================================================================

mapped_data_component::mapped_data_component(const handle entity_id) :
	data()
{
}

// ----------------------------------------------------------------------------

mapped_data_component::mapped_data_component(const handle entity_id, component_deserializer & deserializer) :
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
	data(std::move(other.data))
{
}

// ============================================================================

void vlt::install_mapped_data_component()
{
	mapped_data_component::metadata = init_component_metadata(tag('m', 'a', 'p', 'd'), 0, sizeof(mapped_data_component));
	scene::install_component<mapped_data_component>();
}

// ----------------------------------------------------------------------------

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
