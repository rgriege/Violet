#ifndef Editor_OpenCommand_H
#define Editor_OpenCommand_H

#include "editor/command/Command.h"
#include "violet/Defines.h"

#include <string>
#include <vector>

namespace edt
{
	class EDITOR_API OpenCommand final : public Command
	{
	public:

		OpenCommand(std::string fileName);

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		const std::string m_fileName;
		std::vector<Violet::EntityId> m_entityIds;
	};
}

#endif