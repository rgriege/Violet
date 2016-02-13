#ifndef EDITOR_OPEN_COMMAND_H
#define EDITOR_OPEN_COMMAND_H

#include <memory>
#include <string>
#include <vector>

#include "editor/command/command.h"
#include "violet/core/defines.h"
#include "violet/core/handle.h"

namespace edt
{
	struct EDITOR_API open_command final : public command
	{
		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		open_command(std::string filename);

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		const std::string filename;
		std::vector<vlt::handle> entity_ids;
	};
}

#endif