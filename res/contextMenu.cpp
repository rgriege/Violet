#include "engine/Engine.h"
#include "engine/script/component/CppScriptComponent.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/input/InputResult.h"
#include "engine/input/system/InputSystem.h"

#include <vector>

using namespace Violet;

struct Mem
{
    Handle menu;
};

class AddMenuTask : public Engine::Task
{
public:

    AddMenuTask(const Engine & engine, const Vec2f & position, Mem & mem) :
        Engine::Task(engine),
        m_position(position),
        m_mem(mem)
    {
    }

    virtual void execute() const override
    {
        auto deserializer = FileDeserializerFactory::getInstance().create("block.json");
        if (deserializer != nullptr && *deserializer)
        {
            Entity & menu = m_engine.getCurrentScene().createEntity(m_engine.getCurrentScene().getRoot(), *deserializer);
            auto & transformComponent = menu.getComponent<TransformComponent>();
            if (transformComponent != nullptr)
                transformComponent->m_position = m_position;
            m_mem.menu = menu.getHandle();
        }
    }

private:

    const Vec2f m_position;
    Mem & m_mem;
};

class RemoveMenuTask : public Engine::Task
{
public:

    RemoveMenuTask(const Engine & engine, Mem & mem) :
        Engine::Task(engine),
        m_mem(mem)
    {
    }

    virtual void execute() const override
    {
        m_engine.getCurrentScene().destroyEntity(m_mem.menu);
        m_mem.menu = Handle::ms_invalid;
    }

private:

    Mem & m_mem;
};

bool activeMenu(Mem & mem);
void createMenu(Mem & mem, const Engine & engine, const Vec2f & position);
void removeMenu(Mem & mem, const Engine & engine);

extern "C" __declspec(dllexport) void init(CppScriptComponent::Allocator & allocator)
{
    Mem * mem = allocator.allocate<Mem>();
    mem->menu = Handle::ms_invalid;
}

extern "C" __declspec(dllexport) InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event, Mem * mem)
{
    if (event.button == MB_Right)
    {
        if (activeMenu(*mem))
        {
            removeMenu(*mem, engine);
            createMenu(*mem, engine, event.position);
        }
        else
            createMenu(*mem, engine, event.position);
    }
    else if (activeMenu(*mem))
        removeMenu(*mem, engine);
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
    engine.addTask(std::make_unique<AddMenuTask>(engine, position, mem));
}

void removeMenu(Mem & mem, const Engine & engine)
{
    engine.addTask(std::make_unique<RemoveMenuTask>(engine, mem));
}

