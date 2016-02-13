#ifndef EDITOR_CPP_SCRIPT_EXPORTS_H
#define EDITOR_CPP_SCRIPT_EXPORTS_H

#include <memory>

#include "editor/command/chain_command.h"
#include "editor/command/clear_all_command.h"
#include "editor/command/move_to_command.h"
#include "editor/command/file/open_command.h"
#include "editor/command/file/save_all_command.h"
#include "editor/command/selection/deselect_command.h"
#include "editor/command/selection/select_command.h"

namespace edt
{
	EDITOR_API std::unique_ptr<chain_command> create_chain_command(std::vector<std::unique_ptr<command>> && commands);
	EDITOR_API std::unique_ptr<clear_all_command> create_clear_all_command();
	EDITOR_API std::unique_ptr<deselect_command> create_deselect_command(vlt::handle entity_id);
	EDITOR_API std::unique_ptr<deselect_command> create_deselect_command(std::vector<vlt::handle> && entityIds);
	EDITOR_API std::unique_ptr<move_to_command> create_move_to_command(vlt::handle entity_id, const vlt::v2 & position);
	EDITOR_API std::unique_ptr<open_command> create_open_command(std::string fileName);
	EDITOR_API std::unique_ptr<save_all_command> create_save_all_command(std::string fileName);
	EDITOR_API std::unique_ptr<select_command> create_select_command(vlt::handle entity_id);
}

#endif
