#ifndef EDITOR_DESELECT_COMMAND_H
#define EDITOR_DESELECT_COMMAND_H

#include <memory>
#include <string>
#include <vector>

#include "editor/command/command.h"
#include "violet/core/defines.h"
#include "violet/core/handle.h"

namespace edt
{
	struct EDITOR_API deselect_command : public command
	{
		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		explicit deselect_command(vlt::handle entity_id);
		explicit deselect_command(const std::vector<vlt::handle> & entityIds);
		explicit deselect_command(std::vector<vlt::handle> && entityIds);
		virtual ~deselect_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		std::vector<vlt::handle> m_entityIds;
	};
}

#endif
