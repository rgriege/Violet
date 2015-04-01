#include "violet/game/pathfinding/PathfindingComponent.h"

#include "violet/core/serialization/Deserializer.h"

const char * PathfindingComponent::getLabel()
{
	return "path";
}

PathfindingComponent::PathfindingComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_path(),
	m_lastIntersection(0),
	m_speed(deserializer.getFloat("speed"))
{
}

PathfindingComponent::PathfindingComponent(const Entity & entity, const float speed, Path && path) :
	Component(entity),
	m_path(std::move(path)),
	m_lastIntersection(0),
	m_speed(speed)
{
}

PathfindingComponent::PathfindingComponent(PathfindingComponent && other) :
	Component(other.getEntity()),
	m_path(std::move(other.m_path)),
	m_lastIntersection(other.m_lastIntersection),
	m_speed(other.m_speed)
{
}

PathfindingComponent & PathfindingComponent::operator=(PathfindingComponent && other)
{
	Component::operator=(std::move(other));
	m_path = std::move(other.m_path);
	m_lastIntersection = other.m_lastIntersection;
	m_speed = other.m_speed;
	return *this;
}