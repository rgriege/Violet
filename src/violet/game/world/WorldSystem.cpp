#include "violet/game/world/WorldSystem.h"

#include "violet/core/component/Component.h"

namespace WorldSystemNamespace
{
	const char * ms_label = "wrld";
	std::unique_ptr<Violet::Component> ms_component;
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

bool WorldSystem::owns(const char * /*label*/) const
{
	return false;
}

bool WorldSystem::has(const char * /*label*/, const Violet::Entity & /*entity*/) const
{
	return false;
}

Violet::Component & WorldSystem::fetch(const char * /*label*/, const Violet::Entity & entity)
{
	ms_component = std::make_unique<Violet::Component>(entity);
	return *ms_component;
}

void WorldSystem::update(const float dt, Violet::Engine & /*engine*/)
{
	m_time += static_cast<uint64>(dt * m_timeScale);
}

void WorldSystem::clear()
{
	m_time = Violet::Time(0);
}

const Violet::Time & WorldSystem::getTime() const
{
	return m_time;
}

float WorldSystem::getTimeScale() const
{
	return m_timeScale;
}