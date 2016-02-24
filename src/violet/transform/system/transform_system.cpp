// ============================================================================

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/log.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/transform/system/transform_system.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

namespace Transform_System_Namespace
{
	typedef Entity<Local_Transform_Component, World_Transform_Component> Transform_Entity;

	void update_world_transform(const Transform_Entity & entity, std::map<Handle, m4> & world_transform_cache);
}

using namespace Transform_System_Namespace;

// ============================================================================

const char * Transform_System::get_label_static()
{
	return "tsfm";
}

// ----------------------------------------------------------------------------

void Transform_System::install(System_Factory & factory)
{
	factory.assign(get_label_static(), &Transform_System::init);
}

// ----------------------------------------------------------------------------

static void init_task(void*)
{
	Engine::instance().add_system(std::unique_ptr<vlt::System>(new Transform_System));
}

void Transform_System::init(Deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());
	add_task(init_task, nullptr, Local_Transform_Component::metadata->thread, task_type::write);
}

// ============================================================================

Transform_System::Transform_System() :
	System(get_label_static()),
	m_entityWorldtransformCache()
{
}

// ----------------------------------------------------------------------------

Transform_System::Transform_System(Transform_System && other) :
	System(std::move(other)),
	m_entityWorldtransformCache()
{
}

// ---------------------------------------------------------------------------

void Transform_System::update(const r32 /*dt*/)
{
	m_entityWorldtransformCache = std::map<Handle, m4>{ { Handle::Invalid, m4::Identity } };
	const auto & engine = Engine::instance();
	for (const auto & entity : engine.get_current_scene().get_entity_view<Local_Transform_Component, World_Transform_Component>())
		update_world_transform(entity, m_entityWorldtransformCache);
}

// ============================================================================

struct update_world_transform_task_data
{
	Handle entity_id;
	m4 * transform;
};

static void update_world_transform_task(void * mem)
{
	auto data = make_unique<update_world_transform_task_data>(mem);
	auto pool = Engine::instance().get_current_scene().get_pool<World_Transform_Component>();
	pool->get<World_Transform_Component>(data->entity_id)->transform = *data->transform;
}

void Transform_System_Namespace::update_world_transform(const Transform_Entity & entity, std::map<Handle, m4> & worldtransformCache)
{
	const auto & localtransformComponent = entity.get<Local_Transform_Component>();
	const auto & worldtransformComponent = entity.get<World_Transform_Component>();
	const Handle entity_id = entity.id;
	const Handle & parent_id = localtransformComponent.parent_id;

	const auto it = worldtransformCache.find(parent_id);
	const m4 & parentWorldtransform = it != worldtransformCache.end() ? it->second : Engine::instance().get_current_scene().get_component<World_Transform_Component>(parent_id)->transform;
	
	{
		const m4 & worldtransform = worldtransformCache[entity_id] = parentWorldtransform * localtransformComponent.transform;
		if (worldtransformComponent.transform != worldtransform)
			add_task(update_world_transform_task, new update_world_transform_task_data{ entity_id, &worldtransformCache[entity_id] }, World_Transform_Component::metadata->thread, task_type::write);
	}
}

// ============================================================================
