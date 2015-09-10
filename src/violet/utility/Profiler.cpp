// ============================================================================

#include "violet/utility/Profiler.h"

using namespace Violet;

// ============================================================================

// L1 data cache size is 32kB ??
// L2 data cache size is 256kB ??
void Profiler::flushCache()
{
	static const int size = 1024 * 1024;
	float temp = 0;
	float* data = new float[size];
	for (int i = 0; i < size; i++)
		temp += data[i];
	delete[] data;
}

// ----------------------------------------------------------------------------

Profiler & Profiler::getInstance()
{
	static Profiler s_profiler;
	return s_profiler;
}

// ============================================================================

Profiler::Block::Block(std::string key, Profiler & profiler) :
	m_key(std::move(key)),
	m_profiler(profiler),
	m_startTime(std::chrono::steady_clock::now())
{
}

// ----------------------------------------------------------------------------

Profiler::Block::~Block()
{
	const auto endTime = std::chrono::steady_clock::now();
	m_profiler.add(m_key, std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count());
}


// ============================================================================

void Profiler::add(const std::string & key, int64 microseconds)
{
	m_counters[key] += microseconds;
}

// ----------------------------------------------------------------------------

void Profiler::report(std::ostream & os, const bool clear)
{
	for (auto const & counter : m_counters)
		os << counter.first << ": " << counter.second << "us" << std::endl;

	if (clear)
		m_counters.clear();
}

// ----------------------------------------------------------------------------

int64 Profiler::report(std::string const & key, const bool clear)
{
	const auto it = m_counters.find(key);
	if (it != m_counters.end())
	{
		const int64 result = it->second;
		if (clear)
			m_counters.erase(it);
		return result;
	}

	return -1;
}

// ============================================================================
