#include "violet/plugins/update/system/UpdateSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/script/Procedure.h"
#include "violet/core/script/system/ScriptSystem.h"

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
	{
		auto const & scriptComponent = engine.fetch<ScriptComponent>(component.getEntity());
		scriptComponent.m_script->run(Procedure::create("update", component.getEntity(), engine));
	}
}