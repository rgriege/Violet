// ============================================================================

#include "editor/command/ChainCommand.h"

#include <algorithm>

using namespace edt;

// ============================================================================

ChainCommand::ChainCommand(std::vector<std::unique_ptr<Command>> && commands) :
	m_commands(std::move(commands))
{
}

// ----------------------------------------------------------------------------

void ChainCommand::execute()
{
	for (auto & command : m_commands)
		command->execute();
}

// ----------------------------------------------------------------------------

bool ChainCommand::canUndo() const
{
	return std::any_of(m_commands.begin(), m_commands.end(), [](const std::unique_ptr<Command> & c) { return c->canUndo(); });
}

// ----------------------------------------------------------------------------

void ChainCommand::undo()
{
	for (auto & command : m_commands)
		if (command->canUndo())
			command->undo();
}

// ============================================================================

