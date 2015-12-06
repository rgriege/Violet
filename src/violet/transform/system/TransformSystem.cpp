// ============================================================================

#include "violet/transform/system/TransformSystem.h"

#include "violet/Engine.h"
#include "violet/scene/SceneProcessor.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"

using namespace Violet;

// ============================================================================

namespace TransformSystemNamespace
{
	void updateEntity(const Entity & entity, float dt);
}

using namespace TransformSystemNamespace;

// ============================================================================

const char * TransformSystem::getStaticLabel()
{
	return "tsfm";
}

// ----------------------------------------------------------------------------

void TransformSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &TransformSystem::init);
}

// ----------------------------------------------------------------------------

void TransformSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new TransformSystem));
#ifdef USE_SCENE_PROCESSOR
			engine.addSceneDelegate(SceneProcessor::Filter::create<LocalTransformComponent, WorldTransformComponent>(), updateEntity);
#endif
		});
}

// ============================================================================

TransformSystem::TransformSystem(TransformSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

TransformSystem::~TransformSystem()
{
#ifdef USE_SCENE_PROCESSOR
	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.removeSceneDelegate(SceneProcessor::Filter::create<LocalTransformComponent, WorldTransformComponent>(), updateEntity);
		});
#endif
}

// ---------------------------------------------------------------------------

void TransformSystem::update(const float /*dt*/)
{
#ifndef USE_SCENE_PROCESSOR
	/*auto localView = Engine::getInstance().getCurrentScene().getComponentManager().getEntityView<LocalTransformComponent>();
	auto worldView = Engine::getInstance().getCurrentScene().getComponentManager().getEntityView<WorldTransformComponent>();
	auto localIt = localView.begin(), localEnd = localView.end();
	auto worldIt = worldView.begin(), worldEnd = worldView.end();
	while (localIt != localEnd)
	{
		const auto & localTransformComponent = std::get<0>(*localIt);
		if (worldIt != worldEnd)
		{
			const auto & worldTransformComponent = std::get<0>(*worldIt);
		}
		else
		{
			Engine::addWriteTask(Engine::getInstance().getCurrentScene().getComponentManager());
		}
	}*/
#endif
}

// ============================================================================

TransformSystem::TransformSystem() :
	System(getStaticLabel())
{
}

// ============================================================================