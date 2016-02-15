// ============================================================================

#include "violet/transform/system/transform_system.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/Log.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

namespace transformSystemNamespace
{
	typedef entity<local_transform_component, world_transform_component> transformEntity;

	void updateWorldtransform(const transformEntity & entity, std::map<handle, m4> & worldtransformCache);
}

using namespace transformSystemNamespace;

// ============================================================================

const char * transform_system::get_label_static()
{
	return "tsfm";
}

// ----------------------------------------------------------------------------

void transform_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &transform_system::init);
}

// ----------------------------------------------------------------------------

static void init_task(void*)
{
	engine::instance().add_system(std::unique_ptr<vlt::system>(new transform_system));
}

void transform_system::init(deserializer & deserializer)
{
	deserializer.enter_segment(get_label_static());
	add_task(init_task, nullptr, local_transform_component::metadata->thread, task_type::write);
}

// ============================================================================

transform_system::transform_system() :
	system(get_label_static()),
	m_entityWorldtransformCache()
{
}

// ----------------------------------------------------------------------------

transform_system::transform_system(transform_system && other) :
	system(std::move(other)),
	m_entityWorldtransformCache()
{
}

// ---------------------------------------------------------------------------

void transform_system::update(const r32 /*dt*/)
{
	m_entityWorldtransformCache = std::map<handle, m4>{ { handle::Invalid, m4::Identity } };
	const auto & engine = engine::instance();
	for (const auto & entity : engine.get_current_scene().get_entity_view<local_transform_component, world_transform_component>())
		updateWorldtransform(entity, m_entityWorldtransformCache);
}

// ============================================================================

struct update_world_transform_task_data
{
	handle entity_id;
	m4 * transform;
};

static void update_world_transform_task(void * mem)
{
	auto data = make_unique<update_world_transform_task_data>(mem);
	auto pool = engine::instance().get_current_scene().get_pool<world_transform_component>();
	pool->get<world_transform_component>(data->entity_id)->transform = *data->transform;
}

void transformSystemNamespace::updateWorldtransform(const transformEntity & entity, std::map<handle, m4> & worldtransformCache)
{
	const auto & engine = engine::instance();
	const auto & localtransformComponent = entity.get<local_transform_component>();
	const auto & worldtransformComponent = entity.get<world_transform_component>();
	const handle entity_id = entity.id;

	const auto it = worldtransformCache.find(localtransformComponent.parent_id);
	assert(it != worldtransformCache.end());

	{
		const m4 & parentWorldtransform = it != worldtransformCache.end() ? it->second : m4::Identity;
		const m4 & worldtransform = worldtransformCache[entity_id] = parentWorldtransform * localtransformComponent.transform;
		if (worldtransformComponent.transform != worldtransform)
			add_task(update_world_transform_task, new update_world_transform_task_data{ entity_id, &worldtransformCache[entity_id] }, world_transform_component::metadata->thread, task_type::write);
	}
}

// ============================================================================
