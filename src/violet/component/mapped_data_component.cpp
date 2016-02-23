// ============================================================================

#include "violet/component/component_deserializer.h"
#include "violet/component/mapped_data_component.h"
#include "violet/component/scene.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

const Component_Metadata * Mapped_Data_Component::metadata;

// ============================================================================

Mapped_Data_Component::Mapped_Data_Component(const Handle entity_id) :
	data()
{
}

// ----------------------------------------------------------------------------

Mapped_Data_Component::Mapped_Data_Component(const Handle entity_id, Component_Deserializer & deserializer) :
	data()
{
	const u32 n = deserializer.get_u32("n");
	for (u32 i = 0; i < n; ++i)
	{
		const char * const key = deserializer.get_string("Key");
		data[key] = deserializer.get_string("value");
	}
}

// ----------------------------------------------------------------------------

Mapped_Data_Component::Mapped_Data_Component(Mapped_Data_Component && other) :
	data(std::move(other.data))
{
}

// ============================================================================

void vlt::install_mapped_data_component()
{
	Mapped_Data_Component::metadata = init_component_metadata(Tag('m', 'a', 'p', 'd'), 0, sizeof(Mapped_Data_Component));
	Scene::install_component<Mapped_Data_Component>();
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Mapped_Data_Component & component)
{
	serializer.write_u32("n", component.data.size());
	for (auto const & kvp : component.data)
	{
		serializer.write_string("Key", kvp.first.c_str());
		serializer.write_string("value", kvp.second.c_str());
	}
	return serializer;
}

// ============================================================================
