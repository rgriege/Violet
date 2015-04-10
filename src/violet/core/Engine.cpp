#include "violet/core/Engine.h"

#include "violet/core/Entity/EntityFactory.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/serialization/FileDeserializerFactory.h"
#include "violet/core/system/System.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/window/WindowSystem.h"

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace Violet;

namespace EngineNamespace
{
	void printWarning(const char * msg)
	{
		std::cout << msg << std::endl;
		char c;
		std::cin >> c;
		exit(1);
	}

	bool createScene(const char * filename, SceneInitContext & initContext);
}

using namespace EngineNamespace;

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

	std::unique_ptr<Engine> engine(new Engine(std::move(systems)));
	if (succeeded)
	{
		auto optionsSegment = deserializer.enterSegment("opts");
		succeeded = createScene(optionsSegment->getString("firstScene"), SceneInitContext(*engine));
	}

	return succeeded ? std::move(engine) : nullptr;
}

Engine::Engine(Engine && other) :
	m_systems(std::move(other.m_systems)),
	m_nextSceneFileName(other.m_nextSceneFileName),
	m_running(other.m_running)
{
}

Engine & Engine::operator=(Engine && other)
{
	m_systems = std::move(other.m_systems);
	m_nextSceneFileName = other.m_nextSceneFileName;
	m_running = other.m_running;
	return *this;
}

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

void Engine::runFrame(const float frameTime)
{
	const auto startTime = std::chrono::system_clock::now();

	std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<System> & system) { system->update(frameTime, *this); });

	if (!m_nextSceneFileName.empty())
	{
		for (auto & system : m_systems)
			system->clear();
		createScene(m_nextSceneFileName.c_str(), SceneInitContext(*this));
		m_nextSceneFileName.clear();
	}
}

void Engine::switchScene(const char * filename)
{
	m_nextSceneFileName = filename;
}

void Engine::stop()
{
	m_running = false;
}

Engine::~Engine()
{
	for (auto const & system : m_systems)
	{
		system->bind(m_entityFactory);
		system->unbind(m_componentFactory);
	}
}

Engine::Engine(std::vector<std::unique_ptr<System>> && systems) :
	m_nextSceneFileName(),
	m_systems(std::move(systems)),
	m_entityFactory(),
	m_componentFactory(),
	m_running(true)
{
	for (auto const & system : m_systems)
	{
		system->bind(m_entityFactory);
		system->bind(m_componentFactory);
	}
}


bool EngineNamespace::createScene(const char * filename, SceneInitContext & initContext)
{
	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
	{
		std::cout << "Could not open scene file " << filename << std::endl;
		return false;
	}
	else if (!*deserializer)
	{
		std::cout << "Failed to parse scene file " << filename << std::endl;
		return false;
	}
	else
	{
		while (*deserializer)
			initContext.createEntity(deserializer->nextLabel(), *deserializer);
	}

	return true;
}