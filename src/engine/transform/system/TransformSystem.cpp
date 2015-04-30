#include "engine/transform/system/TransformSystem.h"

#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"
#include "engine/system/SystemFactory.h"

using namespace Violet;

void TransformSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &TransformSystem::init);
}

std::unique_ptr<System> TransformSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new TransformSystem();
	return std::unique_ptr<System>(system);
}

void TransformSystem::update(const float dt, Engine & engine)
{
	ComponentSystem<TransformComponent>::update(dt, engine);
}