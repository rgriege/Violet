#ifndef EDITOR_SELECT_COMMAND_H
#define EDITOR_SELECT_COMMAND_H

#include "editor/command/command.h"
#include "violet/core/handle.h"

#include <memory>
#include <string>
#include <vector>

namespace edt
{
	struct EDITOR_API select_command final : public command
	{
	public:

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

	public:

		explicit select_command(vlt::handle entity_id);
		explicit select_command(const std::vector<vlt::handle> & entityIds);
		explicit select_command(std::vector<vlt::handle> && entityIds);
		virtual ~select_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		std::vector<vlt::handle> m_entityIds;
	};
}

#endif
