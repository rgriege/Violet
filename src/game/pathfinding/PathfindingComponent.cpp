// ============================================================================

#include "game/pathfinding/pathfindingcomponent.h"

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

// ============================================================================

Violet::Tag PathfindingComponent::getStaticTag()
{
	return Violet::Tag('p', 'f', 'n', 'd');
}

// ============================================================================

PathfindingComponent::PathfindingComponent(const Violet::EntityId entityId, Violet::Deserializer & Deserializer) :
	ComponentBase<PathfindingComponent>(entityId),
	m_mapId(Deserializer.getUint("mapId")),
	m_speed(Deserializer.getFloat("speed"))
{
}

// ============================================================================

Violet::Serializer & operator<<(Violet::Serializer & Serializer, const PathfindingComponent & component)
{
	Serializer.writeUint("mapId", component.m_mapId);
	Serializer.writeFloat("speed", component.m_speed);
	return Serializer;
}

// ============================================================================
