#ifndef EDITOR_SAVE_ALL_COMMAND_H
#define EDITOR_SAVE_ALL_COMMAND_H

#include <memory>
#include <string>

#include "editor/command/command.h"
#include "violet/core/defines.h"

namespace edt
{
	struct EDITOR_API save_all_command final : public command
	{
		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

		save_all_command(std::string fileName);

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		const std::string m_fileName;
	};
}

#endif