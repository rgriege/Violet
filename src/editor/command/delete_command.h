#ifndef EDITOR_DELETE_COMMAND_H
#define EDITOR_DELETE_COMMAND_H

#include <memory>
#include <string>

#include "violet/core/handle.h"

#include "editor/command/command.h"

namespace edt
{
	struct EDITOR_API Delete_Command final : public Command
	{
		vlt::Handle root_proxy_id;
		std::string temp_filename;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		Delete_Command(vlt::Handle _entity_id);
		virtual ~Delete_Command() override;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
