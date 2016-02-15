// ============================================================================

#include "violet/physics/system/physics_system.h"

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/serialization/deserializer.h"
#include "violet/structures/quad_tree.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/physics/collision/intersection.h"
#include "violet/utility/memory.h"
#include "violet/window/window_system.h"

using namespace vlt;

namespace PhysicsSystemNamespace
{
	// ----------------------------------------------------------------------------

	typedef entity<world_transform_component, physics_component> physics_entity;
	void resolve_collision(const physics_entity & entity, const intersection & intersection, const r32 impulseMagnitude);

	void update_physics(handle entity_id, const world_transform_component & tc, physics_component & pc, const v2 & gravity, const r32 drag, const r32 dt);
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

namespace
{
	struct init_task_data
	{
		r32 drag;
		v2 gravity;
	};
}

static void init_task(void * mem)
{
	auto data = make_unique<init_task_data>(mem);
	engine::instance().add_system(std::unique_ptr<vlt::system>(new physics_system(data->drag, data->gravity)));
}

void physics_system::init(deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());
	auto data = new init_task_data;
	data->drag = settingsSegment->get_r32("drag");
	data->gravity = v2(*settingsSegment->enter_segment("gravity"));
	add_task(init_task, data, physics_component::metadata->thread, task_type::write);
}

// ============================================================================

physics_system::physics_system(r32 drag, v2 gravity) :
	system(get_label_static()),
	m_drag(drag),
	m_gravity(std::move(gravity))
{
}

// ----------------------------------------------------------------------------

physics_system::physics_system(physics_system && other) :
	system(std::move(other)),
	m_drag(other.m_drag),
	m_gravity(other.m_gravity)
{
}

// ----------------------------------------------------------------------------

struct update_physics_task_data
{
	handle entity_id;
	v2 gravity;
	r32 drag;
	r32 dt;
};

static void update_physics_task(void * mem)
{
	auto data = make_unique<update_physics_task_data>(mem);
	
	auto tc = engine::instance().get_current_scene().get_component<world_transform_component>(data->entity_id);
	auto pc = engine::instance().get_current_scene().get_component<physics_component>(data->entity_id);
	update_physics(data->entity_id, *tc, *pc, data->gravity, data->drag, data->dt);
}

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
		add_task(update_physics_task, new update_physics_task_data{ entity.id, m_gravity, m_drag, dt }, physics_component::metadata->thread, task_type::write);
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
				resolve_collision(transformComponent, physicsComponent, intersection, impulseMagnitude);
				resolve_collision(otherWorldtransformComponent, otherPhysicsComponent, intersection, impulseMagnitude);
				CollisionEvent::emit(engine.event_context, entity, otherEntity.get());*/
			}
		}
	}
}

// ============================================================================

struct resolve_collision_task_data
{
	handle entity_id;
	v2 impulse;
	v2 location;
};

static void resolve_collision_task(void * mem)
{
	auto data = make_unique<resolve_collision_task_data>(mem);
	auto & physicsComponent = *engine::instance().get_current_scene().get_component<physics_component>(data->entity_id);
	collide(physicsComponent, data->impulse / physicsComponent.m_mass, -data->location.cross(data->impulse) / physicsComponent.m_momentOfInertia);
}

void PhysicsSystemNamespace::resolve_collision(const physics_entity & entity, const intersection & intersection, const r32 impulseMagnitude)
{
	const auto & transformComponent = entity.get<world_transform_component>();
	const auto & physicsComponent = entity.get<physics_component>();

	auto data = new resolve_collision_task_data;
	data->entity_id = entity.id;
	data->impulse = intersection.getIntersectionAxis() * impulseMagnitude / physicsComponent.m_mass;
	data->location = intersection.getImpactLocation() - v2(transformComponent.transform[0][2], transformComponent.transform[1][2]);
	if (data->impulse.dot(data->location) > 0)
		data->impulse.invert();

	add_task(resolve_collision_task, data, physics_component::metadata->thread, task_type::write);
}

// ============================================================================

struct move_task_data
{
	handle entity_id;
	v2 position;
};

static void move_task(void * mem)
{
	auto data = make_unique<move_task_data>(mem);

	auto * tc = engine::instance().get_current_scene().get_component<world_transform_component>(data->entity_id);
	tc->transform[0][2] = data->position.x;
	tc->transform[1][2] = data->position.y;
}

void PhysicsSystemNamespace::update_physics(const handle entity_id, const world_transform_component & tc, physics_component & pc, const v2 & gravity, const r32 drag, const r32 dt)
{
	v2 const oldPosition = { tc.transform[0][2], tc.transform[1][2] };
	v2 newPosition = oldPosition;

	if (!gravity.is_zero() || drag != 0)
		pc.m_force += gravity - pc.m_velocity * drag;

	r32 const speed = pc.m_velocity.mag_squared();
	if (speed > ms_minimumSpeed || !pc.m_force.is_zero())
	{
		const v2 acceleration = pc.m_force / pc.m_mass;
		const v2 & velocity = pc.m_velocity;
		newPosition += acceleration * dt * dt / 2.f + velocity * dt;;
		pc.m_velocity += acceleration * dt;
		pc.m_force.zero();
	}
	else if (speed != 0)
		pc.m_velocity.zero();

	/*if (physicsComponent.m_torque > 0)
	{
		r32 const angularAcceleration = physicsComponent.m_torque / physicsComponent.m_momentOfInertia;
		newRotation = tc.m_rotation + angularAcceleration * dt * dt + physicsComponent.m_angularVelocity * dt;
		physicsComponent.m_angularVelocity += angularAcceleration * dt;
		physicsComponent.m_torque = 0;
	}*/

	if (newPosition != oldPosition)
		add_task(move_task, new move_task_data{ entity_id, newPosition }, world_transform_component::metadata->thread, task_type::write);
}

// ============================================================================

void PhysicsSystemNamespace::collide(physics_component & physicsComponent, const v2 & impulse, const r32 angularImpulse)
{
	physicsComponent.m_angularVelocity += angularImpulse;
	physicsComponent.m_velocity += impulse;
}

// ============================================================================
