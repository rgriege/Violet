#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/script/CppScript.h"
#include "violet/plugins/physics/component/PhysicsComponent.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"

#include <iostream>

using namespace Violet;

extern "C" __declspec(dllexport) void init(CppScript::Allocator & allocator)
{
}

extern "C" __declspec(dllexport) void onMouseDown(const Entity & e, Engine & engine)
{
    PhysicsComponent & physics = engine.fetch<PhysicsSystem>(e);
    physics.m_velocity.invert();
}
