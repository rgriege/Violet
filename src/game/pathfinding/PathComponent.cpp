// ============================================================================

#include "game/pathfinding/pathcomponent.h"

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

// ============================================================================

Violet::Tag PathComponent::getStaticTag()
{
	return Violet::Tag('p', 'a', 't', 'h');
}

// ============================================================================

PathComponent::PathComponent(const Violet::EntityId entityId, Violet::Deserializer & Deserializer) :
	ComponentBase<PathComponent>(entityId),
	m_path(),
	m_lastIntersection(0),
	m_speed(Deserializer.getFloat("speed"))
{
}

// ----------------------------------------------------------------------------

PathComponent::PathComponent(const Violet::EntityId entityId, const float speed, Path && path) :
	ComponentBase<PathComponent>(entityId),
	m_path(std::move(path)),
	m_lastIntersection(0),
	m_speed(speed)
{
}

// ----------------------------------------------------------------------------

PathComponent::PathComponent(PathComponent && other) :
	ComponentBase<PathComponent>(std::move(other)),
	m_path(std::move(other.m_path)),
	m_lastIntersection(other.m_lastIntersection),
	m_speed(other.m_speed)
{
}

// ----------------------------------------------------------------------------

//PathComponent & PathComponent::operator=(PathComponent && other)
//{
//	Component<PathComponent>::operator=(std::move(other));
//	m_path = std::move(other.m_path);
//	m_lastIntersection = other.m_lastIntersection;
//	m_speed = other.m_speed;
//	return *this;
//}

// ============================================================================

Violet::Serializer & operator<<(Violet::Serializer & Serializer, const PathComponent & component)
{
	Serializer.writeFloat("speed", component.m_speed);
	return Serializer;
}

// ============================================================================
