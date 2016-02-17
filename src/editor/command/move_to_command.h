#ifndef EDITOR_MOVE_TO_COMMAND_H
#define EDITOR_MOVE_TO_COMMAND_H

#include <memory>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API move_to_command final : public command
	{
		vlt::handle entity_id;
		vlt::v2 position;

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		move_to_command(vlt::handle entity_id, const vlt::v2 & position);
		virtual ~move_to_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
