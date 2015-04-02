#include "violet/core/Engine.h"
#include "violet/core/transform/TransformComponent.h"
#include "violet/game/pathfinding/Path.h"
#include "violet/game/pathfinding/PathfindingSystem.h"

using namespace Violet;

extern "C" __declspec(dllexport) void onMouseDown(Entity & e, Engine & engine)
{
    auto & ps = engine.fetch<PathfindingSystem>();
    if (!ps.has(e))
    {
        auto & tc = engine.fetch<TransformComponent>(e);
        ps.create(e, 50.f, ps.getPath(tc.m_position, -tc.m_position));
    }
}
