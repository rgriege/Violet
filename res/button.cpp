#include "engine/Engine.h"
#include "engine/script/component/CppScriptComponent.h"
#include "engine/graphics/Color.h"
#include "engine/graphics/component/RenderComponent.h"

#include <algorithm>

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
        Entity & entity = m_engine.getCurrentScene().getEntity(m_entityHandle);
        if (entity.isValid())
        {
            auto & renderComponent = entity.getComponent<RenderComponent>();
            if (renderComponent != nullptr)
                renderComponent->m_color = m_color;
        }
    }

private:

    const Handle m_entityHandle;
    const Color m_color;
};

extern "C" __declspec(dllexport) void init(CppScriptComponent::Allocator & allocator)
{
    Mem * mem = allocator.allocate<Mem>();
    mem->opacity = 0xCC;
    mem->disappearing = false;
}

extern "C" __declspec(dllexport) void onMouseIn(const Entity & entity, const Engine & engine, Mem * mem)
{
    mem->opacity = 0xCC;
    mem->disappearing = false;
}

extern "C" __declspec(dllexport) void onMouseOut(const Entity & entity, const Engine & engine, Mem * mem)
{
    mem->opacity = 0x33;
    mem->disappearing = true;
}

extern "C" __declspec(dllexport) void update(const Entity & entity, const Engine & engine, Mem * mem)
{
    auto & renderComponent = entity.getComponent<RenderComponent>();
    if (renderComponent != nullptr)
    {
        if (renderComponent->m_color.a != mem->opacity)
        {
            Color newColor(renderComponent->m_color);
            if (mem->disappearing)
                newColor.a = std::max<uint8>(newColor.a.asUint() - 5u, mem->opacity);
            else
                newColor.a = std::min<uint8>(newColor.a.asUint() + 5u, mem->opacity);
            engine.addTask(std::make_unique<ChangeColorTask>(engine, entity.getHandle(), newColor));
        }
    }
}
