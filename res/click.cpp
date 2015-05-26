#include "engine/Engine.h"
#include "engine/window/MouseButton.h"
#include "engine/input/InputResult.h"
#include "engine/window/WindowSystem.h"

using namespace Violet;

extern "C" __declspec(dllexport) InputResult onMouseDown(const Entity e, Engine & engine, const WindowSystem::MouseEvent & event)
{
    if (event.button == MB_Left)
        engine.switchScene("level1.json");

    return InputResult::Block;
}
