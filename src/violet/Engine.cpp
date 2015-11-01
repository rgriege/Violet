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
		std::unique_ptr<Scene> scene = Scene::create(optionsSegment->getString("firstScene"));
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

// ----------------------------------------------------------------------------

EventContext & Engine::getEventContext()
{
	return m_eventContext;
}

// ----------------------------------------------------------------------------

const EventContext & Engine::getEventContext() const
{
	return m_eventContext;
}

// ============================================================================

Engine::Engine(std::vector<std::unique_ptr<System>> && systems, std::unique_ptr<Scene> && scene, const uint32 workerCount) :
	m_nextSceneFileName(),
	m_systems(std::move(systems)),
	m_scene(std::move(scene)),
	m_taskScheduler(workerCount),
	m_running(true)
{
}

// ============================================================================
