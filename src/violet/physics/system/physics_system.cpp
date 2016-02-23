// ============================================================================

#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/physics/system/physics_system.h"
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

	typedef Entity<World_Transform_Component, Physics_Component> physics_entity;
	void resolve_collision(const physics_entity & entity, const Intersection & intersection, const r32 impulseMagnitude);

	void update_physics(Handle entity_id, const World_Transform_Component & tc, Physics_Component & pc, const v2 & gravity, const r32 drag, const r32 dt);
	void collide(Physics_Component & physics, const v2 & impulse, r32 angularImpulse);

	// ----------------------------------------------------------------------------

	r32 const ms_minimumSpeed = 0.01f;
	r32 const ms_restitution = 1.f;

	// ----------------------------------------------------------------------------
}

using namespace PhysicsSystemNamespace;

// ============================================================================

const char * Physics_System::get_label_static()
{
	return "phys";
}

// ----------------------------------------------------------------------------

void Physics_System::install(System_Factory & factory)
{
	factory.assign(get_label_static(), &Physics_System::init);
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
	Engine::instance().add_system(std::unique_ptr<vlt::System>(new Physics_System(data->drag, data->gravity)));
}

void Physics_System::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());
	auto data = new init_task_data;
	data->drag = settingsSegment->get_r32("drag");
	data->gravity = v2(*settingsSegment->enter_segment("gravity"));
	add_task(init_task, data, Physics_Component::metadata->thread, task_type::write);
}

// ============================================================================

Physics_System::Physics_System(r32 drag, v2 gravity) :
	System(get_label_static()),
	m_drag(drag),
	m_gravity(std::move(gravity))
{
}

// ----------------------------------------------------------------------------

Physics_System::Physics_System(Physics_System && other) :
	System(std::move(other)),
	m_drag(other.m_drag),
	m_gravity(other.m_gravity)
{
}

// ----------------------------------------------------------------------------

struct update_physics_task_data
{
	Handle entity_id;
	v2 gravity;
	r32 drag;
	r32 dt;
};

static void update_physics_task(void * mem)
{
	auto data = make_unique<update_physics_task_data>(mem);
	
	auto tc = Engine::instance().get_current_scene().get_component<World_Transform_Component>(data->entity_id);
	auto pc = Engine::instance().get_current_scene().get_component<Physics_Component>(data->entity_id);
	update_physics(data->entity_id, *tc, *pc, data->gravity, data->drag, data->dt);
}

void Physics_System::update(const r32 dt)
{
	const Engine & engine = Engine::instance();
	const auto & windowSystem = engine.get_system<Window_System>();
	const aabb boundary(0, 0, static_cast<r32>(windowSystem->get_width()), static_cast<r32>(windowSystem->get_height()));
	Quad_Tree<Rigid_Body> tree(boundary, 4);

	for (const auto & entity : Engine::instance().get_current_scene().get_entity_view<World_Transform_Component, Physics_Component>())
	{
		auto & transformComponent = entity.get<World_Transform_Component>();
		auto & physicsComponent = entity.get<Physics_Component>();
		tree.insert(Rigid_Body(transformComponent, physicsComponent), physicsComponent.m_polygon.get_bounding_box().transform(to2d(transformComponent.transform)));
		add_task(update_physics_task, new update_physics_task_data{ entity.id, m_gravity, m_drag, dt }, Physics_Component::metadata->thread, task_type::write);
	}
	for (const auto & entity : Engine::instance().get_current_scene().get_entity_view<World_Transform_Component, Physics_Component>())
	{
		auto & transformComponent = entity.get<World_Transform_Component>();
		auto & physicsComponent = entity.get<Physics_Component>();
		Rigid_Body body(transformComponent, physicsComponent);

		std::vector<Rigid_Body> otherBodies;
		tree.retrieve(otherBodies, physicsComponent.m_polygon.get_bounding_box().transform(to2d(transformComponent.transform)));

		for (auto & otherBody : otherBodies)
		{
			Intersection intersection(std::move(body), std::move(otherBody), dt);
			if (intersection.exists())
			{
				printf("collision!\n");
				/*r32 const impulseMagnitude = (-(1 + ms_restitution) * (otherPhysicsComponent.m_velocity - physicsComponent.m_velocity).dot(Intersection.getIntersectionAxis())) /
					(1 / physicsComponent.m_mass + 1 / otherPhysicsComponent.m_mass);
				resolve_collision(transformComponent, physicsComponent, Intersection, impulseMagnitude);
				resolve_collision(otherWorldtransformComponent, otherPhysicsComponent, Intersection, impulseMagnitude);
				CollisionEvent::emit(Engine.Event_Context, entity, otherEntity.get());*/
			}
		}
	}
}

// ============================================================================

struct resolve_collision_task_data
{
	Handle entity_id;
	v2 impulse;
	v2 location;
};

static void resolve_collision_task(void * mem)
{
	auto data = make_unique<resolve_collision_task_data>(mem);
	auto & physicsComponent = *Engine::instance().get_current_scene().get_component<Physics_Component>(data->entity_id);
	collide(physicsComponent, data->impulse / physicsComponent.m_mass, -data->location.cross(data->impulse) / physicsComponent.m_momentOfInertia);
}

void PhysicsSystemNamespace::resolve_collision(const physics_entity & entity, const Intersection & intersection, const r32 impulseMagnitude)
{
	const auto & transformComponent = entity.get<World_Transform_Component>();
	const auto & physicsComponent = entity.get<Physics_Component>();

	auto data = new resolve_collision_task_data;
	data->entity_id = entity.id;
	data->impulse = intersection.getIntersectionAxis() * impulseMagnitude / physicsComponent.m_mass;
	data->location = intersection.getImpactLocation() - v2(transformComponent.transform[0][2], transformComponent.transform[1][2]);
	if (data->impulse.dot(data->location) > 0)
		data->impulse.invert();

	add_task(resolve_collision_task, data, Physics_Component::metadata->thread, task_type::write);
}

// ============================================================================

struct move_task_data
{
	Handle entity_id;
	v2 position;
};

static void move_task(void * mem)
{
	auto data = make_unique<move_task_data>(mem);

	auto * tc = Engine::instance().get_current_scene().get_component<World_Transform_Component>(data->entity_id);
	tc->transform[0][2] = data->position.x;
	tc->transform[1][2] = data->position.y;
}

void PhysicsSystemNamespace::update_physics(const Handle entity_id, const World_Transform_Component & tc, Physics_Component & pc, const v2 & gravity, const r32 drag, const r32 dt)
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
		add_task(move_task, new move_task_data{ entity_id, newPosition }, World_Transform_Component::metadata->thread, task_type::write);
}

// ============================================================================

void PhysicsSystemNamespace::collide(Physics_Component & physicsComponent, const v2 & impulse, const r32 angularImpulse)
{
	physicsComponent.m_angularVelocity += angularImpulse;
	physicsComponent.m_velocity += impulse;
}

// ============================================================================
