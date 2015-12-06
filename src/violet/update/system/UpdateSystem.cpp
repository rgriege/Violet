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
#ifdef USE_SCENE_PROCESSOR
			engine.addSceneDelegate(SceneProcessor::Filter::create<UpdateComponent, ScriptComponent>(), updateEntity);
#endif
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
#ifdef USE_SCENE_PROCESSOR
	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.removeSceneDelegate(SceneProcessor::Filter::create<UpdateComponent, ScriptComponent>(), updateEntity);
		});
#endif
}

// ---------------------------------------------------------------------------

void UpdateSystem::update(float dt)
{
#ifndef USE_SCENE_PROCESSOR
	for (auto entity : Engine::getInstance().getCurrentScene().getComponentManager().getEntityView<UpdateComponent, ScriptComponent>())
		UpdateMethod::run(*std::get<1>(entity).m_script, std::get<0>(entity).getEntityId(), std::move(dt));
#endif
}

// ============================================================================

UpdateSystem::UpdateSystem() :
	System(getStaticLabel())
{
}

// ============================================================================