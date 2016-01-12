#ifndef EDITOR_SelectCommand_H
#define EDITOR_SelectCommand_H

#include "editor/command/Command.h"
#include "violet/Defines.h"

#include <memory>
#include <string>
#include <vector>

namespace edt
{
	class EDITOR_API SelectCommand : public Command
	{
	public:

		static const char * getUsage();
		static std::unique_ptr<Command> parse(const std::string & text);

	public:

		explicit SelectCommand(Violet::EntityId entityId);
		explicit SelectCommand(const std::vector<Violet::EntityId> & entityIds);
		explicit SelectCommand(std::vector<Violet::EntityId> && entityIds);
		virtual ~SelectCommand() override = default;

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		std::vector<Violet::EntityId> m_entityIds;
	};
}

#endif
