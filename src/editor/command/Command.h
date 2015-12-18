#ifndef Editor_Command_H
#define Editor_Command_H

#include "editor/Defines.h"

namespace edt
{
	class EDITOR_API Command
	{
	public:

		virtual ~Command() = default;
		virtual void execute() = 0;
		virtual bool canUndo() const = 0;
		virtual void undo() = 0;
	};
}

#endif