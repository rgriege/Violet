// ============================================================================

#include "editor/command/chain_command.h"

#include <algorithm>

using namespace edt;

// ============================================================================

chain_command::chain_command(std::vector<std::unique_ptr<command>> && commands) :
	m_commands(std::move(commands))
{
}

// ----------------------------------------------------------------------------

void chain_command::execute()
{
	for (auto & command : m_commands)
		command->execute();
}

// ----------------------------------------------------------------------------

bool chain_command::can_undo() const
{
	return std::any_of(m_commands.begin(), m_commands.end(), [](const std::unique_ptr<command> & c) { return c->can_undo(); });
}

// ----------------------------------------------------------------------------

void chain_command::undo()
{
	for (auto & command : m_commands)
		if (command->can_undo())
			command->undo();
}

// ============================================================================

