// ============================================================================

#include "violet/Engine.h"

#include "violet/log/Log.h"
#include "violet/scene/Scene.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/file/FileSerializerFactory.h"
#include "violet/serialization/Serializer.h"
#include "violet/system/System.h"
#include "violet/system/SystemFactory.h"
#include "violet/task/TaskScheduler.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/Profiler.h"

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
		Log::log("failed to read config file");
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

	ms_instance->m_taskScheduler->finishCurrentTasks();

	// create the first scene if systems initalized properly
	if (ms_instance->m_running)
	{
		ms_instance->m_scene = Scene::create(deserializer->getString("firstScene"));
		if (ms_instance->m_scene == nullptr)
		{
			Log::log("initial scene is invalid");
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

Engine::~Engine()
{
}

// ----------------------------------------------------------------------------

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

void Engine::addSceneDelegate(const SceneProcessor::Filter filter, const SceneProcessor::Delegate & delegate)
{
	m_sceneProcessor->addDelegate(filter, delegate);
}

// ----------------------------------------------------------------------------

void Engine::removeSceneDelegate(const SceneProcessor::Filter filter, const SceneProcessor::Delegate & delegate)
{
	m_sceneProcessor->removeDelegate(filter, delegate);
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
	m_systems(),
	m_scene(nullptr, Scene::destroy),
	m_sceneProcessor(std::make_unique<SceneProcessor>()),
	m_nextSceneFileName(),
	m_taskScheduler(std::make_unique<TaskScheduler>(workerCount)),
	m_taskQueues(),
	m_running(true),
	m_frameStage(FrameStage::Read)
{
}

// ----------------------------------------------------------------------------

void Engine::begin()
{
	float const targetFrameTime = 1.f / 60.f;
	float previousFrameTime = targetFrameTime;

	while (m_running)
	{
		{
			const Profiler::Block block("frame");
			runFrame(previousFrameTime);
		}

		float const frameTime = Profiler::getInstance().report("frame") / 1000000.f;
		if (frameTime < targetFrameTime)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<uint32>((targetFrameTime - frameTime) * 1000)));
			previousFrameTime = targetFrameTime;
		}
		else
		{
			previousFrameTime = frameTime;
			Log::log(FormattedString<128>().sprintf("frame time: %.3f", previousFrameTime));
		}
	}
}

// ----------------------------------------------------------------------------

void Engine::runFrame(const float frameTime)
{
	std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<System> & system) { system->update(frameTime); });
	if (m_scene != nullptr)
		m_sceneProcessor->process(*m_scene, frameTime);

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
	auto & taskQueue = m_taskQueues[static_cast<int>(m_frameStage)];
	auto & tasks = taskQueue.m_tasks;
	const std::lock_guard<std::mutex> lk(taskQueue.m_mutex);
	while (!tasks.empty())
	{
		m_taskScheduler->addTask(std::move(tasks.front().first), static_cast<int>(tasks.front().second));
		tasks.pop();
	}

	m_taskScheduler->finishCurrentTasks();

	m_frameStage = static_cast<FrameStage>((static_cast<int>(m_frameStage)+1) % static_cast<int>(FrameStage::Count));
}

// ----------------------------------------------------------------------------

void Engine::addTask(std::unique_ptr<Task> && task, const Thread thread, const FrameStage frameStage) thread_const
{
	assert(task != nullptr);
	if (frameStage == m_frameStage)
		m_taskScheduler->addTask(std::move(task), static_cast<int>(thread));
	else
	{
		auto & taskQueue = m_taskQueues[static_cast<int>(frameStage)];
		const std::lock_guard<std::mutex> lk(taskQueue.m_mutex);
		taskQueue.m_tasks.emplace(std::move(task), thread);
	}
}

// ============================================================================
