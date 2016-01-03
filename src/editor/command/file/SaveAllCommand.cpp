// ============================================================================

#include "editor/command/file/SaveAllCommand.h"

#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"

using namespace edt;
using namespace Violet;

// ============================================================================

const char * SaveAllCommand::getUsage()
{
	return "save <file>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> SaveAllCommand::parse(const std::string & text)
{
	return text.find(' ') == std::string::npos ? std::make_unique<SaveAllCommand>(text) : nullptr;
}

// ============================================================================

SaveAllCommand::SaveAllCommand(std::string fileName) :
	m_fileName(std::move(fileName))
{
}

// ----------------------------------------------------------------------------

void SaveAllCommand::execute()
{
	Engine::getInstance().getSystem<EditorSystem>()->getScene().save(m_fileName.c_str());
}

// ----------------------------------------------------------------------------

bool SaveAllCommand::canUndo() const
{
	return false;
}

// ----------------------------------------------------------------------------

void SaveAllCommand::undo()
{
}

// ============================================================================
