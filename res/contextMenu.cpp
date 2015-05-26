#include "engine/Engine.h"
#include "engine/script/component/CppScriptComponent.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/window/WindowSystem.h"
#include "engine/input/InputResult.h"

#include <vector>

using namespace Violet;

struct Mem
{
    std::vector<Entity> buttons;
};

bool activeMenu(Mem & mem);
void createMenu(Mem & mem, Engine & engine, Entity entity, int x, int y);
void removeMenu(Mem & mem, Engine & engine);

extern "C" __declspec(dllexport) void init(CppScriptComponent::Allocator & allocator)
{
    Mem * mem = allocator.allocate<Mem>();
    // mem->menu = Entity::INVALID;
}

extern "C" __declspec(dllexport) InputResult onMouseDown(const Entity e, Engine & engine, const WindowSystem::MouseEvent & event, Mem * mem)
{
    if (event.button == MB_Right)
    {
        if (activeMenu(*mem))
        {
            removeMenu(*mem, engine);
            createMenu(*mem, engine, e, event.x, event.y);
        }
        else
            createMenu(*mem, engine, e, event.x, event.y);
    }
    else if (activeMenu(*mem))
        removeMenu(*mem, engine);
    else
        return InputResult::Pass;

    return InputResult::Block;
}

bool activeMenu(Mem & mem)
{
    return !mem.buttons.empty();
    // return mem.menu != Entity::INVALID;
}

void createMenu(Mem & mem, Engine & engine, const Entity entity, int x, int y)
{
    auto deserializer = FileDeserializerFactory::getInstance().create("block.json");
    if (deserializer != nullptr)
    {
        // mem.menu = engine.getCurrentScene().createEntity(*deserializer);
        while (*deserializer)
        {
            mem.buttons.emplace_back(engine.getCurrentScene().createEntity(*deserializer));
            auto * buttonTc = engine.getCurrentScene().getComponent<TransformComponent>(mem.buttons.back());
            if (buttonTc != nullptr)
                buttonTc->m_position += Vec2f(x, y);
        }
    }
}

void removeMenu(Mem & mem, Engine & engine)
{
    // engine.getCurrentScene().destroyEntity(mem.menu);
    // mem.menu = Entity::INVALID;
    for (auto button : mem.buttons)
        engine.getCurrentScene().destroyEntity(button);
    mem.buttons.clear();
}

