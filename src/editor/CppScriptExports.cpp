// ============================================================================

#include "editor/CppScriptExports.h"

using namespace edt;
using namespace Violet;

// ============================================================================

std::unique_ptr<ClearAllCommand> edt::createClearAllCommand()
{
	return std::make_unique<ClearAllCommand>();
}

// ----------------------------------------------------------------------------

std::unique_ptr<MoveToCommand> edt::createMoveToCommand(const EntityId entityId, const Vec2f & position)
{
	return std::make_unique<MoveToCommand>(entityId, position);
}

// ----------------------------------------------------------------------------

std::unique_ptr<OpenCommand> edt::createOpenCommand(std::string fileName)
{
	return std::make_unique<OpenCommand>(std::move(fileName));
}

// ----------------------------------------------------------------------------

std::unique_ptr<SaveAllCommand> edt::createSaveAllCommand(std::string fileName)
{
	return std::make_unique<SaveAllCommand>(std::move(fileName));
}

// ============================================================================
