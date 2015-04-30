#ifndef PATHFINDING_COMPONENT_H
#define PATHFINDING_COMPONENT_H

#include "engine/component/Component.h"
#include "game/Config.h"
#include "game/pathfinding/Path.h"

class Violet::Serializer;

class VIOLET_GAME_API PathComponent : public Violet::Component
{
public:

	static const char * getLabel();

public:

	PathComponent(const Violet::Entity & entity, Violet::Deserializer & deserializer);
	PathComponent(const Violet::Entity & entity, float speed, Path && path);
	PathComponent(PathComponent && other);
	PathComponent & operator=(PathComponent && other);

private:

	PathComponent & operator=(const PathComponent & other) = delete;

public:

	Path m_path;
	uint32 m_lastIntersection;
	float m_speed;
};

Violet::Serializer & operator<<(Violet::Serializer & serializer, const PathComponent & component);

#endif