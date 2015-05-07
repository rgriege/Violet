// ============================================================================

#include "game/pathfinding/PathComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

// ============================================================================

Tag PathComponent::getTypeId()
{
	return Tag('p', 'a', 't', 'h');
}

// ============================================================================

PathComponent::PathComponent(const Violet::Entity entity, Violet::Deserializer & deserializer) :
	Component<PathComponent>(entity),
	m_path(),
	m_lastIntersection(0),
	m_speed(deserializer.getFloat("speed"))
{
}

// ----------------------------------------------------------------------------

PathComponent::PathComponent(const Violet::Entity entity, const float speed, Path && path) :
	Component<PathComponent>(entity),
	m_path(std::move(path)),
	m_lastIntersection(0),
	m_speed(speed)
{
}

// ----------------------------------------------------------------------------

PathComponent::PathComponent(PathComponent && other) :
	Component<PathComponent>(other.getEntity()),
	m_path(std::move(other.m_path)),
	m_lastIntersection(other.m_lastIntersection),
	m_speed(other.m_speed)
{
}

// ----------------------------------------------------------------------------

PathComponent & PathComponent::operator=(PathComponent && other)
{
	Component<PathComponent>::operator=(std::move(other));
	m_path = std::move(other.m_path);
	m_lastIntersection = other.m_lastIntersection;
	m_speed = other.m_speed;
	return *this;
}

// ============================================================================

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathComponent & component)
{
	serializer.writeFloat("speed", component.m_speed);
	return serializer;
}

// ============================================================================
