#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/ui/UiList.h"
#include "violet/update/system/UpdateSystem.h"

#include <functional>

using namespace Violet;

struct Mem : public CppScript::Memory
{
    Mem(const char * elementFile) : list(elementFile) {}
    virtual ~Mem() override = default;
    UiList list;
};

void onUpdate(const Entity & entity, const Engine & engine, Mem & mem);

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    auto m = std::make_unique<Mem>("entityListElement.json");

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

