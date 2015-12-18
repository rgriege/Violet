// ============================================================================

#include "editor/Editor.h"

#include "editor/component/EditorComponent.h"
#include "editor/component/EditorComponentWrapper.h"
#include "editor/command/Command.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/script/ScriptComponent.h"
#include "violet/update/component/UpdateComponent.h"

#include <deque>

using namespace edt;
using namespace Violet;

// ============================================================================

namespace EditorNamespace
{
	std::deque<std::unique_ptr<Command>> ms_commands;
}

using namespace EditorNamespace;

// ============================================================================

const ComponentManager::TagMap Editor::ms_tagMap = {
	{ UpdateComponent::getStaticTag(), EditorComponentWrapper<UpdateComponent>::getStaticTag() },
	{ ScriptComponent::getStaticTag(), EditorComponentWrapper<ScriptComponent>::getStaticTag() },
	{ KeyInputComponent::getStaticTag(), EditorComponentWrapper<KeyInputComponent>::getStaticTag() },
	{ MouseInputComponent::getStaticTag(), EditorComponentWrapper<MouseInputComponent>::getStaticTag() },
	{ EditorComponent::getStaticTag(), EditorComponentWrapper<EditorComponent>::getStaticTag() }
};

// ============================================================================

void Editor::execute(std::unique_ptr<Command> && command)
{
	command->execute();
	if (command->canUndo())
	{
		ms_commands.emplace_back(std::move(command));
		if (ms_commands.size() >= 100)
			ms_commands.pop_front();
	}
}

// ----------------------------------------------------------------------------

void Editor::undo()
{
	if (!ms_commands.empty())
	{
		ms_commands.back()->undo();
		ms_commands.pop_back();
	}
}

// ----------------------------------------------------------------------------

void Editor::addEditBehavior(const ComponentManager & scene, const EntityId entityId)
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

void Editor::removeEditBehavior(const ComponentManager & scene, const EntityId entityId)
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
