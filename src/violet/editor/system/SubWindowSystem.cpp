#include "violet/editor/system/SubWindowSystem.h"

void SubWindowSystem::install(Violet::SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &SubWindowSystem::init);
}

std::unique_ptr<Violet::System> SubWindowSystem::init(Violet::Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	return std::unique_ptr<Violet::System>(new SubWindowSystem(1600, 1200));
}

SubWindowSystem::SubWindowSystem(const int width, const int height) :
	m_width(width),
	m_height(height)
{
}

void SubWindowSystem::render()
{
}

bool SubWindowSystem::getEvent(EventType /*type*/, Event* /*event*/)
{
	return false;
}

void SubWindowSystem::addEvent(Event /*event*/)
{
}

int SubWindowSystem::getWidth() const
{
	return m_width;
}

int SubWindowSystem::getHeight() const
{
	return m_height;
}