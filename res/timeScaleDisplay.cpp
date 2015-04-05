#include "violet/core/Engine.h"
#include "violet/core/utility/FormattedString.h"
#include "violet/game/world/WorldSystem.h"
#include "violet/plugins/graphics/component/TextComponent.h"

using namespace Violet;

extern "C" __declspec(dllexport) void update(Entity & e, Engine & engine)
{
    auto & ws = engine.fetch<WorldSystem>(); 
    engine.fetch<TextComponent>(e).m_text = FormattedString<16>().sprintf("%.0fx", ws.getTimeScale());
}
