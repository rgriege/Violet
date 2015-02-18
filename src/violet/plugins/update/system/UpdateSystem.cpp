#include "violet/plugins/update/system/UpdateSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/component/ComponentFactory.h"
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
	ComponentFactory::getInstance().assign(UpdateComponent::getLabel(), std::bind(&UpdateSystem::create, system, std::placeholders::_1, std::placeholders::_2));
	return std::unique_ptr<System>(system);
}

void UpdateSystem::update(float /*dt*/, Engine & engine)
{
	for (auto const & component : *m_components)
	{
		auto const & scriptComponent = engine.fetch<ScriptSystem>(component.m_entity);
		scriptComponent.m_script->run(Procedure::create("update", component.m_entity, engine));
	}
}