// ============================================================================

#include "engine/update/system/UpdateSystem.h"

#include "engine/Engine.h"
#include "engine/scene/SceneUtilities.h"
#include "engine/script/ScriptUtilities.h"
#include "engine/serialization/Deserializer.h"
#include "engine/system/SystemFactory.h"
#include "engine/update/component/UpdateComponent.h"

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
	SceneUtilities::Processor processor;
	processor.addDelegate(SceneUtilities::Processor::Filter::create<UpdateComponent, CppScriptComponent>(), [&](const Entity & entity, float) {
		const CppScriptComponent & scriptComponent = *entity.getComponent<CppScriptComponent>();
		ScriptUtilities::run<void>(scriptComponent, "update", entity, engine);
	});
	processor.process(engine.getCurrentScene(), dt);
}

// ============================================================================

UpdateSystem::UpdateSystem() :
	System(getStaticLabel())
{
}

// ============================================================================
