// ============================================================================

#include "editor/cpp_script_exports.h"

using namespace edt;
using namespace vlt;

// ============================================================================

std::unique_ptr<Chain_Command> edt::create_chain_command(std::vector<std::unique_ptr<Command>> && commands)
{
	return std::make_unique<Chain_Command>(std::move(commands));
}

// ----------------------------------------------------------------------------

std::unique_ptr<Clear_Command> edt::create_clear_command()
{
	return std::make_unique<Clear_Command>();
}

// ----------------------------------------------------------------------------

std::unique_ptr<Deselect_Command> edt::create_deselect_command(const Handle entity_id)
{
	return std::make_unique<Deselect_Command>(entity_id);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Deselect_Command> edt::create_deselect_command(std::vector<vlt::Handle> && entityIds)
{
	return std::make_unique<Deselect_Command>(std::move(entityIds));
}

// ----------------------------------------------------------------------------

std::unique_ptr<Move_To_Command> edt::create_move_to_command(const Handle entity_id, const v2 & position)
{
	return std::make_unique<Move_To_Command>(entity_id, position);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Open_Command> edt::create_open_command(std::string fileName)
{
	return std::make_unique<Open_Command>(std::move(fileName));
}

// ----------------------------------------------------------------------------

std::unique_ptr<Save_All_Command> edt::create_save_all_command(std::string fileName)
{
	return std::make_unique<Save_All_Command>(std::move(fileName));
}

// ----------------------------------------------------------------------------

std::unique_ptr<Select_Command> edt::create_select_command(const Handle entity_id)
{
	return std::make_unique<Select_Command>(entity_id);
}

// ============================================================================
