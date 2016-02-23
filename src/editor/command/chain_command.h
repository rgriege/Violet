#ifndef EDITOR_CHAIN_COMMAND_H
#define EDITOR_CHAIN_COMMAND_H

#include "editor/command/command.h"

#include <memory>
#include <vector>

namespace edt
{
	struct EDITOR_API Chain_Command final : public Command
	{
	public:

		Chain_Command(std::vector<std::unique_ptr<Command>> && commands);
		virtual ~Chain_Command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		Chain_Command(const Chain_Command &) = delete;
		Chain_Command & operator=(const Chain_Command &) = delete;

	private:

		std::vector<std::unique_ptr<Command>> m_commands;
	};
}

#endif

