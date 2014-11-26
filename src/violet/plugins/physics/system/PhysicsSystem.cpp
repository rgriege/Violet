#include "violet/plugins/physics/system/PhysicsSystem.h"

#include "violet/core/transform/TransformSystem.h"
#include "violet/plugins/physics/collision/Intersection.h"

#include <vector>
#include <iostream>
#include <cstdio>

using namespace Violet;

namespace PhysicsSystemNamespace
{
	const char * const ms_componentLabel = "phys";

	PhysicsSystem * ms_physicsSystem;

	void updateEntity(TransformComponent & transform, PhysicsComponent & physics, float dt);
	void resolveCollisionForEntity(TransformComponent & transform, PhysicsComponent & physics, Intersection & intersection);
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
	if (!ms_physicsSystem->m_gravity.isZero())
		for (auto & component : ms_physicsSystem->m_components)
			component.m_force += ms_physicsSystem->m_gravity - component.m_velocity * ms_physicsSystem->m_drag;

	for (auto & component : ms_physicsSystem->m_components)
	{
		TransformComponent & transform = TransformSystem::fetch(component.m_entity);
		updateEntity(transform, component, dt);
	}

	for (uint32 i = 0, len = ms_physicsSystem->m_components.size(); i < len; ++i)
	{
		PhysicsComponent & physics1 = ms_physicsSystem->m_components[i];
		TransformComponent & transform1 = TransformSystem::fetch(physics1.m_entity);
		for (uint32 j = i + 1; j < len; ++j)
		{
			PhysicsComponent & physics2 = ms_physicsSystem->m_components[j];
			TransformComponent & transform2 = TransformSystem::fetch(physics2.m_entity);
			Intersection intersection(RigidBody(transform1, physics1), RigidBody(transform2, physics2), dt);
			if (intersection.exists())
			{
				resolveCollisionForEntity(transform1, physics1, intersection);
				resolveCollisionForEntity(transform2, physics2, intersection);
			}
		}
	}
}

void PhysicsSystem::create(Entity & entity, Deserializer & deserializer)
{
	deserializer.enterSegment(ms_componentLabel);
	ms_physicsSystem->m_entityComponentMap.emplace(entity.id, ms_physicsSystem->m_components.size());
	ms_physicsSystem->m_components.emplace_back(entity, deserializer);
	deserializer.leaveSegment();
}

PhysicsComponent & PhysicsSystem::fetch(const Entity & entity)
{
	return ms_physicsSystem->m_components[ms_physicsSystem->m_entityComponentMap[entity.id]];
}

void PhysicsSystemNamespace::updateEntity(TransformComponent & transform, PhysicsComponent & physics, const float dt)
{
	Vec2f const acceleration = physics.m_force / physics.m_mass;
	transform.m_position += acceleration * dt * dt / 2.f + physics.m_velocity * dt;
	physics.m_velocity += acceleration * dt;
	physics.m_force.zero();

	float const angularAcceleration = physics.m_torque / physics.m_momentOfInertia;
	transform.m_rotation += angularAcceleration * dt * dt + physics.m_angularVelocity * dt;
	physics.m_angularVelocity += angularAcceleration * dt;
	physics.m_torque = 0;
}

void PhysicsSystemNamespace::resolveCollisionForEntity(TransformComponent & transform, PhysicsComponent & physics, Intersection & intersection)
{
	updateEntity(transform, physics, intersection.getTimeOfImpact());
	Vec2f impulse = intersection.getIntersectionAxis() * intersection.getImpulseScalar();
	Vec2f const location = intersection.getImpactLocation() - transform.m_position;
	if (impulse.dot(location) > 0)
		impulse.invert();

	physics.m_angularVelocity -= location.cross(impulse) / physics.m_momentOfInertia;
	impulse /= physics.m_mass;
	physics.m_velocity += impulse;
}