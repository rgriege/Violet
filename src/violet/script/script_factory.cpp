// ============================================================================

#include "violet/script/script_factory.h"
#include "violet/utility/string_utilities.h"

using namespace vlt;

// ============================================================================

script_factory::factory_type script_factory::ms_instance;

// ============================================================================

void script_factory::assign(const char * const label, const factory_type::producer & producer)
{
	ms_instance.assign(label, producer);
}

// ----------------------------------------------------------------------------

std::unique_ptr<script> script_factory::create(const char * const fileName)
{
	return ms_instance.create(string_utilities::rightOfLast(fileName, '.'), fileName);
}

// ----------------------------------------------------------------------------

void script_factory::remove(const char * const label)
{
	ms_instance.remove(label);
}

// ============================================================================
