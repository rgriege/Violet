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
#include "violet/task/task_scheduler.h"
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

const engine & engine::instance()
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

	// start the thread pool
	{
		auto optionsSegment = deserializer->enter_segment("opts");
		const u32 workerCount = optionsSegment->get_u32("workerCount");
		ms_instance.reset(new engine(workerCount));
	}

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

	ms_instance->m_taskScheduler->finishCurrentTasks();

	// create the first scene if systems initalized properly
	if (ms_instance->m_running)
	{
		ms_instance->m_scene->load(deserializer->get_string("firstScene"));
		ms_instance->begin();
	}

	// cleanup
	ms_instance->m_scene.reset();
	for (auto it = ms_instance->m_systems.rbegin(), end = ms_instance->m_systems.rend(); it != end; ++it)
		it->reset();
	ms_instance->m_systems.clear();

	// clear any pending scene change
	ms_instance->m_nextSceneFileName.clear();

	// run additional frames to handle cleanup tasks
	while (ms_instance->has_tasks())
		ms_instance->run_frame(0.f);

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

// ----------------------------------------------------------------------------

void engine::add_read_task(std::unique_ptr<task> && task, const thread thread) thread_const
{
	add_task(std::move(task), thread, FrameStage::Read);
}

// ----------------------------------------------------------------------------

void engine::add_delete_task(std::unique_ptr<task> && task, const thread thread) thread_const
{
	add_task(std::move(task), thread, FrameStage::Delete);
}

// ============================================================================

engine::engine(const u32 workerCount) :
	m_systems(),
	m_scene(std::make_unique<scene>()),
	m_nextSceneFileName(),
	m_taskScheduler(std::make_unique<task_scheduler>(workerCount)),
	m_taskQueues(),
	m_running(true),
	m_frameStage(FrameStage::Read)
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
			// log(formatted_string<128>().sprintf("frame timestamp: %.3f", previousFrameTime));
		}
	}
}

// ----------------------------------------------------------------------------

void engine::run_frame(const r32 frameTime)
{
	std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<system> & system) { system->update(frameTime); });

	if (!m_nextSceneFileName.empty())
	{
		m_scene->clear();
		m_scene->load(m_nextSceneFileName.c_str());
	}

	while (m_frameStage != FrameStage::Last)
		perform_current_frame_stage();
	perform_current_frame_stage();
}

// ----------------------------------------------------------------------------

void engine::perform_current_frame_stage()
{
	auto & taskQueue = m_taskQueues[static_cast<int>(m_frameStage)];
	auto & tasks = taskQueue.m_tasks;
	const std::lock_guard<std::mutex> lk(taskQueue.m_mutex);
	while (!tasks.empty())
	{
		m_taskScheduler->add_task(std::move(tasks.front().first), static_cast<int>(tasks.front().second));
		tasks.pop();
	}

	m_taskScheduler->finishCurrentTasks();

	m_frameStage = static_cast<FrameStage>((static_cast<int>(m_frameStage)+1) % static_cast<int>(FrameStage::Count));
}

// ----------------------------------------------------------------------------

void engine::add_task(std::unique_ptr<task> && task, const thread thread, const FrameStage frameStage) thread_const
{
	assert(task != nullptr);
	if (frameStage == m_frameStage)
		m_taskScheduler->add_task(std::move(task), static_cast<int>(thread));
	else
	{
		auto & taskQueue = m_taskQueues[static_cast<int>(frameStage)];
		const std::lock_guard<std::mutex> lk(taskQueue.m_mutex);
		taskQueue.m_tasks.emplace(std::move(task), thread);
	}
}

// ----------------------------------------------------------------------------

bool engine::has_tasks() const
{
	for (auto const & taskQueue : m_taskQueues)
		if (!taskQueue.m_tasks.empty())
			return true;
	return false;
}

// ============================================================================
