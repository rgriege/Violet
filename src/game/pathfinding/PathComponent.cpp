// ============================================================================

#include "game/pathfinding/PathComponent.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

// ============================================================================

Violet::Tag PathComponent::getStaticTag()
{
	return Violet::Tag('p', 'a', 't', 'h');
}

// ============================================================================

PathComponent::PathComponent(const Violet::Entity & owner, Violet::Deserializer & deserializer) :
	ComponentBase<PathComponent>(owner),
	m_path(),
	m_lastIntersection(0),
	m_speed(deserializer.getFloat("speed"))
{
}

// ----------------------------------------------------------------------------

PathComponent::PathComponent(const Violet::Entity & owner, const float speed, Path && path) :
	ComponentBase<PathComponent>(owner),
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

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathComponent & component)
{
	serializer.writeFloat("speed", component.m_speed);
	return serializer;
}

// ============================================================================
