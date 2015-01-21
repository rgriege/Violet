#include "violet/plugins/physics/system/PhysicsSystem.h"

#include "violet/core/AlterContext.h"
#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/plugins/physics/collision/Intersection.h"

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

void PhysicsSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &PhysicsSystem::init);
}

std::unique_ptr<System> PhysicsSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());
	float const drag = settingsSegment->getFloat("drag");
	auto system = new PhysicsSystem(drag, Vec2f(*settingsSegment->enterSegment("gravity")));
	ComponentFactory::getInstance().assign(PhysicsComponent::getLabel(), std::bind(&PhysicsSystem::create, system, std::placeholders::_1, std::placeholders::_2));
	return std::unique_ptr<System>(system);
}

void PhysicsSystem::update(const float dt, AlterContext & context)
{
	if (!m_gravity.isZero() || m_drag != 0)
		for (auto & component : *m_components)
			component.m_force += m_gravity - component.m_velocity * m_drag;

	for (auto & component : *m_components)
	{
		TransformComponent & transform = context.fetch<TransformSystem>(component.m_entity);
		updateEntity(transform, component, dt);
	}

	for (uint32 i = 0, len = m_components->size(); i < len; ++i)
	{
		PhysicsComponent & physics1 = m_components->operator[](i);
		TransformComponent & transform1 = context.fetch<TransformSystem>(physics1.m_entity);
		for (uint32 j = i + 1; j < len; ++j)
		{
			PhysicsComponent & physics2 = m_components->operator[](j);
			TransformComponent & transform2 = context.fetch<TransformSystem>(physics2.m_entity);
			Intersection intersection(RigidBody(transform1, physics1), RigidBody(transform2, physics2), dt);
			if (intersection.exists())
			{
				printf("collision!\n");
				float const impulseMagnitude = (-(1 + ms_restitution) * (physics2.m_velocity - physics1.m_velocity).dot(intersection.getIntersectionAxis())) /
					(1 / physics1.m_mass + 1 / physics2.m_mass);
				resolveCollisionForEntity(transform1, physics1, intersection, impulseMagnitude);
				resolveCollisionForEntity(transform2, physics2, intersection, impulseMagnitude);
			}
		}
	}
}

PhysicsSystem::PhysicsSystem(float drag, Vec2f gravity) :
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