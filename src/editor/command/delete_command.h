#ifndef EDITOR_DELETE_COMMAND_H
#define EDITOR_DELETE_COMMAND_H

#include <memory>
#include <string>

#include "violet/core/handle.h"

#include "editor/command/command.h"

namespace edt
{
	struct EDITOR_API delete_command final : public command
	{
		vlt::handle entity_id;
		std::string temp_filename;

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		delete_command(vlt::handle _entity_id);
		virtual ~delete_command() override;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif