// ============================================================================

#include "editor/command/file/save_all_command.h"
#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * Save_All_Command::get_usage()
{
	return "save <file>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> Save_All_Command::parse(const std::string & text)
{
	return text.find(' ') == std::string::npos ? std::make_unique<Save_All_Command>(text) : nullptr;
}

// ============================================================================

Save_All_Command::Save_All_Command(std::string fileName) :
	m_fileName(std::move(fileName))
{
}

// ----------------------------------------------------------------------------

void Save_All_Command::execute()
{
	Engine::instance().get_system<Editor_System>()->get_scene().save(m_fileName.c_str());
}

// ----------------------------------------------------------------------------

bool Save_All_Command::can_undo() const
{
	return false;
}

// ----------------------------------------------------------------------------

void Save_All_Command::undo()
{
}

// ============================================================================
