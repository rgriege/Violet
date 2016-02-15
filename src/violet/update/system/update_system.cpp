// ============================================================================

#include "violet/update/system/update_system.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/script/script_component.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/update/component/update_component.h"

using namespace vlt;

// ============================================================================

const char * update_system::get_label_static()
{
	return "updt";
}

// ----------------------------------------------------------------------------

void update_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &update_system::init);
}

// ----------------------------------------------------------------------------

static void init_task(void *)
{
	engine::instance().add_system(std::unique_ptr<vlt::system>(new update_system));
}

void update_system::init(deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());

	add_task(init_task, nullptr, script_component::metadata->thread, task_type::write);
}

// ============================================================================

update_system::update_system() :
	system(get_label_static())
{
}

// ---------------------------------------------------------------------------

update_system::update_system(update_system && other) :
	system(std::move(other))
{
}

// ---------------------------------------------------------------------------

void update_system::update(r32 dt)
{
	for (const auto & entity : engine::instance().get_current_scene().get_entity_view<update_component, script_component>())
	{
		handle id = entity.id;
		UpdateMethod::run(*entity.get<script_component>().script, std::move(id), std::move(dt));
	}
}

// ============================================================================
