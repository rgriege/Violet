#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/ui/UiList.h"
#include "violet/utility/FormattedString.h"

#include "editor/EditorSystem.h"

#include <functional>
#include <iostream>

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
        MouseDownMethod::assign(m_script, std::bind(&Instance::onMouseDown, this, _1, _2, _3));
    }

    virtual ~Instance() override
    {
        AssignIndexMethod::remove(m_script);
        MouseDownMethod::remove(m_script);
    }

private:

    void onAssignIndex(const Entity & entity, const Engine & engine, const uint32 index)
    {
        m_index = index;
        engine.addWriteTask(*entity.getComponent<TextComponent>(),
            [=](TextComponent & tc) { tc.m_text = "Entity"; });
    }

    InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            const auto & root = engine.getSystem<EditorSystem>()->getSceneRoot(engine);
            if (root != nullptr)
            {
                const auto & children = root->getChildren();
                if (m_index < children.size())
                {
                    const auto & child = *children[m_index];
                    createComponentMenu(engine, child);
                    engine.addWriteTask(entity, [&](Entity & entity)
                        {
                            EntitySelectedEvent::emit(entity.getScene(), child, engine);
                        });
                }
            }
            return InputResult::Block;
        }

        return InputResult::Pass;
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

