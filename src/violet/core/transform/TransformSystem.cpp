#include "violet/core/transform/TransformSystem.h"

#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"

using namespace Violet;

void TransformSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &TransformSystem::init);
}

std::unique_ptr<System> TransformSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new TransformSystem();
	ComponentFactory::getInstance().assign(TransformComponent::getLabel(), std::bind(&TransformSystem::create, system, std::placeholders::_1, std::placeholders::_2));
	return std::unique_ptr<System>(system);
}

void TransformSystem::update(float /*dt*/, AlterContext & /*context*/)
{
}