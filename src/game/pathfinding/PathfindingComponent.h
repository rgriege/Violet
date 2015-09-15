#ifndef GAME_PathfindingComponent_H
#define GAME_PathfindingComponent_H

#include "violet/component/Component.h"
#include "game/Config.h"

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

	PathfindingComponent(Violet::Entity & owner, Violet::Deserializer & deserializer);

public:

	uint32 m_mapId;
	float m_speed;
};

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathfindingComponent & component);

#endif