// ============================================================================

#include "violet/component/component_metadata.h"

#include <assert.h>
#include <mutex>
#include <unordered_map>

using namespace vlt;

// ============================================================================

std::unordered_map<tag, component_metadata> g_component_metadata_map;

// ============================================================================

u32 _get_next_component_flag()
{
	static u32 next_flag = 1;
	const u32 result = next_flag;
	next_flag <<= 1;
	assert(next_flag != 0);

	return result;
}

// ============================================================================

const component_metadata * vlt::init_component_metadata(const tag t, const u32 version, const u32 size)
{
	static std::mutex mutex;

	const std::lock_guard<std::mutex> guard(mutex);

	assert(g_component_metadata_map.find(t) == g_component_metadata_map.end());

	component_metadata & data = g_component_metadata_map[t];
	data.tag = t;
	data.flag = _get_next_component_flag();
	data.size = size;
	data.version = version;
	data.thread = 0;
	return &data;
}

// ----------------------------------------------------------------------------

void vlt::assign_component_to_thread(const tag t, const u32 thread)
{
	assert(g_component_metadata_map.find(t) != g_component_metadata_map.end());

	g_component_metadata_map[t].thread = thread;
}

// ----------------------------------------------------------------------------

const component_metadata * vlt::get_component_metadata(const tag t)
{
	assert(g_component_metadata_map.find(t) != g_component_metadata_map.end());
	return &g_component_metadata_map[t];
}

// ============================================================================

