// ============================================================================

#include "violet/log/ConsoleLogTarget.h"

#include <iostream>

using namespace Violet;

// ============================================================================

ConsoleLogTarget::~ConsoleLogTarget()
{
	std::cout << std::flush;
}

// ----------------------------------------------------------------------------

void ConsoleLogTarget::log(const char * const entry)
{
	std::cout << entry << std::endl;
}

// ============================================================================
