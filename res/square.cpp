#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/log/Log.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/Random.h"
#include "violet/window/WindowSystem.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_speed(100.f, 0)
    {
        using namespace std::placeholders;
        UpdateMethod::assign(script, std::bind(&Instance::onUpdate, this, _1, _2));

        m_speed.rotate(Random::ms_generator.generate0to1() * 3.141 * 2);
    }

    virtual ~Instance() override
    {
        UpdateMethod::remove(m_script);
    }

private:

    void onUpdate(const Handle entityId, const float dt)
    {
        auto transformC = Engine::getInstance().getCurrentScene().getComponent<WorldTransformComponent>(entityId);
        if (transformC != nullptr)
        {
            const auto & window = Engine::getInstance().getSystem<WindowSystem>();
            const float dx = m_speed.x * dt;
            const float dy = m_speed.y * dt;
            Engine::getInstance().addWriteTask(*transformC,
                [=](WorldTransformComponent & transformC)
                {
                    transformC.m_transform[0][2] += dx;
                    transformC.m_transform[1][2] += dy;
                });
            if (std::abs(transformC->m_transform[0][2] + dx) > window->getWidth() / 2)
                m_speed.x = -m_speed.x;
            if (std::abs(transformC->m_transform[1][2] + dy) > window->getHeight() / 2)
                m_speed.y = -m_speed.y;
        }
    }

private:

    Vec2f m_speed;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

