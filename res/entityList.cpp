#include "editor/EditorSystem.h"
#include "engine/Engine.h"
#include "engine/script/cpp/CppScript.h"
#include "engine/ui/UiList.h"
#include "engine/update/system/UpdateSystem.h"

#include <functional>

using namespace Violet;

struct Mem : public CppScript::Memory
{
    Mem(Script & script, const char * elementFile) : list(script, elementFile) {}
    virtual ~Mem() override = default;
    UiList list;
};

void onUpdate(const Entity & entity, const Engine & engine, Mem & mem);

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    auto m = std::make_unique<Mem>(script, "entityListElement.json");

    using namespace std::placeholders;
    UpdateMethod::assign(script, std::bind(onUpdate, _1, _2, std::ref(*m)));

    mem = std::move(m);
}

VIOLET_SCRIPT_EXPORT void clean(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    UpdateMethod::remove(script);

    dynamic_cast<Mem &>(*mem).list.clean(script);
}

void onUpdate(const Entity & entity, const Engine & engine, Mem & mem)
{
    const auto & scene = engine.getSystem<EditorSystem>()->getScene();
    if (scene != nullptr)
        mem.list.update(entity, engine, scene->getRoot().getChildren().size());
}

