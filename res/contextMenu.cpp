#include "violet/Engine.h"
#include "violet/input/InputResult.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/TransformComponent.h"

#include <functional>

using namespace Violet;

struct Mem : public CppScript::Memory
{
    Mem() : menu(Handle::ms_invalid) {}
    virtual ~Mem() override = default;
    Handle menu;
};

InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event, Mem & mem);

bool activeMenu(Mem & mem);
void createMenu(Mem & mem, const Engine & engine, const Vec2f & position);
void removeMenu(Mem & mem, const Engine & engine);

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    auto m = std::make_unique<Mem>();

    using namespace std::placeholders;
    MouseDownMethod::assign(script, std::bind(onMouseDown, _1, _2, _3, std::ref(*m)));

    mem = std::move(m);
}

VIOLET_SCRIPT_EXPORT void clean(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    MouseDownMethod::remove(script);
}

InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event, Mem & mem)
{
    if (event.button == MB_Right)
    {
        if (activeMenu(mem))
        {
            removeMenu(mem, engine);
            createMenu(mem, engine, event.position);
        }
        else
            createMenu(mem, engine, event.position);
    }
    else if (activeMenu(mem))
        removeMenu(mem, engine);
    else
        return InputResult::Pass;

    return InputResult::Block;
}

bool activeMenu(Mem & mem)
{
    return mem.menu.isValid();
}

void createMenu(Mem & mem, const Engine & engine, const Vec2f & position)
{
    engine.addWriteTask(engine.getCurrentScene(), [position, &mem](Scene & scene)
        {
            auto deserializer = FileDeserializerFactory::getInstance().create("block.json");
            if (deserializer != nullptr && *deserializer)
            {
                Entity & menu = scene.getRoot().addChild(*deserializer);
                auto transformComponent = menu.getComponent<TransformComponent>();
                if (transformComponent != nullptr)
                    transformComponent->m_position = position;
                mem.menu = menu.getHandle();
            }
        });
}

void removeMenu(Mem & mem, const Engine & engine)
{
    auto & menu = mem.menu;
    engine.addWriteTask(engine.getCurrentScene(), [menu](Scene & scene) { scene.getRoot().removeChild(menu); });
    menu = Handle::ms_invalid;
}

