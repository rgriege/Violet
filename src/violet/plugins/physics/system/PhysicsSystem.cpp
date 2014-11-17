#include "violet/plugins/physics/system/PhysicsSystem.h"

#include <vector>
#include <iostream>
#include <cstdio>

using namespace Violet;

namespace PhysicsSystemNamespace
{
	const char * const ms_componentLabel = "phys";

	PhysicsSystem * ms_physicsSystem;
}

using namespace PhysicsSystemNamespace;

bool PhysicsSystem::init(Settings & settings)
{
	ms_physicsSystem = new PhysicsSystem();
	ms_physicsSystem->m_drag = settings.drag;
	ms_physicsSystem->m_gravity = settings.gravity;
	return true;
}

void PhysicsSystem::update(const float dt)
{
	for (auto & component : ms_physicsSystem->m_components)
		component.m_shape->applyForce(ms_physicsSystem->m_gravity);



	for (auto & component : ms_physicsSystem->m_components)
		component.m_shape->update(dt);
}

void PhysicsSystem::create(Entity & entity, Deserializer & deserializer)
{
	deserializer.enterSegment(ms_componentLabel);
	ms_physicsSystem->m_components.emplace_back(deserializer);
	ms_physicsSystem->m_entityComponentMap.emplace(entity.id, ms_physicsSystem->m_components.size());
	deserializer.leaveSegment();
}

void PhysicsSystem::moveBodies(float time) {
	/*for (int i = 0; i < actives.size(); i++) {
		if (drag != 0)
			actives[i]->apply_force(-drag*actives[i]->velocity);
		actives[i]->move(time);
	}*/
}

void PhysicsSystem::detectCollisions() {
	/*for (int i = 0; i < actives.size(); i++) {
		for (int j = 0; j < stills.size(); j++) {
			CollisionDP col(*actives[i], *stills[j]);
			if(col.exists())
				actives[i]->apply_impulse(col.get_impulse().inverse());
		}
		for (int j = 0; j < actives.size(); j++) {
			if (i != j) {
				CollisionDD col(*actives[i], *actives[j]);
				if (col.exists()) {
					actives[i]->apply_impulse(col.get_impulse().inverse());
					actives[j]->apply_impulse(col.get_impulse());
				}
			}
		}
	}*/
}

void PhysicsSystem::applyGravity()
{

}