#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/ui/UiList.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

class Instance : public CppScript::Instance
{
public:
    Instance(CppScript & script) :
        CppScript::Instance(script)
    {
        AssignIndexMethod::assign(m_script, &Instance::onAssignIndex);
        MouseDownMethod::assign(m_script, &Instance::onMouseDown);
    }

    virtual ~Instance() override
    {
        AssignIndexMethod::remove(m_script);
        MouseDownMethod::remove(m_script);
    }

private:

    static void onAssignIndex(const Entity & entity, const Engine & engine, const uint32 index)
    {
        engine.addWriteTask(*entity.getComponent<TextComponent>(),
            [=](TextComponent & tc) { tc.m_text = "Entity"; });
    }

    static InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
    {
        return InputResult::Block;
    }
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

