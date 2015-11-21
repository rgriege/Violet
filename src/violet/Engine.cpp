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

	{
		auto optionsSegment = deserializer->enterSegment("opts");
		const uint32 workerCount = optionsSegment->getUint("workerCount");
		ms_instance.reset(new Engine(workerCount));
	}

	{
		auto systemsSegment = deserializer->enterSegment("sysv");
		if (systemsSegment != nullptr)
		{
			while (*systemsSegment)
			{
				const char * systemLabel = systemsSegment->nextLabel();
				auto system = factory.create(systemLabel, *systemsSegment);
				if (system != nullptr)
					ms_instance->m_systems.emplace_back(std::move(system));
				else
				{
					std::cout << "failed to init " << systemLabel << " system" << std::endl;
					ms_instance.reset();
					return false;
				}
			}
		}
	}

	ms_instance->m_scene = Scene::create(deserializer->getString("firstScene"));
	if (ms_instance->m_scene == nullptr)
	{
		std::cout << "initial scene is invalid" << std::endl;
		ms_instance.reset();
		return false;
	}

	ms_instance->begin();

	ms_instance->m_scene.reset();
	ms_instance->m_systems.clear();
	ms_instance->m_taskScheduler.finishCurrentTasks();

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

void Engine::addTask(std::unique_ptr<Task> && task) const
{
	m_taskScheduler.addTask(std::move(task));
}

// ============================================================================

Engine::Engine(const uint32 workerCount) :
	m_nextSceneFileName(),
	m_systems(),
	m_scene(nullptr, Scene::destroy),
	m_taskScheduler(workerCount),
	m_running(true)
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

	m_taskScheduler.finishCurrentTasks();
}

// ============================================================================
