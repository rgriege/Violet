// ============================================================================

#include "editor/cpp_script_exports.h"

using namespace edt;
using namespace vlt;

// ============================================================================

std::unique_ptr<chain_command> edt::create_chain_command(std::vector<std::unique_ptr<command>> && commands)
{
	return std::make_unique<chain_command>(std::move(commands));
}

// ----------------------------------------------------------------------------

std::unique_ptr<clear_command> edt::create_clear_command()
{
	return std::make_unique<clear_command>();
}

// ----------------------------------------------------------------------------

std::unique_ptr<deselect_command> edt::create_deselect_command(const handle entity_id)
{
	return std::make_unique<deselect_command>(entity_id);
}

// ----------------------------------------------------------------------------

std::unique_ptr<deselect_command> edt::create_deselect_command(std::vector<vlt::handle> && entityIds)
{
	return std::make_unique<deselect_command>(std::move(entityIds));
}

// ----------------------------------------------------------------------------

std::unique_ptr<move_to_command> edt::create_move_to_command(const handle entity_id, const v2 & position)
{
	return std::make_unique<move_to_command>(entity_id, position);
}

// ----------------------------------------------------------------------------

std::unique_ptr<open_command> edt::create_open_command(std::string fileName)
{
	return std::make_unique<open_command>(std::move(fileName));
}

// ----------------------------------------------------------------------------

std::unique_ptr<save_all_command> edt::create_save_all_command(std::string fileName)
{
	return std::make_unique<save_all_command>(std::move(fileName));
}

// ----------------------------------------------------------------------------

std::unique_ptr<select_command> edt::create_select_command(const handle entity_id)
{
	return std::make_unique<select_command>(entity_id);
}

// ============================================================================
