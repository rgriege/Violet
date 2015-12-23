// ============================================================================

#include "editor/command/file/OpenCommand.h"

#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"

using namespace edt;
using namespace Violet;

// ============================================================================

const char * OpenCommand::getUsage()
{
	return "open <file>";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> OpenCommand::parse(const std::string & text)
{
	return text.find(' ') == std::string::npos ? std::make_unique<OpenCommand>(text) : nullptr;
}

// ============================================================================

OpenCommand::OpenCommand(std::string fileName) :
	m_fileName(std::move(fileName)),
	m_entityIds()
{
}

// ----------------------------------------------------------------------------

void OpenCommand::execute()
{
	Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
		[=](ComponentManager & scene)
		{
			m_entityIds = scene.load(m_fileName.c_str(), EditorSystem::ms_tagMap);
			for (const auto entityId : m_entityIds)
				EditorSystem::addEditBehavior(scene, entityId);
		});
}

// ----------------------------------------------------------------------------

bool OpenCommand::canUndo() const
{
	return true;
}

// ----------------------------------------------------------------------------

void OpenCommand::undo()
{
	const auto & scene = Engine::getInstance().getCurrentScene();
	for (const EntityId entityId : m_entityIds)
		if (scene.exists(entityId))
			scene.removeAll(entityId);
	m_entityIds.clear();
}

// ============================================================================
