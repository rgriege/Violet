#include "violet/core/Engine.h"
#include "violet/game/world/WorldSystem.h"
#include "violet/plugins/graphics/component/TextComponent.h"

using namespace Violet;

extern "C" __declspec(dllexport) void update(Entity & e, Engine & engine)
{
    engine.fetch<TextComponent>(e).m_text = engine.fetch<WorldSystem>().getTime().toString();
}
