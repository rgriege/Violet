#include "violet/core/Engine.h"
#include "violet/core/window/MouseButton.h"
#include "violet/game/world/WorldSystem.h"
#include "violet/plugins/input/InputResult.h"

#include <iostream>

using namespace Violet;

extern "C" __declspec(dllexport) InputResult onMouseDown(const Entity & e, Engine & engine, const MouseButton button)
{
    if (button == MB_Left)
    {
        auto & ws = engine.fetch<WorldSystem>(); 
        ws.setTimeScale(ws.getTimeScale() / 2);
    }

    return InputResult::Block;
}
