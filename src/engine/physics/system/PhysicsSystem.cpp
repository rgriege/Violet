// ============================================================================

#include "engine/physics/system/PhysicsSystem.h"

#include "engine/Engine.h"
#include "engine/scene/SceneUtilities.h"
#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"
#include "engine/structures/QuadTree.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/physics/collision/Intersection.h"
#include "engine/window/WindowSystem.h"

#include <iostream>
#include <cstdio>

using namespace Violet;

namespace PhysicsSystemNamespace
{
	// ----------------------------------------------------------------------------

	class MoveTask : public Engine::Task
	{
	public:

		MoveTask(const Engine & engine, Handle handle, const Vec2f & gravity, float drag, float dt);

		virtual void execute() const override;

	private:

		Handle m_entityHandle;
		Vec2f m_gravity;
		float m_drag;
		float m_dt;
	};

	class CollideTask : public Engine::Task
	{
	public:

		CollideTask(const Engine & engine, Handle handle, const Vec2f & impulse, float angularImpulse);

		virtual void execute() const override;

	private:

		Handle m_entityHandle;
		Vec2f m_impulse;
		float m_angularImpulse;
	};

	// ----------------------------------------------------------------------------

	void resolveCollisionForEntity(const Engine & engine, const TransformComponent & transform, const PhysicsComponent & physics, const Intersection & intersection, float impulseMagnitude);

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
		SceneUtilities::Processor processor;
		processor.addDelegate(SceneUtilities::Processor::Filter::create<TransformComponent, PhysicsComponent>(), [&](const Entity & entity, const float dt)
		{
			auto & transformComponent = *entity.getComponent<TransformComponent>();
			auto & physicsComponent = *entity.getComponent<PhysicsComponent>();
			tree.insert(entity, physicsComponent.m_polygon.getBoundingBox().translate(transformComponent.m_position));
			engine.addTask(std::make_unique<MoveTask>(engine, entity.getHandle(), m_gravity, m_drag, dt));
		});
		processor.process(engine.getCurrentScene(), dt);
	}

	{
		SceneUtilities::Processor processor;
		processor.addDelegate(SceneUtilities::Processor::Filter::create<TransformComponent, PhysicsComponent>(), [&](const Entity & entity, const float dt)
		{
			auto & transformComponent = *entity.getComponent<TransformComponent>();
			auto & physicsComponent = *entity.getComponent<PhysicsComponent>();

			std::vector<std::reference_wrapper<const Entity>> otherEntities;
			tree.retrieve(otherEntities, physicsComponent.m_polygon.getBoundingBox().translate(transformComponent.m_position));

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
	Vec2f const location = intersection.getImpactLocation() - transform.m_position;
	if (impulse.dot(location) > 0)
		impulse.invert();

	engine.addTask(std::make_unique<CollideTask>(engine, transform.getOwner().getHandle(), impulse / physics.m_mass, -location.cross(impulse) / physics.m_momentOfInertia));
}

// ============================================================================

PhysicsSystemNamespace::MoveTask::MoveTask(const Engine & engine, const Handle handle, const Vec2f & gravity, const float drag, const float dt) :
	Engine::Task(engine),
	m_entityHandle(handle),
	m_gravity(gravity),
	m_drag(drag),
	m_dt(dt)
{
}

// ----------------------------------------------------------------------------

void PhysicsSystemNamespace::MoveTask::execute() const
{
	const auto entity = m_engine.getCurrentScene().getEntity(m_entityHandle);
	if (entity != nullptr)
	{
		auto & transformComponent = *entity->getComponent<TransformComponent>();
		auto & physicsComponent = *entity->getComponent<PhysicsComponent>();
		if (!m_gravity.isZero() || m_drag != 0)
			physicsComponent.m_force += m_gravity - physicsComponent.m_velocity * m_drag;

		float const speed = physicsComponent.m_velocity.magSquared();
		if (speed > ms_minimumSpeed || !physicsComponent.m_force.isZero())
		{
			Vec2f const acceleration = physicsComponent.m_force / physicsComponent.m_mass;
			transformComponent.m_position += acceleration * m_dt * m_dt / 2.f + physicsComponent.m_velocity * m_dt;
			physicsComponent.m_velocity += acceleration * m_dt;
			physicsComponent.m_force.zero();
		}
		else if (speed != 0)
			physicsComponent.m_velocity.zero();

		float const angularAcceleration = physicsComponent.m_torque / physicsComponent.m_momentOfInertia;
		transformComponent.m_rotation += angularAcceleration * m_dt * m_dt + physicsComponent.m_angularVelocity * m_dt;
		physicsComponent.m_angularVelocity += angularAcceleration * m_dt;
		physicsComponent.m_torque = 0;
	}
}

// ============================================================================

PhysicsSystemNamespace::CollideTask::CollideTask(const Engine & engine, const Handle handle, const Vec2f & impulse, const float angularImpulse) :
	Engine::Task(engine),
	m_entityHandle(handle),
	m_impulse(impulse),
	m_angularImpulse(angularImpulse)
{
}

// ----------------------------------------------------------------------------

void PhysicsSystemNamespace::CollideTask::execute() const
{
	const auto entity = m_engine.getCurrentScene().getEntity(m_entityHandle);
	if (entity != nullptr)
	{
		auto & physicsComponent = entity->getComponent<PhysicsComponent>();
		if (physicsComponent != nullptr)
		{
			physicsComponent->m_angularVelocity += m_angularImpulse;
			physicsComponent->m_velocity += m_impulse;
		}
	}
}

// ============================================================================
