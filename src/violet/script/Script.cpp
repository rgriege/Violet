// ============================================================================

#include "violet/script/Script.h"

#include "violet/log/Log.h"

#include <assert.h>
#include <set>

using namespace Violet;

// ============================================================================

void Script::addHook(Script & script, const char * const name, void * hook)
{
	script.m_boundMethods.emplace(name, hook);
}

// ============================================================================

Script::~Script()
{
	assert(m_boundMethods.empty());
}

// ============================================================================

std::vector<const char *> Script::getBoundMethodNames() const
{
	std::vector<const char *> result;
	result.reserve(m_boundMethods.size());
	for (auto const & boundMethod : m_boundMethods)
		result.emplace_back(boundMethod.first);
	return result;
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
