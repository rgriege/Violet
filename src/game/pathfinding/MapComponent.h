#ifndef GAME_MapComponent_H
#define GAME_MapComponent_H

#include "engine/component/Component.h"
#include "engine/entity/Entity.h"
#include "game/Config.h"
#include "game/pathfinding/Map.h"

namespace Violet
{
	class Deserializer;
	class Engine;
}

class VIOLET_GAME_API MapComponent : public Violet::Component<MapComponent>
{
public:

	static Violet::Tag getTypeId();

public:

	MapComponent(Violet::Entity entity, Violet::Deserializer & deserializer);

public:

	Map m_graph;
};

#endif