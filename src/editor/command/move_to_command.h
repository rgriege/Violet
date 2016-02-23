#ifndef EDITOR_MOVE_TO_COMMAND_H
#define EDITOR_MOVE_TO_COMMAND_H

#include <memory>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API Move_To_Command final : public Command
	{
		vlt::Handle entity_id;
		vlt::v2 position;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		Move_To_Command(vlt::Handle entity_id, const vlt::v2 & position);
		virtual ~Move_To_Command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
