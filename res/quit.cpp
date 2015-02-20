#include "violet/core/Engine.h"

using namespace Violet;

extern "C" __declspec(dllexport) void onMouseDown(const Entity & e, Engine & engine)
{
    engine.stop();
}
