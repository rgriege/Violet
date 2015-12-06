#include "violet/Engine.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/log/Log.h"
#include "violet/script/ScriptComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/file/FileSerializerFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/update/component/UpdateComponent.h"
#include "violet/update/system/UpdateSystem.h"
#include "violet/utility/FormattedString.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_count(0)
    {
        using namespace std::placeholders;
        UpdateMethod::assign(script, std::bind(&Instance::onUpdate, this, _1, _2));
        KeyUpMethod::assign(script, std::bind(&Instance::onKeyUp, this, _1, _2));
    }

    virtual ~Instance() override
    {
        UpdateMethod::remove(m_script);
        KeyUpMethod::remove(m_script);
    }

private:

    void onUpdate(const Handle entityId, const float dt)
    {
        if (!m_paused)
        {
            const uint32 delta = 10;
            m_count += delta;
            Log::log(FormattedString<32>().sprintf("square count: %d", m_count));
            for (uint32 i = 0; i < delta; ++i)
            {
                Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene(),
                    [](Scene & scene)
                    {
                        scene.loadEntity("square.json");
                    });
            }
        }
    }

    void onKeyUp(const Handle entityId, const unsigned char key)
    {
        if (key == 'p')
            m_paused = !m_paused;
        else if (key == 's' && m_paused)
        {
            save<UpdateComponent>("save.updt.json");
            save<WorldTransformComponent>("save.wtfm.json");
            save<ColorComponent>("save.colr.json");
            save<ScriptComponent>("save.scpt.json");
        }
    }

    template <typename ComponentType>
    void save(const char * filename)
    {
        auto pool = Engine::getInstance().getCurrentScene().getComponentManager().getPool<ComponentType>();
        auto serializer = FileSerializerFactory::getInstance().create(filename);
        if (pool && serializer)
            pool->save<ComponentType>(*serializer);
    }

private:

    uint32 m_count;
    bool m_paused = false;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

