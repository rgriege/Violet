#ifndef GAME_PathfindingComponent_H
#define GAME_PathfindingComponent_H

#include "violet/component/component.h"
#include "game/config.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
}

class VIOLET_GAME_API PathfindingComponent : public Violet::ComponentBase<PathfindingComponent>
{
public:

	static Violet::Tag getStaticTag();

public:

	PathfindingComponent(Violet::EntityId entityId, Violet::Deserializer & Deserializer);

public:

	uint32 m_mapId;
	float m_speed;
};

Violet::Serializer & operator<<(Violet::Serializer & Serializer, const PathfindingComponent & component);

#endif
