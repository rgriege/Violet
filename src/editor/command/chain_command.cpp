// ============================================================================

#include "editor/command/chain_command.h"

#include <algorithm>

using namespace edt;

// ============================================================================

Chain_Command::Chain_Command(std::vector<std::unique_ptr<Command>> && commands) :
	m_commands(std::move(commands))
{
}

// ----------------------------------------------------------------------------

void Chain_Command::execute()
{
	for (auto & Command : m_commands)
		Command->execute();
}

// ----------------------------------------------------------------------------

bool Chain_Command::can_undo() const
{
	return std::any_of(m_commands.begin(), m_commands.end(), [](const std::unique_ptr<Command> & c) { return c->can_undo(); });
}

// ----------------------------------------------------------------------------

void Chain_Command::undo()
{
	for (auto & Command : m_commands)
		if (Command->can_undo())
			Command->undo();
}

// ============================================================================

