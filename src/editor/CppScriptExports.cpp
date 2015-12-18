// ============================================================================

#include "editor/CppScriptExports.h"

using namespace edt;

// ============================================================================

std::unique_ptr<ClearAllCommand> edt::createClearAllCommand()
{
	return std::make_unique<ClearAllCommand>();
}

// ----------------------------------------------------------------------------

std::unique_ptr<OpenCommand> edt::createOpenCommand(std::string fileName)
{
	return std::make_unique<OpenCommand>(std::move(fileName));
}

// ============================================================================
