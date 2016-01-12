// ============================================================================

#include "editor/command/selection/SelectCommand.h"

#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/Random.h"
#include "violet/utility/StringUtilities.h"

using namespace edt;
using namespace Violet;

// ============================================================================

const char * SelectCommand::getUsage()
{
	return "select [comma-separated-id-list|all]";
}

// ----------------------------------------------------------------------------

std::unique_ptr<Command> SelectCommand::parse(const std::string & text)
{
	if (text == "all")
		return std::make_unique<SelectCommand>(Engine::getInstance().getCurrentScene().getEntityIds());
	else
	{
		std::vector<std::string> idStrings;
		StringUtilities::split(text, ',', idStrings);

		std::vector<EntityId> entityIds;

		for (const auto & idString : idStrings)
			entityIds.emplace_back(std::atoi(idString.c_str()), ~0);
		return std::make_unique<SelectCommand>(std::move(entityIds));
	}
	return nullptr;
}

// ============================================================================

SelectCommand::SelectCommand(const EntityId entityId) :
	m_entityIds()
{
	m_entityIds.emplace_back(entityId);
}

// ----------------------------------------------------------------------------

SelectCommand::SelectCommand(const std::vector<EntityId> & entityIds) :
	m_entityIds(entityIds)
{
}

// ----------------------------------------------------------------------------

SelectCommand::SelectCommand(std::vector<EntityId> && entityIds) :
	m_entityIds(std::move(entityIds))
{
}

// ----------------------------------------------------------------------------

void SelectCommand::execute()
{
	const auto & engine = Engine::getInstance();
	const auto & editor = *engine.getSystem<EditorSystem>();
	const auto & scene = editor.getScene();
	for (auto it = m_entityIds.begin(), end = m_entityIds.end(); it != end; )
	{
		auto & entityId = *it;
		if (entityId.getVersion() == EntityId::MaxVersion)
			entityId = EntityId(entityId.getId(), scene.getEntityVersion(entityId.getId()));

		/*if (editor.selected(entityId))
			it = m_entityIds.erase(it);
		else*/
			++it;
	}		

	engine.addWriteTask(editor,
		[=](EditorSystem & editor)
		{
			m_entityIds.erase(std::remove_if(m_entityIds.begin(), m_entityIds.end(),
				[&](const EntityId entityId)
				{
					return !editor.select(entityId);
				}), m_entityIds.end());
		});
}

// ----------------------------------------------------------------------------

bool SelectCommand::canUndo() const
{
	return !m_entityIds.empty();
}

// ----------------------------------------------------------------------------

void SelectCommand::undo()
{
	const auto & entityIds = m_entityIds;
	Engine::getInstance().addWriteTask(*Engine::getInstance().getSystem<EditorSystem>(),
		[=](EditorSystem & editor)
		{
			for (const auto entityId : entityIds)
				editor.deselect(entityId);
		});
}

// ============================================================================
