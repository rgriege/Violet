// ============================================================================

#include "editor/EditorSystem.h"
#include "editor/command/Command.h"
#include "editor/component/EditorComponent.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/log/Log.h"
#include "violet/script/ScriptComponent.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"

#include <deque>

using namespace edt;
using namespace Violet;

// ============================================================================

namespace EditorNamespace
{
	EditorSystem::CommandFactory ms_commandFactory;
}

using namespace EditorNamespace;

// ============================================================================

const char * EditorSystem::getStaticLabel()
{
	return "edtr";
}

// ----------------------------------------------------------------------------

void EditorSystem::install(Violet::SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &EditorSystem::init);
}

// ----------------------------------------------------------------------------

void EditorSystem::init(Violet::Deserializer & deserializer)
{
	auto segment = deserializer.enterSegment(getStaticLabel());
	std::string editScriptFileName = segment->getString("editScript");

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[=](Engine & engine) mutable
		{
			engine.addSystem(std::unique_ptr<EditorSystem>(new EditorSystem(std::move(editScriptFileName))));
		}, Thread::Window);
}

// ----------------------------------------------------------------------------

void EditorSystem::registerCommand(const char * const usage, const CommandFactory::Producer & producer)
{
	ms_commandFactory.assign(StringUtilities::left(usage, ' '), producer);
}

// ============================================================================

/*void EditorSystem::update(const float dt)
{
	const auto & engine = Engine::getInstance();
	const auto & scene = engine.getCurrentScene();
	std::vector<EntityId> edittedEntityIds;

	{
		const auto & edittedEntities = scene.getEntityView<EditorComponent>();
		entityIds.reserve(edittedEntities.size());
		for (const auto & entity : edittedEntities)
		{
			const EntityId entityId = std::get<0>(entity).getEdittedId();
			edittedEntityIds.emplace_back(entityId);
			if (!m_scene->hasEntity(entityId))
			{
				engine.addWriteTask(scene,
					[=](ComponentManager & scene)
					{
						scene.removeAll(entityId);
					});
			}
		}
	}
		
	for (const auto & entity : scene.getEntityView<LocalTransformComponent>())
		if (!m_scene->hasEntity(std::get<0>(entity).getEntityId()))
			engine.addWriteTask
}*/

// ----------------------------------------------------------------------------

ComponentManager & EditorSystem::getScene()
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

const ComponentManager & EditorSystem::getScene() const
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

void EditorSystem::execute(const std::string & commandString)
{
	const std::string name = StringUtilities::left(commandString, ' ');
	auto command = ms_commandFactory.create(name, StringUtilities::rightOfFirst(commandString, ' '));
	if (command != nullptr)
		execute(std::move(command));
	else
		Log::log(FormattedString<128>().sprintf("Invalid command '%s'", commandString.c_str()));
}

// ----------------------------------------------------------------------------

void EditorSystem::execute(std::unique_ptr<Command> && command)
{
	command->execute();
	if (command->canUndo())
	{
		m_commandHistory.emplace_back(std::move(command));
		if (m_commandHistory.size() >= 100)
			m_commandHistory.pop_front();
	}
}

// ----------------------------------------------------------------------------

void EditorSystem::undo()
{
	if (!m_commandHistory.empty())
	{
		m_commandHistory.back()->undo();
		m_commandHistory.pop_back();
	}
}

// ----------------------------------------------------------------------------

bool EditorSystem::select(const EntityId entityId)
{
	const bool selected = m_selectedEntities.emplace(entityId).second;
	if (selected)
	{
		Engine::getInstance().addWriteTask(Engine::getInstance(),
			[=](Engine & engine)
			{
				auto id = entityId;
				EntitySelectedEvent::emit(engine.getEventContext(), std::move(id));
			});
	}
	return selected;
}

// ----------------------------------------------------------------------------

bool EditorSystem::selected(const EntityId entityId) const
{
	return m_selectedEntities.find(entityId) != m_selectedEntities.end();
}

// ----------------------------------------------------------------------------

const std::set<EntityId> & EditorSystem::getSelectedEntities() const
{
	return m_selectedEntities;
}

// ----------------------------------------------------------------------------

bool EditorSystem::deselect(const EntityId entityId)
{
	const bool deselected = m_selectedEntities.erase(entityId) != 0;
	if (deselected)
	{
		Engine::getInstance().addWriteTask(Engine::getInstance(),
			[=](Engine & engine)
			{
				auto id = entityId;
				EntityDeselectedEvent::emit(engine.getEventContext(), std::move(id));
			});
	}
	return deselected;
}

// ----------------------------------------------------------------------------

void EditorSystem::propogateAdd(const EntityId entityId) const
{
	if (m_scene->hasComponent<WorldTransformComponent>(entityId) && m_scene->hasComponent<ColorComponent>(entityId))
	{
		const auto & engine = Engine::getInstance();
		engine.addWriteTask(engine.getCurrentScene(),
			[=](ComponentManager & scene)
			{
				const auto & editor = *Engine::getInstance().getSystem<EditorSystem>();
				const EntityId copyId = scene.createEntity();

				scene.createComponent<EditorComponent>(copyId, entityId);

				scene.createComponent<WorldTransformComponent>(copyId, editor.getScene().getComponent<WorldTransformComponent>(entityId)->m_transform);

				const auto * cc = editor.getScene().getComponent<ColorComponent>(entityId);
				scene.createComponent<ColorComponent>(copyId, cc->m_mesh->getPolygon(), cc->m_shader, cc->m_color);

				const auto * ltc = editor.getScene().getComponent<LocalTransformComponent>(entityId);
				if (ltc != nullptr)
				{
					EntityId parentId;
					if (ltc->m_parentId != EntityId::ms_invalid)
					{
						for (const auto & entity : scene.getEntityView<EditorComponent>())
						{
							if (entity.get<EditorComponent>().m_editId == ltc->m_parentId)
							{
								parentId = entity.getId();
								break;
							}
						}
					}
					scene.createComponent<LocalTransformComponent>(copyId, parentId, ltc->m_transform);
				}
				editor.addEditBehavior(scene, copyId);
			});
	}
}

// ----------------------------------------------------------------------------

void EditorSystem::propogateRemove(const EntityId entityId) const
{
	const auto & scene = Engine::getInstance().getCurrentScene();
	for (const auto & entity : scene.getEntityView<EditorComponent>())
		if (entity.get<EditorComponent>().m_editId == entityId)
			scene.removeAll(entity.getId());
}

// ============================================================================

EditorSystem::EditorSystem(std::string editScriptFileName) :
	System(getStaticLabel()),
	m_scene(std::make_unique<ComponentManager>()),
	m_editScriptFileName(std::move(editScriptFileName)),
	m_commandHistory(),
	m_selectedEntities()
{
}

// ----------------------------------------------------------------------------

void EditorSystem::addEditBehavior(const ComponentManager & scene, const EntityId entityId) const
{
	Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
		[&scene, entityId, this]()
		{
			Engine::getInstance().addWriteTask(*scene.getPool<ScriptComponent>(),
				[=](ComponentPool & pool)
				{
					pool.create<ScriptComponent>(entityId, m_editScriptFileName.c_str());
				});
			Polygon poly = scene.getComponent<ColorComponent>(entityId)->m_mesh->getPolygon();
			Engine::getInstance().addWriteTask(*scene.getPool<MouseInputComponent>(),
				[=](ComponentPool & pool) mutable
				{
					pool.create<MouseInputComponent>(entityId, std::move(poly));
				});
		}), Thread::Window);
}

// ============================================================================
