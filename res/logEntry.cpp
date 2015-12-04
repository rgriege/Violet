#include "violet/Engine.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/log/Log.h"
#include "violet/script/ScriptUtilities.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/ui/UiList.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/utility/FormattedString.h"

#include "log.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_index(0)
    {
        using namespace std::placeholders;
        AssignIndexMethod::assign(script, std::bind(&Instance::onAssignIndex, this, _1, _2, _3));
        UpdateMethod::assign(script, std::bind(&Instance::onUpdate, this, _1, _2));
    }

    virtual ~Instance() override
    {
        AssignIndexMethod::remove(m_script);
        UpdateMethod::remove(m_script);
    }

private:

    void onAssignIndex(const Entity & entity, const Engine & engine, const uint32 index)
    {
        m_index = index;
    }

    void onUpdate(const Entity & entity, const Engine & engine)
    {
        uint32 index = m_index;
        const auto & text = ScriptUtilities::runOnAncestor<GetLogEntryMethod>(entity, std::move(index));

        const auto & textC = entity.getComponent<TextComponent>();
        engine.addWriteTask(*textC,
            [text](TextComponent & textComponent)
            {
                textComponent.m_text = text;
            });
    }

private:

    uint32 m_index;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

