// ============================================================================

#include "violet/physics/system/PhysicsSystem.h"

#include "violet/Engine.h"
#include "violet/scene/SceneProcessor.h"
#include "violet/entity/Entity.h"
#include "violet/serialization/Deserializer.h"
#include "violet/structures/QuadTree.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/TransformComponent.h"
#include "violet/physics/collision/Intersection.h"
#include "violet/window/WindowSystem.h"

#include <iostream>
#include <cstdio>

using namespace Violet;

namespace PhysicsSystemNamespace
{
	// ----------------------------------------------------------------------------

	void resolveCollisionForEntity(const Engine & engine, const TransformComponent & transform, const PhysicsComponent & physics, const Intersection & intersection, float impulseMagnitude);

	void move(PhysicsComponent & physics, const Engine & engine, const TransformComponent & transform, const Vec2f & gravity, float drag, float dt);
	void collide(PhysicsComponent & physics, const Vec2f & impulse, float angularImpulse);

	// ----------------------------------------------------------------------------

	float const ms_minimumSpeed = 0.01f;
	float const ms_restitution = 1.f;

	// ----------------------------------------------------------------------------
}

using namespace PhysicsSystemNamespace;

// ============================================================================

const char * PhysicsSystem::getStaticLabel()
{
	return "phys";
}

// ----------------------------------------------------------------------------

void PhysicsSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &PhysicsSystem::init);
}

// ----------------------------------------------------------------------------

std::unique_ptr<System> PhysicsSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());
	float const drag = settingsSegment->getFloat("drag");
	auto system = new PhysicsSystem(drag, Vec2f(*settingsSegment->enterSegment("gravity")));
	return std::unique_ptr<System>(system);
}

// ============================================================================

PhysicsSystem::PhysicsSystem(PhysicsSystem && other) :
	System(std::move(other)),
	m_drag(other.m_drag),
	m_gravity(other.m_gravity)
{
}

// ----------------------------------------------------------------------------

void PhysicsSystem::update(const float dt, const Engine & engine)
{
	const auto & windowSystem = engine.getSystem<WindowSystem>();
	const AABB boundary(0, 0, static_cast<float>(windowSystem->getWidth()), static_cast<float>(windowSystem->getHeight()));
	QuadTree<std::reference_wrapper<const Entity>> tree(boundary, 4);

	{
		SceneProcessor processor;
		processor.addDelegate(SceneProcessor::Filter::create<TransformComponent, PhysicsComponent>(), [&](const Entity & entity, const float dt)
		{
			auto & transformComponent = *entity.getComponent<TransformComponent>();
			auto & physicsComponent = *entity.getComponent<PhysicsComponent>();
			tree.insert(entity, physicsComponent.m_polygon.getBoundingBox().transform(transformComponent.m_transform));
			engine.addWriteTask(physicsComponent, move, std::cref(engine), std::cref(transformComponent), m_gravity, m_drag, dt);
		});
		processor.process(engine.getCurrentScene(), dt);
	}

	{
		SceneProcessor processor;
		processor.addDelegate(SceneProcessor::Filter::create<TransformComponent, PhysicsComponent>(), [&](const Entity & entity, const float dt)
		{
			auto & transformComponent = *entity.getComponent<TransformComponent>();
			auto & physicsComponent = *entity.getComponent<PhysicsComponent>();

			std::vector<std::reference_wrapper<const Entity>> otherEntities;
			tree.retrieve(otherEntities, physicsComponent.m_polygon.getBoundingBox().transform(transformComponent.m_transform));

			for (auto & otherEntity : otherEntities)
			{
				auto & otherTransformComponent = *otherEntity.get().getComponent<TransformComponent>();
				auto & otherPhysicsComponent = *otherEntity.get().getComponent<PhysicsComponent>();
				Intersection intersection(RigidBody(transformComponent, physicsComponent), RigidBody(otherTransformComponent, otherPhysicsComponent), dt);
				if (intersection.exists())
				{
					//printf("collision!\n");
					float const impulseMagnitude = (-(1 + ms_restitution) * (otherPhysicsComponent.m_velocity - physicsComponent.m_velocity).dot(intersection.getIntersectionAxis())) /
						(1 / physicsComponent.m_mass + 1 / otherPhysicsComponent.m_mass);
					resolveCollisionForEntity(engine, transformComponent, physicsComponent, intersection, impulseMagnitude);
					resolveCollisionForEntity(engine, otherTransformComponent, otherPhysicsComponent, intersection, impulseMagnitude);
					CollisionEvent::emit(engine.getEventContext(), entity, otherEntity.get());
				}
			}
		});
		processor.process(engine.getCurrentScene(), dt);
	}
}

// ============================================================================

PhysicsSystem::PhysicsSystem(float drag, Vec2f gravity) :
	System(getStaticLabel()),
	m_drag(drag),
	m_gravity(std::move(gravity))
{
}

// ============================================================================

void PhysicsSystemNamespace::resolveCollisionForEntity(const Engine & engine, const TransformComponent & transform, const PhysicsComponent & physics, const Intersection & intersection, const float impulseMagnitude)
{
	Vec2f impulse = intersection.getIntersectionAxis() * impulseMagnitude / physics.m_mass;
	Vec2f const location = intersection.getImpactLocation() - Vec2f(transform.m_transform[0][2], transform.m_transform[1][2]);
	if (impulse.dot(location) > 0)
		impulse.invert();

	engine.addWriteTask(physics, collide, impulse / physics.m_mass, -location.cross(impulse) / physics.m_momentOfInertia);
}

// ============================================================================

void PhysicsSystemNamespace::move(PhysicsComponent & physicsComponent, const Engine & engine, const TransformComponent & transformComponent, const Vec2f & gravity, const float drag, const float dt)
{
	Vec2f const oldPosition = { transformComponent.m_transform[0][2], transformComponent.m_transform[1][2] };
	Vec2f newPosition = oldPosition;

	if (!gravity.isZero() || drag != 0)
		physicsComponent.m_force += gravity - physicsComponent.m_velocity * drag;

	float const speed = physicsComponent.m_velocity.magSquared();
	if (speed > ms_minimumSpeed || !physicsComponent.m_force.isZero())
	{
		const Vec2f acceleration = physicsComponent.m_force / physicsComponent.m_mass;
		const Vec2f & velocity = physicsComponent.m_velocity;
		newPosition += acceleration * dt * dt / 2.f + velocity * dt;;
		physicsComponent.m_velocity += acceleration * dt;
		physicsComponent.m_force.zero();
	}
	else if (speed != 0)
		physicsComponent.m_velocity.zero();

	/*if (physicsComponent.m_torque > 0)
	{
		float const angularAcceleration = physicsComponent.m_torque / physicsComponent.m_momentOfInertia;
		newRotation = transformComponent.m_rotation + angularAcceleration * dt * dt + physicsComponent.m_angularVelocity * dt;
		physicsComponent.m_angularVelocity += angularAcceleration * dt;
		physicsComponent.m_torque = 0;
	}*/

	if (newPosition != oldPosition)
	{
		engine.addWriteTask(transformComponent, [newPosition](TransformComponent & transformComponent)
			{
				transformComponent.m_transform[0][2] = newPosition.x;
				transformComponent.m_transform[1][2] = newPosition.y;
			});
	}
}

// ============================================================================

void PhysicsSystemNamespace::collide(PhysicsComponent & physicsComponent, const Vec2f & impulse, const float angularImpulse)
{
	physicsComponent.m_angularVelocity += angularImpulse;
	physicsComponent.m_velocity += impulse;
}

// ============================================================================
