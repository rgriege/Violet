#include "violet/game/pathfinding/PathComponent.h"

#include "violet/core/serialization/Deserializer.h"

const char * PathComponent::getLabel()
{
	return "path";
}

PathComponent::PathComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_path(),
	m_lastIntersection(0),
	m_speed(deserializer.getFloat("speed"))
{
}

PathComponent::PathComponent(const Entity & entity, const float speed, Path && path) :
	Component(entity),
	m_path(std::move(path)),
	m_lastIntersection(0),
	m_speed(speed)
{
}

PathComponent::PathComponent(PathComponent && other) :
	Component(other.getEntity()),
	m_path(std::move(other.m_path)),
	m_lastIntersection(other.m_lastIntersection),
	m_speed(other.m_speed)
{
}

PathComponent & PathComponent::operator=(PathComponent && other)
{
	Component::operator=(std::move(other));
	m_path = std::move(other.m_path);
	m_lastIntersection = other.m_lastIntersection;
	m_speed = other.m_speed;
	return *this;
}