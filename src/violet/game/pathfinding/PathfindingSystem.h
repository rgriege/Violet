#ifndef PATHFINDING_SYSTEM_H
#define PATHFINDING_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/game/Config.h"
#include "violet/game/pathfinding/Map.h"
#include "violet/game/pathfinding/PathComponent.h"

namespace Violet
{
	class SceneInitContext;
}

class VIOLET_GAME_API PathfindingSystem : public Violet::ComponentSystem<PathComponent>
{
public:

	static void install(Violet::SystemFactory & factory);
	static std::unique_ptr<Violet::System> init(Violet::Deserializer & deserializer);

public:

	PathfindingSystem(PathfindingSystem && other);

	virtual ~PathfindingSystem() override;
	virtual void bind(Violet::EntityFactory & factory) override;
	virtual void unbind(Violet::EntityFactory & factory) override;
	virtual void update(float dt, Violet::Engine & engine) override;

	Path getPath(const Vec2f & start, const Vec2f & goal);

private:

	PathfindingSystem();

	void createMap(Violet::Deserializer & deserializer, Violet::SceneInitContext & initContext);

private:

	Map m_map;
};

#endif
