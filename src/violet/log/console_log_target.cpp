// ============================================================================

#include <iostream>

#include "violet/log/console_log_target.h"

using namespace vlt;

// ============================================================================

Console_Log_Target::~Console_Log_Target()
{
	std::cout << std::flush;
}

// ----------------------------------------------------------------------------

void Console_Log_Target::log(const char * const entry)
{
	std::cout << entry << std::endl;
}

// ============================================================================
