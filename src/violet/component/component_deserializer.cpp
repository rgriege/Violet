// ============================================================================

#include "violet/component/component_deserializer.h"

#include <assert.h>

using namespace vlt;

// ============================================================================

Component_Deserializer::Component_Deserializer(std::unique_ptr<Deserializer> && _deserializer, const u32 _version, std::shared_ptr<const std::unordered_map<u32, Handle>> _handle_id_map) :
	inner_deserializer(std::move(_deserializer)),
	version(_version),
	handle_id_map(std::move(_handle_id_map))
{
}

// ----------------------------------------------------------------------------

b8 Component_Deserializer::is_valid() const
{
	return inner_deserializer->is_valid();
}

// ----------------------------------------------------------------------------


std::unique_ptr<Deserializer> Component_Deserializer::enter_segment(const char * const label)
{
	return inner_deserializer->enter_segment(label);
}

// ----------------------------------------------------------------------------


const char * Component_Deserializer::next_label() const
{
	return inner_deserializer->next_label();
}

// ----------------------------------------------------------------------------


bool Component_Deserializer::get_b8(const char * const label)
{
	return inner_deserializer->get_b8(label);
}

// ----------------------------------------------------------------------------


u32 Component_Deserializer::get_u32(const char * const label)
{
	return inner_deserializer->get_u32(label);
}

// ----------------------------------------------------------------------------


s32 Component_Deserializer::get_s32(const char * const label)
{
	return inner_deserializer->get_s32(label);
}

// ----------------------------------------------------------------------------


r32 Component_Deserializer::get_r32(const char * const label)
{
	return inner_deserializer->get_r32(label);
}

// ----------------------------------------------------------------------------


r64 Component_Deserializer::get_r64(const char * const label)
{
	return inner_deserializer->get_r64(label);
}

// ----------------------------------------------------------------------------


const char * Component_Deserializer::get_string(const char * const label)
{
	return inner_deserializer->get_string(label);
}

// ----------------------------------------------------------------------------

u32 Component_Deserializer::get_version() const
{
	return version;
}

// ----------------------------------------------------------------------------

Handle Component_Deserializer::get_entity_id(const char * const label) const
{
	const u32 desired_id = inner_deserializer->get_u32(label);
	const auto it = handle_id_map->find(desired_id);
	assert(!Handle(desired_id, 0).is_valid() || it != handle_id_map->end());
	return it == handle_id_map->end() ? Handle::Invalid : it->second;
}

// ============================================================================
