#ifndef GAME_PathfindingComponent_H
#define GAME_PathfindingComponent_H

#include "engine/component/Component.h"
#include "game/Config.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
}

class VIOLET_GAME_API PathfindingComponent : public Violet::Component<PathfindingComponent>
{
public:

	static Violet::Tag getTypeId();

public:

	PathfindingComponent(Violet::Entity entity, Violet::Deserializer & deserializer);

public:

	Violet::Entity m_mapEntity;
	float m_speed;
};

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathfindingComponent & component);

#endif