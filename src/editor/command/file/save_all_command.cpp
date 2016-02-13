// ============================================================================

#include "editor/command/file/save_all_command.h"

#include "editor/editor_system.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"

using namespace edt;
using namespace vlt;

// ============================================================================

const char * save_all_command::get_usage()
{
	return "save <file>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<command> save_all_command::parse(const std::string & text)
{
	return text.find(' ') == std::string::npos ? std::make_unique<save_all_command>(text) : nullptr;
}

// ============================================================================

save_all_command::save_all_command(std::string fileName) :
	m_fileName(std::move(fileName))
{
}

// ----------------------------------------------------------------------------

void save_all_command::execute()
{
	engine::instance().get_system<editor_system>()->get_scene().save(m_fileName.c_str());
}

// ----------------------------------------------------------------------------

bool save_all_command::can_undo() const
{
	return false;
}

// ----------------------------------------------------------------------------

void save_all_command::undo()
{
}

// ============================================================================
