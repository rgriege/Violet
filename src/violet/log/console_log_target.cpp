// ============================================================================

#include "violet/log/console_log_target.h"

#include <iostream>

using namespace vlt;

// ============================================================================

console_log_target::~console_log_target()
{
	std::cout << std::flush;
}

// ----------------------------------------------------------------------------

void console_log_target::log(const char * const entry)
{
	std::cout << entry << std::endl;
}

// ============================================================================
