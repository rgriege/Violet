#ifndef EDITOR_CLEAR_COMMAND_H
#define EDITOR_CLEAR_COMMAND_H

#include "editor/command/command.h"

#include <memory>
#include <string>

namespace edt
{
	struct EDITOR_API Clear_Command final : public Command
	{
		std::string temp_filename;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		Clear_Command();
		virtual ~Clear_Command() override;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
