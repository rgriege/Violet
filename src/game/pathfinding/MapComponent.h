#ifndef GAME_MapComponent_H
#define GAME_MapComponent_H

#include "violet/component/component.h"
#include "game/config.h"
#include "game/pathfinding/map.h"

namespace Violet
{
	class Deserializer;
	class Engine;
}

class VIOLET_GAME_API MapComponent : public Violet::ComponentBase<MapComponent>
{
public:

	static Violet::Tag getStaticTag();

public:

	MapComponent(Violet::EntityId entityId, Violet::Deserializer & Deserializer);
	MapComponent(MapComponent && other);

public:

	Map m_graph;
	uint32 m_id;
};

Violet::Serializer & operator<<(Violet::Serializer & Serializer, const MapComponent & component);

#endif
