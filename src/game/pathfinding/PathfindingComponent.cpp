// ============================================================================

#include "game/pathfinding/PathfindingComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

// ============================================================================

Tag PathfindingComponent::getTypeId()
{
	return Tag('p', 'f', 'n', 'd');
}

// ============================================================================

PathfindingComponent::PathfindingComponent(const Violet::Entity entity, Violet::Deserializer & deserializer) :
	Component<PathfindingComponent>(entity),
	m_mapEntity(deserializer.getUint("mapId"), 0),
	m_speed(deserializer.getFloat("speed"))
{
}

// ============================================================================

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathfindingComponent & component)
{
	serializer.writeUint("mapId", component.m_mapEntity.getId());
	serializer.writeFloat("speed", component.m_speed);
	return serializer;
}

// ============================================================================
