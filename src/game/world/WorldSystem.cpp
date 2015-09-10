#include "game/world/WorldSystem.h"

#include "violet/component/Component.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"

namespace WorldSystemNamespace
{
	const char * ms_label = "wrld";
}

using namespace WorldSystemNamespace;


void WorldSystem::install(Violet::SystemFactory & factory)
{
	factory.assign(ms_label, init);
}

std::unique_ptr<Violet::System> WorldSystem::init(Violet::Deserializer & deserializer)
{
	auto const settingsSegment = deserializer.enterSegment(ms_label);
	const float timeScale = settingsSegment->getFloat("timeScale");
	return std::unique_ptr<System>(new WorldSystem(timeScale));
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

void WorldSystem::update(const float dt, const Violet::Engine & /*engine*/)
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