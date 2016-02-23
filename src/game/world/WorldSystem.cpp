#include "game/world/worldsystem.h"

#include "violet/component/component.h"
#include "violet/engine.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/systemfactory.h"

namespace WorldSystemNamespace
{
	const char * ms_label = "wrld";
}

using namespace WorldSystemNamespace;


void WorldSystem::install(Violet::SystemFactory & Factory)
{
	Factory.assign(ms_label, init);
}

void WorldSystem::init(Violet::Deserializer & Deserializer)
{
	auto const settingsSegment = Deserializer.enterSegment(ms_label);
	const float timeScale = settingsSegment->getFloat("timeScale");

	Violet::Engine::getInstance().addWriteTask(Violet::Engine::getInstance(),
		[=](Violet::Engine & Engine)
		{
			Engine.addSystem(std::unique_ptr<System>(new WorldSystem(timeScale)));
		});
}

const char * WorldSystem::getStaticLabel()
{
	return ms_label;
}

WorldSystem::WorldSystem(const float timeScale) :
	System(ms_label),
	m_time(0),
	m_timeScale(timeScale)
{
}

void WorldSystem::update(const float dt)
{
	m_time += static_cast<uint64>(dt * m_timeScale);
}

const Violet::Time & WorldSystem::getTime() const
{
	return m_time;
}

float WorldSystem::getTimeScale() const
{
	return m_timeScale;
}

void WorldSystem::setTimeScale(const float timeScale)
{
	m_timeScale = timeScale;
}