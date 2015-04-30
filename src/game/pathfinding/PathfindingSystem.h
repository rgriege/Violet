#ifndef PATHFINDING_SYSTEM_H
#define PATHFINDING_SYSTEM_H

#include "engine/system/System.h"
#include "game/Config.h"
#include "game/pathfinding/Map.h"
#include "game/pathfinding/PathComponent.h"

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

	void createMap(Violet::Deserializer & deserializer, Violet::Engine & engine);

private:

	Map m_map;
};

#endif
