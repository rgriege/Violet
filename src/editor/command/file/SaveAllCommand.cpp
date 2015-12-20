// ============================================================================

#include "editor/command/file/SaveAllCommand.h"

#include "editor/Editor.h"
#include "editor/component/EditorComponent.h"
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
	const auto & scene = Engine::getInstance().getCurrentScene();

	std::vector<EntityId> entityIds;
	for (const auto & entity : scene.getEntityView<EditorComponent>())
		entityIds.emplace_back(std::get<0>(entity).getEntityId());

	Engine::getInstance().getCurrentScene().save(m_fileName.c_str(), entityIds, Editor::ms_tagMap);
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
