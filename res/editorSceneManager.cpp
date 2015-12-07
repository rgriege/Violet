#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/script/ScriptComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/file/FileSerializerFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/update/component/UpdateComponent.h"
#include "violet/utility/FormattedString.h"
#include "violet/utility/StringUtilities.h"

#include "dialog.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script)
    {
        using namespace std::placeholders;
        KeyUpMethod::assign(script, std::bind(&Instance::onKeyUp, this, _1, _2));
        Engine::getInstance().addWriteTask(Engine::getInstance(),
            [=](Engine & engine)
            {
                m_delegateId = DialogClosedEvent::subscribe(engine, std::bind(&Instance::onDialogClosed, this, _1));
            });
    }

    virtual ~Instance() override
    {
        KeyUpMethod::remove(m_script);
        DialogClosedEvent::unsubscribe(Engine::getInstance(), m_delegateId);
    }

private:

    void onKeyUp(const Handle entityId, const unsigned char key)
    {
        if (m_dialog == None)
        {
            switch (key)
            {
                case 'o':
                    Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
                        [](ComponentManager & scene)
                        {
                            scene.load("dialog.json");
                        });
                    m_dialog = Load;
                    break;

                case 's':
                    Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
                        [](ComponentManager & scene)
                        {
                            scene.load("dialog.json");
                        });
                    m_dialog = Save;
                    break;
            }
        }
    }

    void onDialogClosed(const std::string & fileName)
    {
        if (!fileName.empty())
        {
            switch (m_dialog)
            {
                case Load:
                    Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
                        [=](ComponentManager & scene)
                        {
                            scene.load(fileName.c_str());
                        });
                    break;

                case Save:
                    save<UpdateComponent>(fileName);
                    save<WorldTransformComponent>(fileName);
                    save<ColorComponent>(fileName);
                    save<ScriptComponent>(fileName);
                    break;
            }
        }
        m_dialog = None;
    }

    template <typename ComponentType>
    void save(std::string const & fileName)
    {
        const auto & name = StringUtilities::left(fileName, '.');
        const auto & ext = StringUtilities::right(fileName, '.');
        const std::string componentFileName = name + "." + ComponentType::getStaticTag().asString() + "." + ext;
        auto pool = Engine::getInstance().getCurrentScene().getPool<ComponentType>();
        auto serializer = FileSerializerFactory::getInstance().create(componentFileName.c_str());
        if (pool && serializer)
            pool->save<ComponentType>(*serializer);
    }

private:

    enum Dialog
    {
        Load,
        Save,
        None
    };

    Dialog m_dialog = None;
    uint32 m_delegateId;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

