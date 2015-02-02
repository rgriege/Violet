#include "violet/core/AlterContext.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/script/CppScript.h"
#include "violet/core/transform/TransformComponent.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/plugins/physics/component/PhysicsComponent.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"

#include <iostream>

using namespace Violet;

struct Mem
{
    bool keyUpPressed, keyDownPressed;
};

extern "C" __declspec(dllexport) void init(CppScript::Allocator & allocator)
{
    Mem * mem = static_cast<Mem*>(allocator.allocate<Mem>());
    mem->keyUpPressed = mem->keyDownPressed = false;
}

extern "C" __declspec(dllexport) void onKeyDown(const Entity & e, AlterContext & context, const unsigned char & key, Mem * mem)
{
    if (key == 119)
        mem->keyUpPressed = true;
    else if (key == 115)
        mem->keyDownPressed = true;
}

extern "C" __declspec(dllexport) void onKeyUp(const Entity & e, AlterContext & context, const unsigned char & key, Mem * mem)
{
    if (key == 119)
        mem->keyUpPressed = false; 
    else if (key == 115)
        mem->keyDownPressed = false;
}

extern "C" __declspec(dllexport) void update(const Entity & e, AlterContext & context, Mem * mem)
{
    if (mem->keyUpPressed ^ mem->keyDownPressed)
    {
        TransformComponent & transform = context.fetch<TransformSystem>(e);
        transform.m_position += Vec2f(0, mem->keyUpPressed ? 5 : -5);
    }

    PhysicsComponent & physics = context.fetch<PhysicsSystem>(e);
    physics.m_velocity.x = 0;
}
