#ifndef EDITOR_SaveAllCommand_H
#define EDITOR_SaveAllCommand_H

#include "editor/command/Command.h"
#include "violet/Defines.h"

#include <string>

namespace edt
{
	class EDITOR_API SaveAllCommand final : public Command
	{
	public:

		SaveAllCommand(std::string fileName);

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		const std::string m_fileName;
	};
}

#endif