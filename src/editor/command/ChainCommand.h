#ifndef EDITOR_ChainCommand_H
#define EDITOR_ChainCommand_H

#include "editor/command/Command.h"

#include <memory>
#include <vector>

namespace edt
{
	class EDITOR_API ChainCommand : public Command
	{
	public:

		ChainCommand(std::vector<std::unique_ptr<Command>> && commands);
		virtual ~ChainCommand() override = default;

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		ChainCommand(const ChainCommand &) = delete;
		ChainCommand & operator=(const ChainCommand &) = delete;

	private:

		std::vector<std::unique_ptr<Command>> m_commands;
	};
}

#endif

