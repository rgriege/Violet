#include "engine/update/system/UpdateSystem.h"

#include "engine/Engine.h"
#include "engine/script/ScriptUtilities.h"

using namespace Violet;

void UpdateSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &UpdateSystem::init);
}

std::unique_ptr<System> UpdateSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new UpdateSystem;
	return std::unique_ptr<System>(system);
}

UpdateSystem::UpdateSystem(UpdateSystem && other) :
	ComponentSystem<UpdateComponent>(std::move(other))
{
}

void UpdateSystem::update(float /*dt*/, Engine & engine)
{
	for (auto const & component : getComponents())
		ScriptUtilities::run<void>(engine, component.getEntity(), "update", component.getEntity(), engine);
}