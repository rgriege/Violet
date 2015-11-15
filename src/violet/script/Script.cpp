// ============================================================================

#include "violet/script/Script.h"

#include "violet/log/Log.h"

#include <assert.h>
#include <set>

using namespace Violet;

// ============================================================================

void Script::addHook(Script & script, const uint32 id, void * hook)
{
	script.m_boundMethods.emplace(id, hook);
}

// ============================================================================

Script::~Script()
{
	assert(m_boundMethods.empty());
}

// ============================================================================

void Script::warn(const char * const procedureName, const char * const context) const
{
	static std::set<std::string> s_warnings;

	std::string warning = getFileName() + "(" + procedureName + "): " + context;
	auto it = s_warnings.find(warning);
	if (it == s_warnings.end())
	{
		Log::log(warning.c_str());
		s_warnings.insert(warning);
	}
}

// ============================================================================
