// ============================================================================

#include <algorithm>
#include <mutex>
#include <vector>

#include "violet/log/log.h"

// ============================================================================

using namespace vlt;

namespace LogNamespace
{
	typedef std::pair<u32, std::unique_ptr<log_target>> target;
	std::vector<target> ms_targets;
	std::mutex ms_targetMutex;
	u32 ms_nextId = 0;
}

using namespace LogNamespace;

// ============================================================================

log_target::guard::guard(const u32 handle) :
	handle(handle)
{
}

// ----------------------------------------------------------------------------

log_target::guard::~guard()
{
	remove_log_target(handle);
}

// ============================================================================

u32 vlt::install_log_target(std::unique_ptr<log_target> && logTarget)
{
	const std::lock_guard<std::mutex> guard(ms_targetMutex);
	ms_targets.emplace_back(ms_nextId++, std::move(logTarget));
	return ms_targets.back().first;
}

// ----------------------------------------------------------------------------

void vlt::remove_log_target(const u32 id)
{
	const std::lock_guard<std::mutex> guard(ms_targetMutex);
	ms_targets.erase(std::remove_if(ms_targets.begin(), ms_targets.end(), [id](const target & target) { return target.first == id; }), ms_targets.end());
}

// ----------------------------------------------------------------------------

void vlt::log(const char * const entry)
{
	const std::lock_guard<std::mutex> guard(ms_targetMutex);
	for (const auto & target : ms_targets)
		target.second->log(entry);
}

// ============================================================================
