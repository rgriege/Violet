// ============================================================================

#include "violet/core/engine.h"

#include "violet/component/scene.h"
#include "violet/log/Log.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/file/file_serializer_factory.h"
#include "violet/serialization/serializer.h"
#include "violet/system/system.h"
#include "violet/system/system_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/profiler.h"

#include <algorithm>

using namespace vlt;

// ============================================================================

namespace engineNamespace
{
	std::unique_ptr<engine> ms_instance = nullptr;
}

using namespace engineNamespace;

// ============================================================================

engine & engine::instance()
{
	// For now, just don't...
	assert(ms_instance != nullptr);

	return *ms_instance;
}

// ----------------------------------------------------------------------------

bool engine::bootstrap(const system_factory & factory, const char * const configFileName)
{
	// For now, just don't...
	assert(ms_instance == nullptr);

	auto deserializer = vlt::file_deserializer_factory::instance().create(configFileName);
	if (deserializer == nullptr || !deserializer->is_valid())
	{
		log("failed to read config file");
		return false;
	}

	// assign data to threads and start the thread pool
	{
		auto thread_segment = deserializer->enter_segment("thread");
		init_thread_pool(thread_segment->get_u32("worker_cnt"));

		while (thread_segment->is_valid())
		{
			const tag tag(thread_segment->get_string("tag"));
			const u32 thread_idx = thread_segment->get_u32("thread_idx");
			assign_component_to_thread(tag, thread_idx);
		}
	} 

	ms_instance.reset(new engine);

	// create the systems
	{
		auto systemsSegment = deserializer->enter_segment("sysv");
		if (systemsSegment != nullptr)
		{
			while (systemsSegment->is_valid())
			{
				const char * systemLabel = systemsSegment->next_label();
				factory.create(systemLabel, *systemsSegment);
			}
		}
	}

	finish_all_tasks();

	// create and run the first scene if systems initalized properly
	if (ms_instance->m_running)
	{
		ms_instance->m_scene->load(deserializer->get_string("firstScene"));
		ms_instance->begin();
		finish_all_tasks();
	}

	// cleanup
	ms_instance->m_scene.reset();
	ms_instance->m_nextSceneFileName.clear();
	for (auto it = ms_instance->m_systems.rbegin(), end = ms_instance->m_systems.rend(); it != end; ++it)
		it->reset();
	ms_instance->m_systems.clear();
	cleanup_thread_pool();
	ms_instance.reset();
	return true;
}

// ============================================================================

engine::~engine()
{
}

// ----------------------------------------------------------------------------

void engine::switch_scene(const char * filename)
{
	m_nextSceneFileName = filename;
}

// ----------------------------------------------------------------------------

scene & engine::get_current_scene()
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

const scene & engine::get_current_scene() const
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

void engine::stop()
{
	m_running = false;
}

// ----------------------------------------------------------------------------

void engine::add_system(std::unique_ptr<vlt::system> && s)
{
	assert(s != nullptr);
	m_systems.emplace_back(std::move(s));
}

// ============================================================================

engine::engine() :
	m_systems(),
	m_scene(std::make_unique<scene>()),
	m_nextSceneFileName(),
	m_running(true)
{
}

// ----------------------------------------------------------------------------

void engine::begin()
{
	r32 const targetFrameTime = 1.f / 60.f;
	r32 previousFrameTime = targetFrameTime;

	while (m_running)
	{
		{
			const profiler::block block("frame");
			run_frame(previousFrameTime);
		}

		r32 const frameTime = profiler::instance().report("frame") / 1000000.f;
		if (frameTime < targetFrameTime)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<u32>((targetFrameTime - frameTime) * 1000)));
			previousFrameTime = targetFrameTime;
		}
		else
		{
			previousFrameTime = frameTime;
			log(formatted_string<128>().sprintf("frame timestamp: %.3f", previousFrameTime));
		}
	}
}

// ----------------------------------------------------------------------------

void engine::run_frame(const r32 frameTime)
{
	std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<system> & system) { system->update(frameTime); });

	while (get_current_stage() != task_type::last)
		complete_frame_stage();
	complete_frame_stage();

	if (!m_nextSceneFileName.empty())
	{
		m_scene->clear();
		m_scene->load(m_nextSceneFileName.c_str());
	}
}

// ============================================================================
