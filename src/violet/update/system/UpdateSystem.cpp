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

std::unique_ptr<System> UpdateSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	auto system = new UpdateSystem;
	return std::unique_ptr<System>(system);
}

// ============================================================================

UpdateSystem::UpdateSystem(UpdateSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

void UpdateSystem::update(const float dt, const Engine & engine)
{
	SceneProcessor processor;
	processor.addDelegate(SceneProcessor::Filter::create<UpdateComponent, ScriptComponent>(), [&engine](const Entity & entity, float dt) {
		UpdateMethod::run(*entity.getComponent<ScriptComponent>()->m_script, entity, engine, std::move(dt));
	});
	processor.process(engine.getCurrentScene(), dt);
}

// ============================================================================

UpdateSystem::UpdateSystem() :
	System(getStaticLabel())
{
}

// ============================================================================
