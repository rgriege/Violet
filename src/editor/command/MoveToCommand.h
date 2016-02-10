#ifndef EDITOR_MoveToCommand_H
#define EDITOR_MoveToCommand_H

#include "editor/command/Command.h"

#include "violet/math/Vec2.h"

#include <memory>

namespace edt
{
	class EDITOR_API MoveToCommand : public Command
	{
	public:

		static const char * getUsage();
		static std::unique_ptr<Command> parse(const std::string & text);

	public:

		MoveToCommand(Violet::EntityId entityId, const Violet::Vec2f & position);
		virtual ~MoveToCommand() override = default;

		virtual void execute() override;
		virtual bool canUndo() const override;
		virtual void undo() override;

	private:

		Violet::EntityId m_entityId;
		Violet::Vec2f m_position;
	};
}

#endif
