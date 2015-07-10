#include "engine/Engine.h"
#include "engine/graphics/Color.h"
#include "engine/graphics/component/RenderComponent.h"

using namespace Violet;

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

extern "C" __declspec(dllexport) void onMouseIn(const Entity & entity, const Engine & engine)
{
    engine.addTask(std::make_unique<ChangeColorTask>(engine, entity.getHandle(), Color::kGreen));
}

extern "C" __declspec(dllexport) void onMouseOut(const Entity & entity, const Engine & engine)
{
    engine.addTask(std::make_unique<ChangeColorTask>(engine, entity.getHandle(), Color::kRed));
}
