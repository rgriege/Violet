#ifndef EDITOR_CppScriptExports_H
#define EDITOR_CppScriptExports_H

#include "editor/command/ClearAllCommand.h"
#include "editor/command/file/OpenCommand.h"

#include <memory>

namespace edt
{
	EDITOR_API std::unique_ptr<ClearAllCommand> createClearAllCommand();
	EDITOR_API std::unique_ptr<OpenCommand> createOpenCommand(std::string fileName);
}

#endif