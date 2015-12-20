#ifndef EDITOR_ClearAllCommand_H
#define EDITOR_ClearAllCommand_H

#include "editor/command/Command.h"

#include <memory>
#include <string>

namespace edt
{
	class EDITOR_API ClearAllCommand : public Command
	{
	public:

		static const char * getUsage();
		static std::unique_ptr<Command> parse(const std::string & text);

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