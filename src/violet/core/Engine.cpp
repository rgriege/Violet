#include "violet/core/Engine.h"

#include "violet/core/scene/Scene.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/System.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/utility/Time.h"

#include <iostream>
#include <algorithm>
#include <Windows.h>

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
}

using namespace EngineNamespace;

std::unique_ptr<Engine> Engine::init(SystemFactory & factory, Deserializer & deserializer)
{
	std::vector<std::unique_ptr<System>> systems;
	bool succeeded = true;
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

	std::unique_ptr<Scene> firstScene;
	if (succeeded)
	{
		auto optionsSegment = deserializer.enterSegment("opts");
		firstScene = Scene::create(optionsSegment->getString("firstScene"));
		succeeded = firstScene != nullptr;
	}

	return succeeded ? std::unique_ptr<Engine>(new Engine(std::move(systems), std::move(firstScene))) : nullptr;
}

void Engine::begin()
{
	uint32 const targetFrameTime = 1000 / 60;
	uint32 previousFrameTime = targetFrameTime;

	while (true)
	{
		uint32 const startTime = Time::getTimeInMilliseconds();

		float const deltaSeconds = previousFrameTime / 1000.f;
		std::for_each(std::begin(m_systems), std::end(m_systems), [&](std::unique_ptr<System> & system) { system->update(deltaSeconds, *this); });

		uint32 const frameTime = Time::getTimeInMilliseconds() - startTime;
		if (frameTime < targetFrameTime)
		{
			Sleep(targetFrameTime - frameTime);
			previousFrameTime = targetFrameTime;
		}
		else
		{
			printf("frame time: %.3f\n", frameTime / 1000.f);
			previousFrameTime = frameTime;
		}
	}
}

Engine::~Engine()
{
}

Engine::Engine(std::vector<std::unique_ptr<System>> && systems, std::unique_ptr<Scene> && firstScene) :
	m_activeScene(std::move(firstScene)),
	m_systems(std::move(systems))
{
}