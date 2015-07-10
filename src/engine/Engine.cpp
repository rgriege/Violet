// ============================================================================

#include "engine/Engine.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/serialization/file/FileSerializerFactory.h"
#include "engine/serialization/Serializer.h"
#include "engine/system/System.h"
#include "engine/system/SystemFactory.h"
#include "engine/utility/Profiler.h"

#include <iostream>
#include <algorithm>

using namespace Violet;

// ============================================================================

namespace EngineNamespace
{
	void printWarning(const char * msg)
	{
		std::cout << msg << std::endl;
		char c;
		std::cin >> c;
		exit(1);
	}
}

using namespace EngineNamespace;

// ============================================================================

Engine::Task::Task(const Engine & engine, const uint32 priority) :
	Violet::Task(priority),
	m_engine(const_cast<Engine &>(engine))
{
}

// ============================================================================

std::unique_ptr<Engine> Engine::init(SystemFactory & factory, Deserializer & deserializer)
{
	bool succeeded = true;

	std::vector<std::unique_ptr<System>> systems;
	{
		auto systemsSegment = deserializer.enterSegment("sysv");
		while (*systemsSegment && succeeded)
		{
			const char * systemLabel = systemsSegment->nextLabel();
			auto system = factory.create(systemLabel, *systemsSegment);
			if (system != nullptr)
				systems.emplace_back(std::move(system));
			else
			{
				std::cout << "failed to init " << systemLabel << " system" << std::endl;
				succeeded = false;
			}
		}
	}

	if (succeeded)
	{
		auto optionsSegment = deserializer.enterSegment("opts");
		Scene scene = Scene::create(optionsSegment->getString("firstScene"));
		const uint32 workerCount = optionsSegment->getUint("workerCount");

		std::unique_ptr<Engine> engine(new Engine(std::move(systems), std::move(scene), workerCount));
		return std::move(engine);
	}

	return nullptr;
}

// ============================================================================

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

// ----------------------------------------------------------------------------

void Engine::switchScene(const char * filename)
{
	m_nextSceneFileName = filename;
}

// ----------------------------------------------------------------------------

Scene & Engine::getCurrentScene()
{
	return m_scene;
}

// ----------------------------------------------------------------------------

const Scene & Engine::getCurrentScene() const
{
	return m_scene;
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

Engine::Engine(std::vector<std::unique_ptr<System>> && systems, Scene && scene, const uint32 workerCount) :
	m_nextSceneFileName(),
	m_systems(std::move(systems)),
	m_scene(std::move(scene)),
	m_taskScheduler(workerCount),
	m_running(true)
{
}

// ============================================================================
