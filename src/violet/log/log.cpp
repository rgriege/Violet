// ============================================================================

#include <algorithm>
#include <mutex>
#include <vector>

#include "violet/log/log.h"

// ============================================================================

using namespace vlt;

namespace LogNamespace
{
	typedef std::pair<u32, std::unique_ptr<Log_Target>> Target;
	std::vector<Target> g_targets;
	std::mutex g_target_mutex;
	u32 ms_nextId = 0;
}

using namespace LogNamespace;

// ============================================================================

Log_Target::Guard::Guard(const u32 handle) :
	handle(handle)
{
}

// ----------------------------------------------------------------------------

Log_Target::Guard::~Guard()
{
	remove_log_target(handle);
}

// ============================================================================

u32 vlt::install_log_target(std::unique_ptr<Log_Target> && target)
{
	const std::lock_guard<std::mutex> guard(g_target_mutex);
	g_targets.emplace_back(ms_nextId++, std::move(target));
	return g_targets.back().first;
}

// ----------------------------------------------------------------------------

void vlt::remove_log_target(const u32 id)
{
	const std::lock_guard<std::mutex> guard(g_target_mutex);
	g_targets.erase(std::remove_if(g_targets.begin(), g_targets.end(), [id](const Target & target) { return target.first == id; }), g_targets.end());
}

// ----------------------------------------------------------------------------

void vlt::log(const char * const entry)
{
	const std::lock_guard<std::mutex> guard(g_target_mutex);
	for (const auto & target : g_targets)
		target.second->log(entry);
}

// ============================================================================
