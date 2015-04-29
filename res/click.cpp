#include "violet/core/Engine.h"
#include "violet/core/window/MouseButton.h"

using namespace Violet;

extern "C" __declspec(dllexport) void onMouseDown(const Entity & e, Engine & engine, const MouseButton button)
{
    if (button == MB_Left)
        engine.switchScene("level1.json");
}
