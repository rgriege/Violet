#include "violet/core/script/system/ScriptSystem.h"

using namespace Violet;

void ScriptSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &ScriptSystem::init);
}

std::unique_ptr<System> ScriptSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new ScriptSystem;
	return std::unique_ptr<System>(system);
}

ScriptSystem::ScriptSystem(ScriptSystem && other) :
	ComponentSystem<ScriptComponent>(std::move(other))
{
}

void ScriptSystem::update(float /*dt*/, Engine & /*engine*/)
{
}