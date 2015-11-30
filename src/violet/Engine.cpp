// ============================================================================

#include "violet/Engine.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/file/FileSerializerFactory.h"
#include "violet/serialization/Serializer.h"
#include "violet/system/System.h"
#include "violet/system/SystemFactory.h"
#include "violet/utility/Profiler.h"

#include <iostream>
#include <algorithm>

using namespace Violet;

// ============================================================================

namespace EngineNamespace
{
	std::unique_ptr<Engine> ms_instance = nullptr;
}

using namespace EngineNamespace;

// ============================================================================

const Engine & Engine::getInstance()
{
	// For now, just don't...
	assert(ms_instance != nullptr);

	return *ms_instance;
}

// ----------------------------------------------------------------------------

bool Engine::bootstrap(const SystemFactory & factory, const char * const configFileName)
{
	// For now, just don't...
	assert(ms_instance == nullptr);

	auto deserializer = Violet::FileDeserializerFactory::getInstance().create(configFileName);
	if (deserializer == nullptr || !*deserializer)
	{
		std::cout << "failed to read config file" << std::endl;
		return false;
	}

	// start the thread pool
	{
		auto optionsSegment = deserializer->enterSegment("opts");
		const uint32 workerCount = optionsSegment->getUint("workerCount");
		ms_instance.reset(new Engine(workerCount));
	}

	// create the systems
	{
		auto systemsSegment = deserializer->enterSegment("sysv");
		if (systemsSegment != nullptr)
		{
			while (*systemsSegment)
			{
				const char * systemLabel = systemsSegment->nextLabel();
				factory.create(systemLabel, *systemsSegment);
			}
		}
	}

	ms_instance->m_taskScheduler.finishCurrentTasks();

	// create the first scene if systems initalized properly
	if (ms_instance->m_running)
	{
		ms_instance->m_scene = Scene::create(deserializer->getString("firstScene"));
		if (ms_instance->m_scene == nullptr)
		{
			std::cout << "initial scene is invalid" << std::endl;
			ms_instance.reset();
			return false;
		}

		// run
		ms_instance->begin();
	}

	// cleanup
	ms_instance->m_scene.reset();
	ms_instance->m_systems.clear();

	// clear any pending scene change
	ms_instance->m_nextSceneFileName.clear();

	// run another frame to handle cleanup tasks
	ms_instance->runFrame(0.f);

	ms_instance.reset();
	return true;
}

// ============================================================================

void Engine::switchScene(const char * filename)
{
	m_nextSceneFileName = filename;
}

// ----------------------------------------------------------------------------

Scene & Engine::getCurrentScene()
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

const Scene & Engine::getCurrentScene() const
{
	return *m_scene;
}

// ----------------------------------------------------------------------------

void Engine::stop()
{
	m_running = false;
}

// ----------------------------------------------------------------------------

void Engine::addSystem(std::unique_ptr<System> && system)
{
	assert(system != nullptr);
	m_systems.emplace_back(std::move(system));
}

// ----------------------------------------------------------------------------

void Engine::addReadTask(std::unique_ptr<Task> && task, const Thread thread) thread_const
{
	addTask(std::move(task), thread, FrameStage::Read);
}

// ----------------------------------------------------------------------------

void Engine::addDeleteTask(std::unique_ptr<Task> && task, const Thread thread) thread_const
{
	addTask(std::move(task), thread, FrameStage::Delete);
}

// ============================================================================

Engine::Engine(const uint32 workerCount) :
	m_nextSceneFileName(),
	m_systems(),
	m_scene(nullptr, Scene::destroy),
	m_taskScheduler(workerCount),
	m_taskQueues(),
	m_running(true),
	m_frameStage(FrameStage::Read)
{
}

// ----------------------------------------------------------------------------

void Engine::begin()
{
	uint32 const targetFrameTime = 1000 / 60;
	uint32 previousFrameTime = targetFrameTime;
	uint32 framesSinceReport = 0;

	while (m_running)
	{
		{
			Profiler::Block("frame");
			runFrame(previousFrameTime / 1000.f);
		}

		auto const frameTime = static_cast<uint32>(Profiler::getInstance().report("frame"));
		if (frameTime < targetFrameTime)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameTime - frameTime));
			previousFrameTime = targetFrameTime;
		}
		else
		{
			printf("frame time: %.3f\n", frameTime / 1000.f);
			previousFrameTime = frameTime;
		}
	}
}

// ----------------------------------------------------------------------------

void Engine::runFrame(const float frameTime)
{
	std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<System> & system) { system->update(frameTime, *this); });

	if (!m_nextSceneFileName.empty())
	{
		m_scene = Scene::create(m_nextSceneFileName.c_str());
		m_nextSceneFileName.clear();
	}

	while (m_frameStage != FrameStage::Last)
		performCurrentFrameStage();
	performCurrentFrameStage();
}

// ----------------------------------------------------------------------------

void Engine::performCurrentFrameStage()
{
	m_taskScheduler.finishCurrentTasks();

	const bool addPendingTasks = m_frameStage != FrameStage::Last;
	m_frameStage = static_cast<FrameStage>((static_cast<int>(m_frameStage)+1) % static_cast<int>(FrameStage::Count));

	if (addPendingTasks)
	{
		auto & tasks = m_taskQueues[static_cast<int>(m_frameStage)].m_tasks;
		while (!tasks.empty())
		{
			m_taskScheduler.addTask(std::move(tasks.front().first), static_cast<int>(tasks.front().second));
			tasks.pop();
		}
	}
}

// ----------------------------------------------------------------------------

void Engine::addTask(std::unique_ptr<Task> && task, const Thread thread, const FrameStage frameStage) thread_const
{
	if (frameStage == m_frameStage)
		m_taskScheduler.addTask(std::move(task), static_cast<int>(thread));
	else
		m_taskQueues[static_cast<int>(frameStage)].m_tasks.emplace(std::move(task), thread);
}

// ============================================================================
