#include "engine/Engine.h"
#include "game/world/WorldSystem.h"
#include "engine/graphics/component/TextComponent.h"

using namespace Violet;

extern "C" __declspec(dllexport) void update(Entity & e, Engine & engine)
{
    engine.fetch<TextComponent>(e).m_text = engine.fetch<WorldSystem>().getTime().toString();
}
