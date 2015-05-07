// ============================================================================

#include "engine/update/system/UpdateSystem.h"

#include "engine/Engine.h"
#include "engine/scene/Scene.h"
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

void UpdateSystem::update(float /*dt*/, Engine & engine)
{
	for (auto & components : engine.getCurrentScene().getView<UpdateComponent, CppScriptComponent>())
	{
		CppScriptComponent & sc = get<CppScriptComponent&>(components);
		ScriptUtilities::run<void>(sc, "update", sc.getEntity(), engine);
	}
}

// ============================================================================

UpdateSystem::UpdateSystem() :
	System(getStaticLabel())
{
}

// ============================================================================
