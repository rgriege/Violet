#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"
#include "violet/graphics/Color.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/update/system/UpdateSystem.h"

#include <algorithm>
#include <functional>

using namespace Violet;

struct Mem : public CppScript::Memory
{
    Mem(uint8 _opacity) : opacity(_opacity), disappearing(true) {}
    virtual ~Mem() override = default;
    uint8 opacity;
    bool disappearing;
};

const uint8 ms_maxOpacity = 0xCC;
const uint8 ms_minOpacity = 0x33;
const int8 ms_transitionDelta = 5;

void onMouseIn(const Entity & entity, const Engine & engine, Mem & mem);
InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event, Mem & mem);
void onMouseOut(const Entity & entity, const Engine & engine, Mem & mem);
void update(const Entity & entity, const Engine & engine, float dt, Mem & mem);

void adjustOpacity(const Entity & entity, const Engine & engine, Mem & mem, int diff);

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    auto m = std::make_unique<Mem>(ms_minOpacity);

    using namespace std::placeholders;
    MouseInMethod::assign(script, std::bind(onMouseIn, _1, _2, std::ref(*m)));
    MouseDownMethod::assign(script, std::bind(onMouseDown, _1, _2, _3, std::ref(*m)));
    MouseOutMethod::assign(script, std::bind(onMouseOut, _1, _2, std::ref(*m)));
    UpdateMethod::assign(script, std::bind(update, _1, _2, _3, std::ref(*m)));

    mem = std::move(m);
}

VIOLET_SCRIPT_EXPORT void clean(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    MouseInMethod::remove(script);
    MouseDownMethod::remove(script);
    MouseOutMethod::remove(script);
    UpdateMethod::remove(script);
}

void onMouseIn(const Entity & entity, const Engine & engine, Mem & mem)
{
    mem.opacity = ms_maxOpacity;
    adjustOpacity(entity, engine, mem, mem.opacity);
}

InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event, Mem & mem)
{
    engine.getSystem<EditorSystem>()->loadScene("editor.json", engine);
    return InputResult::Block;
}

void onMouseOut(const Entity & entity, const Engine & engine, Mem & mem)
{
    mem.opacity = ms_minOpacity;
    mem.disappearing = true;
}

void update(const Entity & entity, const Engine & engine, const float dt, Mem & mem)
{
    if (mem.disappearing)
        adjustOpacity(entity, engine, mem, -ms_transitionDelta);
}

void adjustOpacity(const Entity & entity, const Engine & engine, Mem & mem, int diff)
{
    auto colorComponent = entity.getComponent<ColorComponent>();
    if (colorComponent != nullptr)
    {
        if (colorComponent->m_color.a != mem.opacity)
        {
            Color newColor(colorComponent->m_color);
            newColor.a = std::min<uint8>(std::max<uint8>(newColor.a.asUint() + diff, ms_minOpacity), ms_maxOpacity);
            engine.addWriteTask(*colorComponent, [newColor](ColorComponent & colorComponent) { colorComponent.m_color = newColor; });

            if (newColor.a.asUint() == mem.opacity)
                mem.disappearing = false;
        }
    }
}
