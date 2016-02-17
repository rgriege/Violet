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
	struct EDITOR_API set_color_command final : public command
	{
		vlt::handle entity_id;
		vlt::color new_color;

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		set_color_command(vlt::handle _entity_id, const vlt::color _new_color);
		virtual ~set_color_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
