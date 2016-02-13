// ============================================================================

#include "violet/physics/system/physics_system.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/serialization/deserializer.h"
#include "violet/structures/quad_tree.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/physics/collision/intersection.h"
#include "violet/window/window_system.h"

using namespace vlt;

namespace PhysicsSystemNamespace
{
	// ----------------------------------------------------------------------------

	void resolveCollisionForEntity(const world_transform_component & transform, const physics_component & physics, const intersection & intersection, const r32 impulseMagnitude);

	void move(physics_component & physics, const world_transform_component & transform, const v2 & gravity, const r32 drag, const r32 dt);
	void collide(physics_component & physics, const v2 & impulse, r32 angularImpulse);

	// ----------------------------------------------------------------------------

	r32 const ms_minimumSpeed = 0.01f;
	r32 const ms_restitution = 1.f;

	// ----------------------------------------------------------------------------
}

using namespace PhysicsSystemNamespace;

// ============================================================================

const char * physics_system::get_label_static()
{
	return "phys";
}

// ----------------------------------------------------------------------------

void physics_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &physics_system::init);
}

// ----------------------------------------------------------------------------

void physics_system::init(deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());
	r32 const drag = settingsSegment->get_r32("drag");
	v2 const gravity(*settingsSegment->enter_segment("gravity"));

	engine::instance().add_write_task(engine::instance(),
		[=](engine & engine)
		{
			engine.add_system(std::unique_ptr<system>(new physics_system(drag, gravity)));
		});
}

// ============================================================================

physics_system::physics_system(physics_system && other) :
	system(std::move(other)),
	m_drag(other.m_drag),
	m_gravity(other.m_gravity)
{
}

// ----------------------------------------------------------------------------

void physics_system::update(const r32 dt)
{
	const engine & engine = engine::instance();
	const auto & windowSystem = engine.get_system<window_system>();
	const aabb boundary(0, 0, static_cast<r32>(windowSystem->get_width()), static_cast<r32>(windowSystem->get_height()));
	quad_tree<rigid_body> tree(boundary, 4);

	for (const auto & entity : engine::instance().get_current_scene().get_entity_view<world_transform_component, physics_component>())
	{
		auto & transformComponent = entity.get<world_transform_component>();
		auto & physicsComponent = entity.get<physics_component>();
		tree.insert(rigid_body(transformComponent, physicsComponent), physicsComponent.m_polygon.get_bounding_box().transform(to2d(transformComponent.transform)));
		engine.add_write_task(physicsComponent,
			[&transformComponent, this, dt](physics_component & physics)
			{
				move(physics, transformComponent, m_gravity, m_drag, dt);
			});
	}
	for (const auto & entity : engine::instance().get_current_scene().get_entity_view<world_transform_component, physics_component>())
	{
		auto & transformComponent = entity.get<world_transform_component>();
		auto & physicsComponent = entity.get<physics_component>();
		rigid_body body(transformComponent, physicsComponent);

		std::vector<rigid_body> otherBodies;
		tree.retrieve(otherBodies, physicsComponent.m_polygon.get_bounding_box().transform(to2d(transformComponent.transform)));

		for (auto & otherBody : otherBodies)
		{
			intersection intersection(std::move(body), std::move(otherBody), dt);
			if (intersection.exists())
			{
				printf("collision!\n");
				/*r32 const impulseMagnitude = (-(1 + ms_restitution) * (otherPhysicsComponent.m_velocity - physicsComponent.m_velocity).dot(intersection.getIntersectionAxis())) /
					(1 / physicsComponent.m_mass + 1 / otherPhysicsComponent.m_mass);
				resolveCollisionForEntity(transformComponent, physicsComponent, intersection, impulseMagnitude);
				resolveCollisionForEntity(otherWorldtransformComponent, otherPhysicsComponent, intersection, impulseMagnitude);
				CollisionEvent::emit(engine.event_context, entity, otherEntity.get());*/
			}
		}
	}
}

// ============================================================================

physics_system::physics_system(r32 drag, v2 gravity) :
	system(get_label_static()),
	m_drag(drag),
	m_gravity(std::move(gravity))
{
}

// ============================================================================

void PhysicsSystemNamespace::resolveCollisionForEntity(const world_transform_component & transform, const physics_component & physics, const intersection & intersection, const r32 impulseMagnitude)
{
	v2 impulse = intersection.getIntersectionAxis() * impulseMagnitude / physics.m_mass;
	v2 const location = intersection.getImpactLocation() - v2(transform.transform[0][2], transform.transform[1][2]);
	if (impulse.dot(location) > 0)
		impulse.invert();

	engine::instance().add_write_task(physics,
		[=](physics_component & physicsComponent)
		{
			collide(physicsComponent, impulse / physicsComponent.m_mass, -location.cross(impulse) / physicsComponent.m_momentOfInertia);
		});
}

// ============================================================================

void PhysicsSystemNamespace::move(physics_component & physicsComponent, const world_transform_component & transformComponent, const v2 & gravity, const r32 drag, const r32 dt)
{
	v2 const oldPosition = { transformComponent.transform[0][2], transformComponent.transform[1][2] };
	v2 newPosition = oldPosition;

	if (!gravity.is_zero() || drag != 0)
		physicsComponent.m_force += gravity - physicsComponent.m_velocity * drag;

	r32 const speed = physicsComponent.m_velocity.mag_squared();
	if (speed > ms_minimumSpeed || !physicsComponent.m_force.is_zero())
	{
		const v2 acceleration = physicsComponent.m_force / physicsComponent.m_mass;
		const v2 & velocity = physicsComponent.m_velocity;
		newPosition += acceleration * dt * dt / 2.f + velocity * dt;;
		physicsComponent.m_velocity += acceleration * dt;
		physicsComponent.m_force.zero();
	}
	else if (speed != 0)
		physicsComponent.m_velocity.zero();

	/*if (physicsComponent.m_torque > 0)
	{
		r32 const angularAcceleration = physicsComponent.m_torque / physicsComponent.m_momentOfInertia;
		newRotation = transformComponent.m_rotation + angularAcceleration * dt * dt + physicsComponent.m_angularVelocity * dt;
		physicsComponent.m_angularVelocity += angularAcceleration * dt;
		physicsComponent.m_torque = 0;
	}*/

	if (newPosition != oldPosition)
	{
		engine::instance().add_write_task(transformComponent, [newPosition](world_transform_component & transformComponent)
			{
				transformComponent.transform[0][2] = newPosition.x;
				transformComponent.transform[1][2] = newPosition.y;
			});
	}
}

// ============================================================================

void PhysicsSystemNamespace::collide(physics_component & physicsComponent, const v2 & impulse, const r32 angularImpulse)
{
	physicsComponent.m_angularVelocity += angularImpulse;
	physicsComponent.m_velocity += impulse;
}

// ============================================================================
