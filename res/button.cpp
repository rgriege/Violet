#include "engine/Engine.h"
#include "engine/input/system/InputSystem.h"
#include "engine/graphics/Color.h"
#include "engine/graphics/component/ColorComponent.h"
#include "engine/script/cpp/CppScriptComponent.h"
#include "engine/update/system/UpdateSystem.h"

#include <algorithm>
#include <functional>

using namespace Violet;

struct Mem
{
    uint8 opacity;
    bool disappearing;
};

class ChangeColorTask : public Engine::Task
{
public:
    ChangeColorTask(const Engine & engine, const Handle handle, const Color & color) :
        Engine::Task(engine),
        m_entityHandle(handle),
        m_color(color)
    {
    }

    virtual void execute() const override
    {
        const auto entity = m_engine.getCurrentScene().getEntity(m_entityHandle);
        if (entity != nullptr)
        {
            auto colorComponent = entity->getComponent<ColorComponent>();
            if (colorComponent != nullptr)
                colorComponent->m_color = m_color;
        }
    }

private:

    const Handle m_entityHandle;
    const Color m_color;
};

void onMouseIn(const Entity & entity, const Engine & engine, Mem * mem);

void onMouseOut(const Entity & entity, const Engine & engine, Mem * mem);

void update(const Entity & entity, const Engine & engine, float dt, Mem * mem);

VIOLET_SCRIPT_EXPORT void init(CppScriptComponent::Allocator & allocator, const Entity & entity)
{
    Mem * mem = allocator.allocate<Mem>();
    mem->opacity = 0xCC;
    mem->disappearing = false;

    using namespace std::placeholders;
    MouseInMethod::assign(entity, std::bind(onMouseIn, _1, _2, mem));
    MouseOutMethod::assign(entity, std::bind(onMouseOut, _1, _2, mem));
    UpdateMethod::assign(entity, std::bind(update, _1, _2, _3, mem));
}

VIOLET_SCRIPT_EXPORT void clean(const Entity & entity)
{
    MouseInMethod::remove(entity);
    MouseOutMethod::remove(entity);
    UpdateMethod::remove(entity);
}

void onMouseIn(const Entity & entity, const Engine & engine, Mem * mem)
{
    mem->opacity = 0xCC;
    mem->disappearing = false;
}

void onMouseOut(const Entity & entity, const Engine & engine, Mem * mem)
{
    mem->opacity = 0x33;
    mem->disappearing = true;
}

void update(const Entity & entity, const Engine & engine, const float dt, Mem * mem)
{
    auto colorComponent = entity.getComponent<ColorComponent>();
    if (colorComponent != nullptr)
    {
        if (colorComponent->m_color.a != mem->opacity)
        {
            Color newColor(colorComponent->m_color);
            if (mem->disappearing)
                newColor.a = std::max<uint8>(newColor.a.asUint() - 5u, mem->opacity);
            else
                newColor.a = std::min<uint8>(newColor.a.asUint() + 5u, mem->opacity);
            engine.addTask(std::make_unique<ChangeColorTask>(engine, entity.getHandle(), newColor));
        }
    }
}
