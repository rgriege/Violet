#ifndef EDITOR_SET_TEXT_COMMAND_H
#define EDITOR_SET_TEXT_COMMAND_H

#include <memory>
#include <string>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API set_text_command final : public command
	{
		vlt::handle entity_id;
		std::string text;

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		set_text_command(vlt::handle _entity_id, const std::string & _text);
		virtual ~set_text_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
