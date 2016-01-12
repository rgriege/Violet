#ifndef EDITOR_DeselectCommand_H
#define EDITOR_DeselectCommand_H

#include "editor/command/Command.h"
#include "violet/Defines.h"

#include <memory>
#include <string>
#include <vector>

namespace edt
{
	class EDITOR_API DeselectCommand : public Command
	{
	public:

		static const char * getUsage();
		static std::unique_ptr<Command> parse(const std::string & text);

	public:

		explicit DeselectCommand(Violet::EntityId entityId);
		explicit DeselectCommand(const std::vector<Violet::EntityId> & entityIds);
		explicit DeselectCommand(std::vector<Violet::EntityId> && entityIds);
		virtual ~DeselectCommand() override = default;

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		std::vector<Violet::EntityId> m_entityIds;
	};
}

#endif
