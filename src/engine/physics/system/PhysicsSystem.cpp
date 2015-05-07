#include "engine/physics/system/PhysicsSystem.h"

#include "engine/Engine.h"
#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/physics/collision/Intersection.h"

#include <iostream>
#include <cstdio>

using namespace Violet;

namespace PhysicsSystemNamespace
{
	float const ms_minimumSpeed = 0.01f;
	float const ms_restitution = 1.f;

	void updateEntity(TransformComponent & transform, PhysicsComponent & physics, float dt);
	void resolveCollisionForEntity(TransformComponent & transform, PhysicsComponent & physics, Intersection & intersection, float impulseMagnitude);
}

using namespace PhysicsSystemNamespace;

const char * PhysicsSystem::getStaticLabel()
{
	return "phys";
}

void PhysicsSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &PhysicsSystem::init);
}

std::unique_ptr<System> PhysicsSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());
	float const drag = settingsSegment->getFloat("drag");
	auto system = new PhysicsSystem(drag, Vec2f(*settingsSegment->enterSegment("gravity")));
	return std::unique_ptr<System>(system);
}

PhysicsSystem::PhysicsSystem(PhysicsSystem && other) :
	System(std::move(other)),
	m_drag(other.m_drag),
	m_gravity(other.m_gravity)
{
}

void PhysicsSystem::update(const float dt, Engine & engine)
{
	auto view = engine.getCurrentScene().getView<PhysicsComponent>();

	if (!m_gravity.isZero() || m_drag != 0)
	{
		for (auto & componentSet : view)
		{
			PhysicsComponent & component = get<PhysicsComponent&>(componentSet);
			component.m_force += m_gravity - component.m_velocity * m_drag;
		}
	}

	for (auto & componentSet : view)
	{
		PhysicsComponent & component = get<PhysicsComponent&>(componentSet);
		TransformComponent * transform = engine.getCurrentScene().getComponent<TransformComponent>(component.getEntity());
		updateEntity(*transform, component, dt);
	}

	for (auto it = view.begin(), end = view.end(); it != end; ++it)
	{
		PhysicsComponent & physics1 = get<PhysicsComponent&>(*it);
		TransformComponent * transform1 = engine.getCurrentScene().getComponent<TransformComponent>(physics1.getEntity());
		auto it2 = it;
		++it2;
		for (; it2 != end; ++it2)
		{
			PhysicsComponent & physics2 = get<PhysicsComponent&>(*it2);
			TransformComponent * transform2 = engine.getCurrentScene().getComponent<TransformComponent>(physics2.getEntity());
			Intersection intersection(RigidBody(*transform1, physics1), RigidBody(*transform2, physics2), dt);
			if (intersection.exists())
			{
				//printf("collision!\n");
				float const impulseMagnitude = (-(1 + ms_restitution) * (physics2.m_velocity - physics1.m_velocity).dot(intersection.getIntersectionAxis())) /
					(1 / physics1.m_mass + 1 / physics2.m_mass);
				resolveCollisionForEntity(*transform1, physics1, intersection, impulseMagnitude);
				resolveCollisionForEntity(*transform2, physics2, intersection, impulseMagnitude);
			}
		}
	}
}

PhysicsSystem::PhysicsSystem(float drag, Vec2f gravity) :
	System(getStaticLabel()),
	m_drag(drag),
	m_gravity(std::move(gravity))
{
}

void PhysicsSystemNamespace::updateEntity(TransformComponent & transform, PhysicsComponent & physics, const float dt)
{
	float const speed = physics.m_velocity.magSquared();
	if (speed > ms_minimumSpeed || !physics.m_force.isZero())
	{
		Vec2f const acceleration = physics.m_force / physics.m_mass;
		transform.m_position += acceleration * dt * dt / 2.f + physics.m_velocity * dt;
		physics.m_velocity += acceleration * dt;
		physics.m_force.zero();
	}
	else if (speed != 0)
		physics.m_velocity.zero();

	float const angularAcceleration = physics.m_torque / physics.m_momentOfInertia;
	transform.m_rotation += angularAcceleration * dt * dt + physics.m_angularVelocity * dt;
	physics.m_angularVelocity += angularAcceleration * dt;
	physics.m_torque = 0;
}

void PhysicsSystemNamespace::resolveCollisionForEntity(TransformComponent & transform, PhysicsComponent & physics, Intersection & intersection, float impulseMagnitude)
{
	Vec2f impulse = intersection.getIntersectionAxis() * impulseMagnitude / physics.m_mass;
	Vec2f const location = intersection.getImpactLocation() - transform.m_position;
	if (impulse.dot(location) > 0)
		impulse.invert();

	physics.m_angularVelocity -= location.cross(impulse) / physics.m_momentOfInertia;
	impulse /= physics.m_mass;
	physics.m_velocity += impulse;
}