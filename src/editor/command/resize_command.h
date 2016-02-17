#ifndef EDITOR_RESIZE_COMMAND_H
#define EDITOR_RESIZE_COMMAND_H

#include <memory>

#include "editor/command/command.h"
#include "violet/core/handle.h"
#include "violet/math/v2.h"

namespace edt
{
	struct EDITOR_API resize_command final : public command
	{
		vlt::handle entity_id;
		vlt::v2 dimensions;

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		resize_command(vlt::handle _entity_id, const vlt::v2 & _dimensions);
		virtual ~resize_command() override = default;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;
	};
}

#endif
