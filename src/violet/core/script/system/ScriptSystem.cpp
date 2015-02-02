#include "violet/core/script/system/ScriptSystem.h"

#include "violet/core/AlterContext.h"
#include "violet/core/component/ComponentFactory.h"

using namespace Violet;

void ScriptSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &ScriptSystem::init);
}

std::unique_ptr<System> ScriptSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new ScriptSystem;
	ComponentFactory::getInstance().assign(ScriptComponent::getLabel(), std::bind(&ScriptSystem::create, system, std::placeholders::_1, std::placeholders::_2));
	return std::unique_ptr<System>(system);
}

void ScriptSystem::update(float /*dt*/, AlterContext & /*context*/)
{
}