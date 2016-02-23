#ifndef EDITOR_COMMAND_H
#define EDITOR_COMMAND_H

#include "editor/defines.h"

namespace edt
{
	struct EDITOR_API Command
	{
		virtual ~Command() = default;
		virtual void execute() = 0;
		virtual bool can_undo() const = 0;
		virtual void undo() = 0;
	};
}

#endif