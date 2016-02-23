// ============================================================================

#include <algorithm>
#include <thread>

#include "violet/component/scene.h"
#include "violet/core/engine.h"
#include "violet/log/log.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/file/file_serializer_factory.h"
#include "violet/serialization/serializer.h"
#include "violet/system/system.h"
#include "violet/system/system_factory.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/profiler.h"

using namespace vlt;

// ============================================================================

static std::unique_ptr<Engine> g_instance = nullptr;

// ============================================================================

Engine & Engine::instance()
{
	// For now, just don't...
	assert(g_instance != nullptr);

	return *g_instance;
}

// ----------------------------------------------------------------------------

bool Engine::bootstrap(const System_Factory & factory, const char * const config_file_name)
{
	// For now, just don't...
	assert(g_instance == nullptr);

	auto deserializer = vlt::File_Deserializer_Factory::instance().create(config_file_name);
	if (deserializer == nullptr || !deserializer->is_valid())
	{
		log("failed to read config file");
		return false;
	}

	// assign data to threads and start the Thread pool
	{
		auto thread_segment = deserializer->enter_segment("Thread");
		init_thread_pool(thread_segment->get_u32("worker_cnt"));

		while (thread_segment->is_valid())
		{
			const Tag tag(thread_segment->get_string("tag"));
			const u32 thread_idx = thread_segment->get_u32("thread_idx");
			assign_component_to_thread(tag, thread_idx);
		}
	} 

	g_instance.reset(new Engine);

	// create the systems
	{
		auto systems_segment = deserializer->enter_segment("sysv");
		if (systems_segment != nullptr)
		{
			while (systems_segment->is_valid())
			{
				const char * systemLabel = systems_segment->next_label();
				factory.create(systemLabel, *systems_segment);
			}
		}
	}

	finish_all_tasks();

	// create and run the first Scene if systems initalized properly
	if (g_instance->running)
	{
		g_instance->scene->load(deserializer->get_string("firstScene"));
		g_instance->begin();
		finish_all_tasks();
	}

	// cleanup
	g_instance->scene.reset();
	g_instance->next_scene_filename.clear();
	for (auto it = g_instance->systems.rbegin(), end = g_instance->systems.rend(); it != end; ++it)
		it->reset();
	g_instance->systems.clear();
	cleanup_thread_pool();
	g_instance.reset();
	return true;
}

// ============================================================================

Engine::~Engine()
{
}

// ----------------------------------------------------------------------------

void Engine::switch_scene(const char * filename)
{
	next_scene_filename = filename;
}

// ----------------------------------------------------------------------------

Scene & Engine::get_current_scene()
{
	return *scene;
}

// ----------------------------------------------------------------------------

const Scene & Engine::get_current_scene() const
{
	return *scene;
}

// ----------------------------------------------------------------------------

void Engine::stop()
{
	running = false;
}

// ----------------------------------------------------------------------------

void Engine::add_system(std::unique_ptr<vlt::System> && s)
{
	assert(s != nullptr);
	systems.emplace_back(std::move(s));
}

// ============================================================================

Engine::Engine() :
	systems(),
	scene(std::make_unique<Scene>()),
	next_scene_filename(),
	running(true)
{
}

// ----------------------------------------------------------------------------

void Engine::begin()
{
	r32 const target_frame_time = 1.f / 60.f;
	r32 previous_frame_time = target_frame_time;

	while (running)
	{
		{
			const Profiler::Block block("frame");
			run_frame(previous_frame_time);
		}

		r32 const frame_time = Profiler::instance().report("frame") / 1000000.f;
		if (frame_time < target_frame_time)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<u32>((target_frame_time - frame_time) * 1000)));
			previous_frame_time = target_frame_time;
		}
		else
		{
			previous_frame_time = frame_time;
			// log(Formatted_String<128>().sprintf("frame Timestamp: %.3f", previous_frame_time));
		}
	}
}

// ----------------------------------------------------------------------------

void Engine::run_frame(const r32 frame_time)
{
	std::for_each(std::begin(systems), std::end(systems), [&](std::unique_ptr<System> & system) { system->update(frame_time); });

	while (get_current_stage() != task_type::last)
		complete_frame_stage();
	complete_frame_stage();

	if (!next_scene_filename.empty())
	{
		scene->clear();
		scene->load(next_scene_filename.c_str());
	}
}

// ============================================================================
