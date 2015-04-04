#ifndef PATHFINDING_SYSTEM_H
#define PATHFINDING_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/game/Config.h"
#include "violet/game/pathfinding/Map.h"
#include "violet/game/pathfinding/PathfindingComponent.h"

class VIOLET_GAME_API PathfindingSystem : public Violet::ComponentSystem<PathfindingComponent>
{
public:

	static void install(SystemFactory & factory);
	static std::unique_ptr<System> init(Deserializer & deserializer);

public:

	virtual ~PathfindingSystem() override;
	virtual void update(float dt, Engine & engine);

	Path getPath(const Vec2f & start, const Vec2f & goal);

private:

	PathfindingSystem();
	PathfindingSystem(Deserializer & deserializer);

private:

	Map m_map;
};

#endif
