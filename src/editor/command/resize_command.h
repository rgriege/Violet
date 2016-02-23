#ifndef EDITOR_RESIZE_COMMAND_H
#define EDITOR_RESIZE_COMMAND_H

#include <memory>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API Resize_Command final : public Command
	{
		vlt::Handle entity_id;
		vlt::v2 dimensions;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		Resize_Command(vlt::Handle _entity_id, const vlt::v2 & _dimensions);
		virtual ~Resize_Command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
