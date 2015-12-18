// ============================================================================

#include "violet/script/ScriptFactory.h"
#include "violet/utility/StringUtilities.h"

using namespace Violet;

// ============================================================================

ScriptFactory::FactoryType ScriptFactory::ms_instance;

// ============================================================================

void ScriptFactory::assign(const char * const label, const FactoryType::Producer & producer)
{
	ms_instance.assign(label, producer);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Script> ScriptFactory::create(const char * const fileName)
{
	return ms_instance.create(StringUtilities::rightOfLast(fileName, '.'), fileName);
}

// ----------------------------------------------------------------------------

void ScriptFactory::remove(const char * const label)
{
	ms_instance.remove(label);
}

// ============================================================================
