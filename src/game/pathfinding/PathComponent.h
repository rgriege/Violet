#ifndef GAME_PathComponent_H
#define GAME_PathComponent_H

#include "violet/component/Component.h"
#include "game/Config.h"
#include "game/pathfinding/Path.h"

namespace Violet
{
    class Serializer;
}

class VIOLET_GAME_API PathComponent : public Violet::ComponentBase<PathComponent>
{
public:

	static Violet::Tag getStaticTag();

public:

	PathComponent(Violet::EntityId entityId, Violet::Deserializer & deserializer);
	PathComponent(Violet::EntityId entityId, float speed, Path && path);
	PathComponent(PathComponent && other);
	//PathComponent & operator=(PathComponent && other);

private:

	PathComponent & operator=(const PathComponent & other) = delete;

public:

	Path m_path;
	uint32 m_lastIntersection;
	float m_speed;
};

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathComponent & component);

#endif
