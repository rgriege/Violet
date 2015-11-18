#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/ScriptUtilities.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/transform/Transform.h"
#include "violet/transform/component/TransformComponent.h"

#include "entityListElementMethods.h"

#include <algorithm>
#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_active(false)
    {
        using namespace std::placeholders;
        MouseDownMethod::assign(script, std::bind(&Instance::onMouseDown, this, _1, _2, _3));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
    {
        auto transformC = entity.getComponent<TransformComponent>();
        if (transformC != nullptr)
        {
            const float radians = m_active ? 3.14/2.f : -3.14/2.f;
            engine.addWriteTask(*transformC,
                [radians](TransformComponent & transformComponent)
                {
                    Transform::rotate(transformComponent.m_transform, radians);
                });
        }

        if (m_active)
            ScriptUtilities::runOnAncestor<ShrinkMethod>(entity);
        else
            ScriptUtilities::runOnAncestor<ExpandMethod>(entity);

        m_active = !m_active;

        return InputResult::Block;
    }

private:

    bool m_active;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

