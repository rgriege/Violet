#ifndef EDITOR_CLEAR_COMMAND_H
#define EDITOR_CLEAR_COMMAND_H

#include "editor/command/command.h"

#include <memory>
#include <string>

namespace edt
{
	struct EDITOR_API clear_command final : public command
	{
		std::string temp_filename;

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		clear_command();
		virtual ~clear_command() override;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif