#include "violet/core/AlterContext.h"
#include "violet/core/entity/Entity.h"
#include "violet/plugins/physics/component/PhysicsComponent.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"

#include <iostream>

using namespace Violet;

extern "C" __declspec(dllexport) void onMouse(const Entity & e, AlterContext & context)
{
    std::cout << "entity " << e.m_id << " clicked!" << std::endl;
    PhysicsComponent & physics = context.fetch<PhysicsSystem>(e);
    physics.m_velocity.invert();
}
