#include "violet/core/Engine.h"
#include "violet/core/script/CppScript.h"
#include "violet/core/transform/TransformComponent.h"
#include "violet/game/pathfinding/Path.h"
#include "violet/game/pathfinding/PathfindingSystem.h"
#include "violet/game/world/WorldSystem.h"

using namespace Violet;

struct Mem
{
    bool working, traveling;
};

void travel(Entity & e, Engine & engine, Mem * mem);
void work(Mem * mem);
void rest(Mem * mem);

extern "C" __declspec(dllexport) void init(CppScript::Allocator & allocator)
{
    Mem * mem = static_cast<Mem*>(allocator.allocate<Mem>());
    mem->traveling = mem->working = false;
}

extern "C" __declspec(dllexport) void update(Entity & e, Engine & engine, Mem * mem)
{
    auto & ws = engine.fetch<WorldSystem>(); 
    auto const hours = ws.getTime().getHours();
    if (hours >= 8)
    {
        if (hours < 17)
        {
            if (!mem->working)
            {
                if (!mem->traveling)
                    travel(e, engine, mem);
                else if (!engine.fetch<PathfindingSystem>().has(e))
                    work(mem);
            }
        }
        else
        {
            if (mem->working)
                travel(e, engine, mem);
            else if (mem->traveling && !engine.fetch<PathfindingSystem>().has(e))
                rest(mem);
        }
    }
}

void travel(Entity & e, Engine & engine, Mem * mem)
{
    auto & ws = engine.fetch<WorldSystem>(); 
    auto & ps = engine.fetch<PathfindingSystem>();
    auto & tc = engine.fetch<TransformComponent>(e);
    const Vec2f goal = mem->working ? Vec2f(0, -270.f) : Vec2f(200, 120);
    ps.create(e, 1.f, ps.getPath(tc.m_position, goal));
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
