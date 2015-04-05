#include "violet/core/Engine.h"
#include "violet/plugins/graphics/Color.h"
#include "violet/plugins/graphics/component/RenderComponent.h"

using namespace Violet;

extern "C" __declspec(dllexport) void onMouseIn(Entity & e, Engine & engine)
{
    auto & rc = engine.fetch<RenderComponent>(e);
    rc.m_color = Color::kGreen;
}

extern "C" __declspec(dllexport) void onMouseOut(Entity & e, Engine & engine)
{
    auto & rc = engine.fetch<RenderComponent>(e);
    rc.m_color = Color::kRed;
}
