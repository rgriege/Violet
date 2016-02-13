// ============================================================================

#include "violet/script/script.h"

#include "violet/log/Log.h"

#include <assert.h>
#include <mutex>
#include <set>

using namespace vlt;

// ============================================================================

void script::add_hook(script & script, const u32 id, const delegate_store & hook)
{
	script.m_boundMethods.emplace(id, hook);
}

// ============================================================================

script::~script()
{
	assert(m_boundMethods.empty());
}

// ============================================================================

void script::warn(const char * const procedureName, const char * const context) const
{
	static std::set<std::string> s_warnings;
	static std::mutex s_mutex;

	const std::lock_guard<std::mutex> guard(s_mutex);
	std::string warning = get_filename() + "(" + procedureName + "): " + context;
	auto it = s_warnings.find(warning);
	if (it == s_warnings.end())
	{
		log(warning.c_str());
		s_warnings.insert(warning);
	}
}

// ============================================================================
