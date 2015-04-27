#include "violet/game/pathfinding/PathComponent.h"

#include "violet/core/serialization/Deserializer.h"
#include "violet/core/serialization/Serializer.h"

const char * PathComponent::getLabel()
{
	return "path";
}

PathComponent::PathComponent(const Violet::Entity & entity, Violet::Deserializer & deserializer) :
	Component(entity),
	m_path(),
	m_lastIntersection(0),
	m_speed(deserializer.getFloat("speed"))
{
}

PathComponent::PathComponent(const Violet::Entity & entity, const float speed, Path && path) :
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

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathComponent & component)
{
	serializer.writeFloat("speed", component.m_speed);
	return serializer;
}
