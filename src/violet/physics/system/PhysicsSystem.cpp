// ============================================================================

#include "violet/physics/system/PhysicsSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/serialization/Deserializer.h"
#include "violet/structures/QuadTree.h"
#include "violet/system/SystemFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/physics/collision/Intersection.h"
#include "violet/window/WindowSystem.h"

using namespace Violet;

namespace PhysicsSystemNamespace
{
	// ----------------------------------------------------------------------------

	void resolveCollisionForEntity(const WorldTransformComponent & transform, const PhysicsComponent & physics, const Intersection & intersection, float impulseMagnitude);

	void move(PhysicsComponent & physics, const WorldTransformComponent & transform, const Vec2f & gravity, float drag, float dt);
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

void PhysicsSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());
	float const drag = settingsSegment->getFloat("drag");
	Vec2f const gravity(*settingsSegment->enterSegment("gravity"));

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[=](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new PhysicsSystem(drag, gravity)));
		});
}

// ============================================================================

PhysicsSystem::PhysicsSystem(PhysicsSystem && other) :
	System(std::move(other)),
	m_drag(other.m_drag),
	m_gravity(other.m_gravity)
{
}

// ----------------------------------------------------------------------------

void PhysicsSystem::update(const float dt)
{
	const Engine & engine = Engine::getInstance();
	const auto & windowSystem = engine.getSystem<WindowSystem>();
	const AABB boundary(0, 0, static_cast<float>(windowSystem->getWidth()), static_cast<float>(windowSystem->getHeight()));
	QuadTree<RigidBody> tree(boundary, 4);

	for (const auto entity : Engine::getInstance().getCurrentScene().getEntityView<WorldTransformComponent, PhysicsComponent>())
	{
		auto & transformComponent = std::get<0>(entity);
		auto & physicsComponent = std::get<1>(entity);
		tree.insert(RigidBody(transformComponent, physicsComponent), physicsComponent.m_polygon.getBoundingBox().transform(to2d(transformComponent.m_transform)));
		engine.addWriteTask(physicsComponent,
			[&transformComponent, this, dt](PhysicsComponent & physics)
			{
				move(physics, transformComponent, m_gravity, m_drag, dt);
			});
	}
	for (const auto entity : Engine::getInstance().getCurrentScene().getEntityView<WorldTransformComponent, PhysicsComponent>())
	{
		auto & transformComponent = std::get<0>(entity);
		auto & physicsComponent = std::get<1>(entity);
		RigidBody body(transformComponent, physicsComponent);

		std::vector<RigidBody> otherBodies;
		tree.retrieve(otherBodies, physicsComponent.m_polygon.getBoundingBox().transform(to2d(transformComponent.m_transform)));

		for (auto & otherBody : otherBodies)
		{
			Intersection intersection(std::move(body), std::move(otherBody), dt);
			if (intersection.exists())
			{
				printf("collision!\n");
				/*float const impulseMagnitude = (-(1 + ms_restitution) * (otherPhysicsComponent.m_velocity - physicsComponent.m_velocity).dot(intersection.getIntersectionAxis())) /
					(1 / physicsComponent.m_mass + 1 / otherPhysicsComponent.m_mass);
				resolveCollisionForEntity(transformComponent, physicsComponent, intersection, impulseMagnitude);
				resolveCollisionForEntity(otherWorldTransformComponent, otherPhysicsComponent, intersection, impulseMagnitude);
				CollisionEvent::emit(engine.getEventContext(), entity, otherEntity.get());*/
			}
		}
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

void PhysicsSystemNamespace::resolveCollisionForEntity(const WorldTransformComponent & transform, const PhysicsComponent & physics, const Intersection & intersection, const float impulseMagnitude)
{
	Vec2f impulse = intersection.getIntersectionAxis() * impulseMagnitude / physics.m_mass;
	Vec2f const location = intersection.getImpactLocation() - Vec2f(transform.m_transform[0][2], transform.m_transform[1][2]);
	if (impulse.dot(location) > 0)
		impulse.invert();

	Engine::getInstance().addWriteTask(physics,
		[=](PhysicsComponent & physicsComponent)
		{
			collide(physicsComponent, impulse / physicsComponent.m_mass, -location.cross(impulse) / physicsComponent.m_momentOfInertia);
		});
}

// ============================================================================

void PhysicsSystemNamespace::move(PhysicsComponent & physicsComponent, const WorldTransformComponent & transformComponent, const Vec2f & gravity, const float drag, const float dt)
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
		Engine::getInstance().addWriteTask(transformComponent, [newPosition](WorldTransformComponent & transformComponent)
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
