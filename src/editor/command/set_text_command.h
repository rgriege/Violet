#ifndef EDITOR_SET_TEXT_COMMAND_H
#define EDITOR_SET_TEXT_COMMAND_H

#include <memory>
#include <string>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API Set_Text_Command final : public Command
	{
		vlt::Handle entity_id;
		std::string text;

		static const char * get_usage();
		static std::unique_ptr<Command> parse(const std::string & text);

		Set_Text_Command(vlt::Handle _entity_id, const std::string & _text);
		virtual ~Set_Text_Command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
