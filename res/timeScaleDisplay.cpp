#include "engine/Engine.h"
#include "engine/utility/FormattedString.h"
#include "game/world/WorldSystem.h"
#include "engine/graphics/component/TextComponent.h"

using namespace Violet;

extern "C" __declspec(dllexport) void update(Entity & e, Engine & engine)
{
    auto & ws = engine.fetch<WorldSystem>(); 
    engine.fetch<TextComponent>(e).m_text = FormattedString<16>().sprintf("%.0fx", ws.getTimeScale());
}
