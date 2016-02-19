// ============================================================================

#include "violet/utility/profiler.h"

using namespace vlt;

// ============================================================================

// L1 data cache size is 32kB ??
// L2 data cache size is 256kB ??
void profiler::flush_cache()
{
	static const int size = 1024 * 1024;
	r32 temp = 0;
	r32* data = new r32[size];
	for (int i = 0; i < size; i++)
		temp += data[i];
	delete[] data;
}

// ----------------------------------------------------------------------------

profiler & profiler::instance()
{
	static profiler s_profiler;
	return s_profiler;
}

// ============================================================================

profiler::block::block(std::string key, profiler & profiler) :
	m_key(std::move(key)),
	m_profiler(profiler),
	m_startTime(std::chrono::steady_clock::now())
{
}

// ----------------------------------------------------------------------------

profiler::block::~block()
{
	const auto endTime = std::chrono::steady_clock::now();
	m_profiler.add(m_key, std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count());
}


// ============================================================================

void profiler::add(const std::string & key, const s64 microseconds)
{
	m_counters[key] += microseconds;
}

// ----------------------------------------------------------------------------

void profiler::report(std::ostream & os, const bool clear)
{
	for (auto const & counter : m_counters)
		os << counter.first << ": " << counter.second << "us" << std::endl;

	if (clear)
		m_counters.clear();
}

// ----------------------------------------------------------------------------

s64 profiler::report(std::string const & key, const bool clear)
{
	const auto it = m_counters.find(key);
	if (it != m_counters.end())
	{
		const s64 result = it->second;
		if (clear)
			m_counters.erase(it);
		return result;
	}

	return -1;
}

// ============================================================================
