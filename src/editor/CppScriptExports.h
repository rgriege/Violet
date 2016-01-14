#ifndef EDITOR_CppScriptExports_H
#define EDITOR_CppScriptExports_H

#include "editor/command/ChainCommand.h"
#include "editor/command/ClearAllCommand.h"
#include "editor/command/MoveToCommand.h"
#include "editor/command/file/OpenCommand.h"
#include "editor/command/file/SaveAllCommand.h"
#include "editor/command/selection/DeselectCommand.h"
#include "editor/command/selection/SelectCommand.h"

#include <memory>

namespace edt
{
	EDITOR_API std::unique_ptr<ChainCommand> createChainCommand(std::vector<std::unique_ptr<Command>> && commands);
	EDITOR_API std::unique_ptr<ClearAllCommand> createClearAllCommand();
	EDITOR_API std::unique_ptr<DeselectCommand> createDeselectCommand(Violet::EntityId entityId);
	EDITOR_API std::unique_ptr<DeselectCommand> createDeselectCommand(std::vector<Violet::EntityId> && entityIds);
	EDITOR_API std::unique_ptr<MoveToCommand> createMoveToCommand(Violet::EntityId entityId, const Violet::Vec2f & position);
	EDITOR_API std::unique_ptr<OpenCommand> createOpenCommand(std::string fileName);
	EDITOR_API std::unique_ptr<SaveAllCommand> createSaveAllCommand(std::string fileName);
	EDITOR_API std::unique_ptr<SelectCommand> createSelectCommand(Violet::EntityId entityId);
}

#endif
