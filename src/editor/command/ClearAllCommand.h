#ifndef EDITOR_ClearAllCommand_H
#define EDITOR_ClearAllCommand_H

#include "editor/command/Command.h"

#include <string>

namespace edt
{
	class EDITOR_API ClearAllCommand : public Command
	{
	public:

		ClearAllCommand();
		virtual ~ClearAllCommand() override;

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		std::string m_tempFileName;
	};
}

#endif