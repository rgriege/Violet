// ============================================================================

#include "violet/script/script_factory.h"
#include "violet/utility/string_utilities.h"

using namespace vlt;

// ============================================================================

Script_Factory::Factory_Type g_instance;

// ============================================================================

void Script_Factory::assign(const char * const label, const Factory_Type::Producer & producer)
{
	g_instance.assign(label, producer);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Script> Script_Factory::create(const char * const filename)
{
	return g_instance.create(String_Utilities::rightOfLast(filename, '.'), filename);
}

// ----------------------------------------------------------------------------

void Script_Factory::remove(const char * const label)
{
	g_instance.remove(label);
}

// ============================================================================
