#include "engine/Engine.h"
#include "engine/script/component/CppScriptComponent.h"
#include "engine/transform/component/TransformComponent.h"
#include "game/pathfinding/PathComponent.h"
#include "game/pathfinding/PathfindingSystem.h"
#include "game/world/WorldSystem.h"

using namespace Violet;

struct Mem
{
    bool working, traveling;
};

void travel(const Entity e, Engine & engine, Mem * mem);
void work(Mem * mem);
void rest(Mem * mem);

extern "C" __declspec(dllexport) void init(CppScriptComponent::Allocator & allocator)
{
    Mem * mem = allocator.allocate<Mem>();
    mem->traveling = mem->working = false;
}

extern "C" __declspec(dllexport) void update(const Entity e, Engine & engine, Mem * mem)
{
    auto * ws = engine.fetch<WorldSystem>(); 
    auto const hours = ws->getTime().getHours();
    if (hours >= 8)
    {
        if (hours < 17)
        {
            if (!mem->working)
            {
                if (!mem->traveling)
                    travel(e, engine, mem);
                else if (!engine.getCurrentScene().hasComponent<PathComponent>(e))
                    work(mem);
            }
        }
        else
        {
            if (mem->working)
                travel(e, engine, mem);
            else if (mem->traveling && !engine.getCurrentScene().hasComponent<PathComponent>(e))
                rest(mem);
        }
    }
}

void travel(const Entity e, Engine & engine, Mem * mem)
{
    auto * ps = engine.fetch<PathfindingSystem>();
    const Vec2f goal = mem->working ? Vec2f(0, -270.f) : Vec2f(200, 120);
    ps->path(engine.getCurrentScene(), e, goal);
    mem->traveling = true;
    mem->working = false;
}

void work(Mem * mem)
{
    mem->traveling = false;
    mem->working = true;
}

void rest(Mem * mem)
{
    mem->traveling = false;
    mem->working = false;
}
