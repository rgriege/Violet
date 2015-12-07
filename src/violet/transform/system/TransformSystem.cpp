// ============================================================================

#include "violet/transform/system/TransformSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/LocalTransformComponent.h"
#include "violet/transform/component/WorldTransformComponent.h"

using namespace Violet;

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
}

// ---------------------------------------------------------------------------

void TransformSystem::update(const float /*dt*/)
{
	auto localView = Engine::getInstance().getCurrentScene().getEntityView<LocalTransformComponent>();
	auto worldView = Engine::getInstance().getCurrentScene().getEntityView<WorldTransformComponent>();
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
			// Engine::addWriteTask(Engine::getInstance().getCurrentScene());
		}
	}
}

// ============================================================================

TransformSystem::TransformSystem() :
	System(getStaticLabel())
{
}

// ============================================================================