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

void update_system::init(deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());

	engine::instance().add_write_task(engine::instance(),
		[](engine & engine)
		{
			engine.add_system(std::unique_ptr<system>(new update_system));
		});
}

// ============================================================================

update_system::update_system(update_system && other) :
	system(std::move(other))
{
}

// ----------------------------------------------------------------------------

update_system::~update_system()
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

update_system::update_system() :
	system(get_label_static())
{
}

// ============================================================================
