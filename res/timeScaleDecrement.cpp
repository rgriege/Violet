#include "violet/core/Engine.h"
#include "violet/game/world/WorldSystem.h"

using namespace Violet;

extern "C" __declspec(dllexport) void onMouseDown(Entity & e, Engine & engine)
{
    auto & ws = engine.fetch<WorldSystem>(); 
    ws.setTimeScale(ws.getTimeScale() / 2);
}
