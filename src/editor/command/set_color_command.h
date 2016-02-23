#ifndef EDITOR_SET_COLOR_COMMAND_H
#define EDITOR_SET_COLOR_COMMAND_H

#include <memory>
#include <string>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/graphics/color.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API Set_Color_Command final : public Command
	{
		vlt::Handle entity_id;
		vlt::Color new_color;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		Set_Color_Command(vlt::Handle _entity_id, const vlt::Color _new_color);
		virtual ~Set_Color_Command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
