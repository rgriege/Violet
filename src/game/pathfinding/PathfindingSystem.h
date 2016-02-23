#ifndef GAME_PathfindingSystem_H
#define GAME_PathfindingSystem_H

#include "violet/system/system.h"
#include "game/config.h"
#include "game/pathfinding/path.h"
#include "game/pathfinding/pathfindingcomponent.h"

namespace Violet
{
	class Scene;
	class SystemFactory;
}

class VIOLET_GAME_API PathfindingSystem : public Violet::System
{
public:

	static const char * getStaticLabel();
	static void install(Violet::SystemFactory & Factory);
	static void init(Violet::Deserializer & Deserializer);

public:

	PathfindingSystem(PathfindingSystem && other);

	virtual ~PathfindingSystem() override;
	void update(float dt) override;

	void path(Violet::Scene & Scene, Violet::Entity & entity, const Vec2f & goal);

private:

	PathfindingSystem();

	void createMap(Violet::Deserializer & Deserializer, Violet::Engine & Engine);
};

#endif
