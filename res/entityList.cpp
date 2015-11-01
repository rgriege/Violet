#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/ui/UiList.h"
#include "violet/update/system/UpdateSystem.h"

#include <functional>

using namespace Violet;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script, const char * elementFile, uint32 elementHeight) :
        CppScript::Instance(script),
        m_list(elementFile, elementHeight)
    {
        using namespace std::placeholders;
        UpdateMethod::assign(script, std::bind(&Instance::onUpdate, this, _1, _2));
    }

    virtual ~Instance() override
    {
        UpdateMethod::remove(m_script);
        m_list.clean(m_script);
    }

private:

    void onUpdate(const Entity & entity, const Engine & engine)
    {
        const auto & root = engine.getSystem<EditorSystem>()->getSceneRoot(engine);
        if (root != nullptr)
            m_list.update(entity, engine, root->getChildren().size());
    }

private:

    UiList m_list;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script, "entityListElement.json", 20);
}


