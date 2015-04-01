#ifndef PATHFINDING_COMPONENT_H
#define PATHFINDING_COMPONENT_H

#include "violet/core/component/Component.h"
#include "violet/game/Config.h"
#include "violet/game/pathfinding/Path.h"

class VIOLET_GAME_API PathfindingComponent : public Component
{
public:

	static const char * getLabel();

public:

	PathfindingComponent(const Entity & entity, Deserializer & deserializer);
	PathfindingComponent(const Entity & entity, float speed, Path && path);
	PathfindingComponent(PathfindingComponent && other);
	PathfindingComponent & operator=(PathfindingComponent && other);

private:

	PathfindingComponent & operator=(const PathfindingComponent & other) = delete;

public:

	Path m_path;
	uint32 m_lastIntersection;
	float m_speed;
};

#endif