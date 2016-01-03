#ifndef EDITOR_CppScriptExports_H
#define EDITOR_CppScriptExports_H

#include "editor/command/ClearAllCommand.h"
#include "editor/command/MoveToCommand.h"
#include "editor/command/file/OpenCommand.h"
#include "editor/command/file/SaveAllCommand.h"

#include <memory>

namespace edt
{
	EDITOR_API std::unique_ptr<ClearAllCommand> createClearAllCommand();
    EDITOR_API std::unique_ptr<MoveToCommand> createMoveToCommand(Violet::EntityId entityId, const Violet::Vec2f & position);
	EDITOR_API std::unique_ptr<OpenCommand> createOpenCommand(std::string fileName);
	EDITOR_API std::unique_ptr<SaveAllCommand> createSaveAllCommand(std::string fileName);
}

#endif
