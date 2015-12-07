// ============================================================================

#include "violet/update/system/UpdateSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
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

void UpdateSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new UpdateSystem));
		});
}

// ============================================================================

UpdateSystem::UpdateSystem(UpdateSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

UpdateSystem::~UpdateSystem()
{
}

// ---------------------------------------------------------------------------

void UpdateSystem::update(float dt)
{
	for (auto entity : Engine::getInstance().getCurrentScene().getEntityView<UpdateComponent, ScriptComponent>())
		UpdateMethod::run(*std::get<1>(entity).m_script, std::get<0>(entity).getEntityId(), std::move(dt));
}

// ============================================================================

UpdateSystem::UpdateSystem() :
	System(getStaticLabel())
{
}

// ============================================================================