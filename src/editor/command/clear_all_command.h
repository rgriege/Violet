#ifndef EDITOR_CLEAR_ALL_COMMAND_H
#define EDITOR_CLEAR_ALL_COMMAND_H

#include "editor/command/command.h"

#include <memory>
#include <string>

namespace edt
{
	struct EDITOR_API clear_all_command final : public command
	{
	public:

		static const char * get_usage();
		static std::unique_ptr<command> parse(const std::string & text);

	public:

		clear_all_command();
		virtual ~clear_all_command() override;

		virtual void execute() override;
		virtual bool can_undo() const override;
		virtual void undo() override;

	private:

		std::string m_tempFileName;
	};
}

#endif