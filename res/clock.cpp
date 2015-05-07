#include "engine/Engine.h"
#include "game/world/WorldSystem.h"
#include "engine/graphics/component/TextComponent.h"

#include <iostream>

using namespace Violet;

extern "C" __declspec(dllexport) void update(const Entity e, Engine & engine)
{
    engine.getCurrentScene().getComponent<TextComponent>(e)->m_text = engine.fetch<WorldSystem>()->getTime().toString();
}
