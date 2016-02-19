#ifndef EDITOR_COMMAND_H
#define EDITOR_COMMAND_H

#include "editor/Defines.h"

namespace edt
{
	struct EDITOR_API command
	{
		virtual ~command() = default;
		virtual void execute() = 0;
		virtual bool can_undo() const = 0;
		virtual void undo() = 0;
	};
}

#endif