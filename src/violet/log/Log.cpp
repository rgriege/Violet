// ============================================================================

#include "violet/log/Log.h"
#include "violet/log/LogTarget.h"

#include <algorithm>
#include <mutex>
#include <vector>

// ============================================================================

using namespace Violet;

namespace LogNamespace
{
	typedef std::pair<uint32, std::unique_ptr<LogTarget>> Target;
	std::vector<Target> ms_targets;
	std::mutex ms_targetMutex;
	uint32 ms_nextId = 0;
}

using namespace LogNamespace;

// ============================================================================

uint32 Log::installTarget(std::unique_ptr<LogTarget> && logTarget)
{
	const std::lock_guard<std::mutex> guard(ms_targetMutex);
	ms_targets.emplace_back(ms_nextId++, std::move(logTarget));
	return ms_targets.back().first;
}

// ----------------------------------------------------------------------------

void Log::removeTarget(const uint32 id)
{
	const std::lock_guard<std::mutex> guard(ms_targetMutex);
	ms_targets.erase(std::remove_if(ms_targets.begin(), ms_targets.end(), [id](const Target & target) { return target.first == id; }), ms_targets.end());
}

// ----------------------------------------------------------------------------

void Log::log(const char * const entry)
{
	const std::lock_guard<std::mutex> guard(ms_targetMutex);
	for (const auto & target : ms_targets)
		target.second->log(entry);
}

// ============================================================================
