#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/TransformComponent.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script)
    {
        using namespace std::placeholders;
        MouseDownMethod::assign(m_script, std::bind(&Instance::onMouseDown, this, _1, _2));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const Entity & entity, const InputSystem::MouseButtonEvent & event)
    {
        if (event.button == MB_Left)
        {
            createComponentMenu(entity);
            return InputResult::Block;
        }

        return InputResult::Pass;
    }

    void createComponentMenu(const Entity & entity)
    {
        const auto & engine = Engine::getInstance();
        const auto & menu = engine.getCurrentScene().getEntity(Handle(4000, 0));
        if (menu != nullptr)
        {
            if (entity.hasComponent<TransformComponent>())
            {
                engine.addWriteTask(*menu, [&](Entity & m)
                    {
                        const auto & deserializer = FileDeserializerFactory::getInstance().create("editTransform.json");
                        if (deserializer != nullptr)
                            m.addChild(*deserializer);

                        const auto & engine = Engine::getInstance();
                        engine.addWriteTask(engine.getCurrentScene(), [&](Scene & scene)
                            {
                                EntitySelectedEvent::emit(scene, entity);
                            });
                    });
            }
        }
    }
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

