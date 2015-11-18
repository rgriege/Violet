#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/log/Log.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/Transform.h"
#include "violet/transform/component/TransformComponent.h"
#include "violet/ui/UiList.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/utility/FormattedString.h"

#include "entityListElementMethods.h"

#include <algorithm>
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
        AssignIndexMethod::assign(m_script, std::bind(&Instance::onAssignIndex, this, _1, _2, _3));
        DescribeMethod::assign(script, std::bind(&Instance::onDescribe, this, _1));
        ExpandMethod::assign(script, std::bind(&Instance::onExpand, this));
        ShrinkMethod::assign(script, std::bind(&Instance::onShrink, this));
    }

    virtual ~Instance() override
    {
        AssignIndexMethod::remove(m_script);
        DescribeMethod::remove(m_script);
        ExpandMethod::remove(m_script);
        ShrinkMethod::remove(m_script);
    }

private:

    void onAssignIndex(const Entity & entity, const Engine & engine, const uint32 index)
    {
        m_index = index;
    }

    void onDescribe(const Engine & engine)
    {
        Log::log("describing");
        const auto & root = engine.getSystem<EditorSystem>()->getSceneRoot(engine);
        if (root != nullptr)
        {
            const auto & children = root->getChildren();
            if (m_index < children.size())
            {
                const auto & child = *children[m_index];
                createComponentMenu(engine, child);
                engine.addWriteTask(engine.getCurrentScene(), [&](Scene & scene)
                    {
                        EntitySelectedEvent::emit(scene, child, engine);
                    });
            }
        }
    }

    void onExpand()
    {
        Log::log("expanding");
    }

    void onShrink()
    {
        Log::log("shrinking");
    }

    void createComponentMenu(const Engine & engine, const Entity & entity)
    {
        const auto & menu = engine.getCurrentScene().getEntity(Handle(4000, 0));
        if (menu != nullptr)
        {
            if (entity.hasComponent<TransformComponent>())
            {
                engine.addWriteTask(*menu, [](Entity & m)
                    {
                        const auto & deserializer = FileDeserializerFactory::getInstance().create("editTransform.json");
                        if (deserializer != nullptr)
                            m.addChild(*deserializer);
                    });
            }
        }
    }

private:

    uint32 m_index;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

