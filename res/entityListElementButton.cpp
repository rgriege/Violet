#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/ScriptUtilities.h"
#include "violet/script/cpp/CppScript.h"

#include "entityListElementMethods.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script)
    {
        using namespace std::placeholders;
        MouseDownMethod::assign(m_script, std::bind(&Instance::onMouseDown, this, _1, _2, _3));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            ScriptUtilities::runOnAncestor<DescribeMethod>(entity, engine);
            return InputResult::Block;
        }

        return InputResult::Pass;
    }
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

