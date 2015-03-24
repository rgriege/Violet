#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/script/CppScript.h"
#include "violet/core/transform/TransformComponent.h"
#include "violet/plugins/physics/component/PhysicsComponent.h"

#include <iostream>

#define WASD

using namespace Violet;

struct Mem
{
    bool keyUpPressed, keyDownPressed;
};

extern "C" __declspec(dllexport) void init(CppScript::Allocator & allocator)
{
#ifdef WASD
    std::cout << "wasd loaded" << std::endl;
#else
    std::cout << "ijkl loaded" << std::endl;
#endif

    Mem * mem = static_cast<Mem*>(allocator.allocate<Mem>());
    mem->keyUpPressed = mem->keyDownPressed = false;
}

extern "C" __declspec(dllexport) void onKeyDown(const Entity & e, Engine & engine, const unsigned char & key, Mem * mem)
{
#ifdef WASD
    if (key == 119)
        mem->keyUpPressed = true;
    else if (key == 115)
        mem->keyDownPressed = true;
#else
    if (key == 105)
        mem->keyUpPressed = true;
    else if (key == 107)
        mem->keyDownPressed = true;
#endif
}

extern "C" __declspec(dllexport) void onKeyUp(const Entity & e, Engine & engine, const unsigned char & key, Mem * mem)
{
#ifdef WASD
    if (key == 119)
        mem->keyUpPressed = false; 
    else if (key == 115)
        mem->keyDownPressed = false;
#else
    if (key == 105)
        mem->keyUpPressed = false; 
    else if (key == 107)
        mem->keyDownPressed = false;
#endif
}

extern "C" __declspec(dllexport) void update(const Entity & e, Engine & engine, Mem * mem)
{
    if (mem->keyUpPressed ^ mem->keyDownPressed)
    {
        TransformComponent & transform = engine.fetch<TransformComponent>(e);
        transform.m_position += Vec2f(0, mem->keyUpPressed ? 5 : -5);
    }

    PhysicsComponent & physics = engine.fetch<PhysicsComponent>(e);
    physics.m_velocity.x = 0;
}
