#ifndef EDITOR_CPP_SCRIPT_EXPORTS_H
#define EDITOR_CPP_SCRIPT_EXPORTS_H

#include <memory>

#include "editor/command/chain_command.h"
#include "editor/command/clear_command.h"
#include "editor/command/move_to_command.h"
#include "editor/command/file/open_command.h"
#include "editor/command/file/save_all_command.h"
#include "editor/command/selection/deselect_command.h"
#include "editor/command/selection/select_command.h"

namespace edt
{
	EDITOR_API std::unique_ptr<Chain_Command> create_chain_command(std::vector<std::unique_ptr<Command>> && commands);
	EDITOR_API std::unique_ptr<Clear_Command> create_clear_command();
	EDITOR_API std::unique_ptr<Deselect_Command> create_deselect_command(vlt::Handle entity_id);
	EDITOR_API std::unique_ptr<Deselect_Command> create_deselect_command(std::vector<vlt::Handle> && entityIds);
	EDITOR_API std::unique_ptr<Move_To_Command> create_move_to_command(vlt::Handle entity_id, const vlt::v2 & position);
	EDITOR_API std::unique_ptr<Open_Command> create_open_command(std::string fileName);
	EDITOR_API std::unique_ptr<Save_All_Command> create_save_all_command(std::string fileName);
	EDITOR_API std::unique_ptr<Select_Command> create_select_command(vlt::Handle entity_id);
}

#endif
