#ifndef EDITOR_SELECT_COMMAND_H
#define EDITOR_SELECT_COMMAND_H

#include <memory>
#include <string>
#include <vector>

#include "editor/command/command.h"
#include "violet/core/handle.h"

namespace edt
{
	struct EDITOR_API Select_Command final : public Command
	{
		std::vector<vlt::Handle> proxy_ids;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		explicit Select_Command(vlt::Handle proxy_id);
		explicit Select_Command(const std::vector<vlt::Handle> & proxy_ids);
		explicit Select_Command(std::vector<vlt::Handle> && proxy_ids);
		virtual ~Select_Command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
