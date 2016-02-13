// ============================================================================

#include "violet/component/component.h"

#include <assert.h>
#include <mutex>

using namespace vlt;

// ============================================================================

u32 component::get_next_flag()
{
	static u32 next_flag = 1;
	static std::mutex mutex;

	const std::lock_guard<std::mutex> guard(mutex);
	const u32 result = next_flag;
	next_flag <<= 1;
	assert(next_flag != 0);

	return result;
}

// ============================================================================

