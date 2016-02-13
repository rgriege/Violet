// ============================================================================

#include "violet/component/component_deserializer.h"

#include <assert.h>

using namespace vlt;

// ============================================================================

component_deserializer::component_deserializer(std::unique_ptr<deserializer> && deserializer, const u32 version, std::shared_ptr<const std::unordered_map<u32, handle>> handleIdMap) :
	inner_deserializer(std::move(deserializer)),
	version(version),
	handleIdMap(std::move(handleIdMap))
{
}

// ----------------------------------------------------------------------------

b8 component_deserializer::is_valid() const
{
	return inner_deserializer->is_valid();
}

// ----------------------------------------------------------------------------


std::unique_ptr<deserializer> component_deserializer::enter_segment(const char * const label)
{
	return inner_deserializer->enter_segment(label);
}

// ----------------------------------------------------------------------------


const char * component_deserializer::next_label() const
{
	return inner_deserializer->next_label();
}

// ----------------------------------------------------------------------------


bool component_deserializer::get_b8(const char * const label)
{
	return inner_deserializer->get_b8(label);
}

// ----------------------------------------------------------------------------


u32 component_deserializer::get_u32(const char * const label)
{
	return inner_deserializer->get_u32(label);
}

// ----------------------------------------------------------------------------


s32 component_deserializer::get_s32(const char * const label)
{
	return inner_deserializer->get_s32(label);
}

// ----------------------------------------------------------------------------


r32 component_deserializer::get_r32(const char * const label)
{
	return inner_deserializer->get_r32(label);
}

// ----------------------------------------------------------------------------


r64 component_deserializer::get_r64(const char * const label)
{
	return inner_deserializer->get_r64(label);
}

// ----------------------------------------------------------------------------


const char * component_deserializer::get_string(const char * const label)
{
	return inner_deserializer->get_string(label);
}

// ----------------------------------------------------------------------------

u32 component_deserializer::get_version() const
{
	return version;
}

// ----------------------------------------------------------------------------

handle component_deserializer::get_entity_id(const char * const label) const
{
	const u32 desiredId = inner_deserializer->get_u32(label);
	const auto it = handleIdMap->find(desiredId);
	assert(!handle(desiredId, 0).is_valid() || it != handleIdMap->end());
	return it == handleIdMap->end() ? handle::Invalid : it->second;
}

// ============================================================================
