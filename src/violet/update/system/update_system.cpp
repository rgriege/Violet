// ============================================================================

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/script/script_component.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/update/component/update_component.h"
#include "violet/update/system/update_system.h"

using namespace vlt;

// ============================================================================

const char * Update_System::get_label_static()
{
	return "updt";
}

// ----------------------------------------------------------------------------

void Update_System::install(System_Factory & factory)
{
	factory.assign(get_label_static(), &Update_System::init);
}

// ----------------------------------------------------------------------------

static void init_task(void *)
{
	Engine::instance().add_system(std::unique_ptr<vlt::System>(new Update_System));
}

void Update_System::init(Deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());

	add_task(init_task, nullptr, Script_Component::metadata->thread, task_type::write);
}

// ============================================================================

Update_System::Update_System() :
	System(get_label_static())
{
}

// ---------------------------------------------------------------------------

Update_System::Update_System(Update_System && other) :
	System(std::move(other))
{
}

// ---------------------------------------------------------------------------

void Update_System::update(r32 dt)
{
	for (const auto & entity : Engine::instance().get_current_scene().get_entity_view<Update_Component, Script_Component>())
	{
		Handle id = entity.id;
		UpdateMethod::run(*entity.get<Script_Component>().script, std::move(id), std::move(dt));
	}
}

// ============================================================================
