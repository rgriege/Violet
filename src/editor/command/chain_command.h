#ifndef EDITOR_CHAIN_COMMAND_H
#define EDITOR_CHAIN_COMMAND_H

#include "editor/command/command.h"

#include <memory>
#include <vector>

namespace edt
{
	struct EDITOR_API chain_command final : public command
	{
	public:

		chain_command(std::vector<std::unique_ptr<command>> && commands);
		virtual ~chain_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		chain_command(const chain_command &) = delete;
		chain_command & operator=(const chain_command &) = delete;

	private:

		std::vector<std::unique_ptr<command>> m_commands;
	};
}

#endif

