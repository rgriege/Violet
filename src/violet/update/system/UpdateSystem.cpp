// ============================================================================

#include "violet/update/system/UpdateSystem.h"

#include "violet/Engine.h"
#include "violet/scene/SceneProcessor.h"
#include "violet/script/ScriptComponent.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/update/component/UpdateComponent.h"

using namespace Violet;

// ============================================================================

namespace UpdateSystemNamespace
{
	void updateEntity(const Entity & entity, float dt);
}

using namespace UpdateSystemNamespace;

// ============================================================================

const char * UpdateSystem::getStaticLabel()
{
	return "updt";
}

// ----------------------------------------------------------------------------

void UpdateSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &UpdateSystem::init);
}

// ----------------------------------------------------------------------------

void UpdateSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new UpdateSystem));
			engine.addSceneDelegate(SceneProcessor::Filter::create<UpdateComponent, ScriptComponent>(), updateEntity);
		});
}

// ============================================================================

UpdateSystem::UpdateSystem(UpdateSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

UpdateSystem::~UpdateSystem()
{
	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.removeSceneDelegate(SceneProcessor::Filter::create<UpdateComponent, ScriptComponent>(), updateEntity);
		});
}

// ============================================================================

UpdateSystem::UpdateSystem() :
	System(getStaticLabel())
{
}

// ============================================================================

void UpdateSystemNamespace::updateEntity(const Entity & entity, float dt)
{
	UpdateMethod::run(*entity.getComponent<ScriptComponent>()->m_script, entity, std::move(dt));
}

// ============================================================================