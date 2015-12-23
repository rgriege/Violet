// ============================================================================

#include "editor/EditorSystem.h"

#include "editor/component/EditorComponent.h"
#include "editor/component/EditorComponentWrapper.h"
#include "editor/command/Command.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/log/Log.h"
#include "violet/script/ScriptComponent.h"
#include "violet/system/SystemFactory.h"
#include "violet/update/component/UpdateComponent.h"
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

const ComponentManager::TagMap EditorSystem::ms_tagMap = {
	{ UpdateComponent::getStaticTag(), EditorComponentWrapper<UpdateComponent>::getStaticTag() },
	{ ScriptComponent::getStaticTag(), EditorComponentWrapper<ScriptComponent>::getStaticTag() },
	{ KeyInputComponent::getStaticTag(), EditorComponentWrapper<KeyInputComponent>::getStaticTag() },
	{ MouseInputComponent::getStaticTag(), EditorComponentWrapper<MouseInputComponent>::getStaticTag() },
	{ EditorComponent::getStaticTag(), EditorComponentWrapper<EditorComponent>::getStaticTag() }
};

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
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[=](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<EditorSystem>(new EditorSystem));
		}, Thread::Window);
}

// ----------------------------------------------------------------------------

void EditorSystem::registerCommand(const char * const usage, const CommandFactory::Producer & producer)
{
	ms_commandFactory.assign(StringUtilities::left(usage, ' '), producer);
}

// ----------------------------------------------------------------------------

void EditorSystem::addEditBehavior(const ComponentManager & scene, const EntityId entityId)
{
	Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
		[&scene, entityId]()
		{
			if (scene.hasComponent<ColorComponent>(entityId))
			{
				Engine::getInstance().addWriteTask(*scene.getPool<ScriptComponent>(),
					[=](ComponentPool & pool)
					{
						pool.create<ScriptComponent>(entityId, "editEntity.dll");
					});
				Polygon poly = scene.getComponent<ColorComponent>(entityId)->m_mesh->getPolygon();
				Engine::getInstance().addWriteTask(*scene.getPool<MouseInputComponent>(),
					[=](ComponentPool & pool) mutable
					{
						pool.create<MouseInputComponent>(entityId, std::move(poly));
					});
			}
		}), Thread::Window);
	Engine::getInstance().addWriteTask(*scene.getPool<EditorComponent>(),
		[entityId](ComponentPool & pool)
		{
			pool.create<EditorComponent>(entityId);
		});
}

// ----------------------------------------------------------------------------

void EditorSystem::removeEditBehavior(const ComponentManager & scene, const EntityId entityId)
{
	const auto & engine = Engine::getInstance();
	engine.addWriteTask(*engine.getCurrentScene().getPool<ScriptComponent>(),
		[=](ComponentPool & pool)
		{
			pool.remove(entityId);
		});
	engine.addWriteTask(*engine.getCurrentScene().getPool<MouseInputComponent>(),
		[=](ComponentPool & pool)
		{
			pool.remove(entityId);
		});
}

// ============================================================================

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

// ============================================================================

EditorSystem::EditorSystem() :
	System(getStaticLabel()),
	m_commandHistory()
{
}

// ============================================================================
