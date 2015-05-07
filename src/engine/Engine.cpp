// ============================================================================

#include "engine/Engine.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/serialization/file/FileSerializerFactory.h"
#include "engine/serialization/Serializer.h"
#include "engine/system/System.h"
#include "engine/system/SystemFactory.h"
#include "engine/window/WindowSystem.h"

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

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
		Scene scene = Scene::create(optionsSegment->getString("firstScene"));

		std::unique_ptr<Engine> engine(new Engine(std::move(systems), std::move(scene)));
		return std::move(engine);
	}

	return nullptr;
}

// ============================================================================

Engine::~Engine()
{
}

// ----------------------------------------------------------------------------

Engine::Engine(Engine && other) :
	m_systems(std::move(other.m_systems)),
	m_nextSceneFileName(other.m_nextSceneFileName),
	m_running(other.m_running)
{
}

// ----------------------------------------------------------------------------

Engine & Engine::operator=(Engine && other)
{
	m_systems = std::move(other.m_systems);
	m_nextSceneFileName = other.m_nextSceneFileName;
	m_running = other.m_running;
	return *this;
}

// ----------------------------------------------------------------------------

void Engine::begin()
{
	uint32 const targetFrameTime = 1000 / 60;
	uint32 previousFrameTime = targetFrameTime;

	while (m_running)
	{
		const auto startTime = std::chrono::system_clock::now();
		runFrame(previousFrameTime / 1000.f);

		auto const frameTime = static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count());
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
	const auto startTime = std::chrono::system_clock::now();

	std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<System> & system) { system->update(frameTime, *this); });

	if (!m_nextSceneFileName.empty())
	{
		m_scene = Scene::create(m_nextSceneFileName.c_str());
		m_nextSceneFileName.clear();
	}
}

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

void Engine::stop()
{
	m_running = false;
}

// ============================================================================

Engine::Engine(std::vector<std::unique_ptr<System>> && systems, Scene && scene) :
	m_nextSceneFileName(),
	m_systems(std::move(systems)),
	m_scene(std::move(scene)),
	m_running(true)
{
}

// ============================================================================
