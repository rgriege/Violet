// ============================================================================

#include "game/pathfinding/PathfindingComponent.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

// ============================================================================

Violet::Tag PathfindingComponent::getStaticTag()
{
	return Violet::Tag('p', 'f', 'n', 'd');
}

// ============================================================================

PathfindingComponent::PathfindingComponent(Violet::Entity & owner, Violet::Deserializer & deserializer) :
	ComponentBase<PathfindingComponent>(owner),
	m_mapId(deserializer.getUint("mapId")),
	m_speed(deserializer.getFloat("speed"))
{
}

// ============================================================================

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathfindingComponent & component)
{
	serializer.writeUint("mapId", component.m_mapId);
	serializer.writeFloat("speed", component.m_speed);
	return serializer;
}

// ============================================================================
