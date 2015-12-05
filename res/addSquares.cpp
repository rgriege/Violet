#include "violet/Engine.h"
#include "violet/log/Log.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
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
    }

    virtual ~Instance() override
    {
        UpdateMethod::remove(m_script);
    }

private:

    void onUpdate(const Entity & entity, const float dt)
    {
        const uint32 delta = 10;
        m_count += delta;
        Log::log(FormattedString<32>().sprintf("square count: %d", m_count));
        for (uint32 i = 0; i < delta; ++i)
        {
            Engine::getInstance().addWriteTask(Engine::getInstance().getCurrentScene().getRoot(),
                [](Entity & root)
                {
                    auto deserializer = FileDeserializerFactory::getInstance().create("square.json");
                    if (deserializer != nullptr)
                        root.addChild(*deserializer);
                }, Engine::Thread::Window);
        }
    }

private:

    uint32 m_count;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}

