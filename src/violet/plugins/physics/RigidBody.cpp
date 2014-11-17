#include "violet/plugins/physics/RigidBody.h"

using namespace std;

RigidBody::RigidBody() :
	mass(1),
	fixed(false),
	center(),
	velocity(),
	acceleration()
{
}

void RigidBody::apply_impulse(const Vec2f & impulse)
{
	velocity += (1/mass)*impulse;
}

void RigidBody::apply_force(const Vec2f & force)
{
	acceleration += (1/mass)*force;
}

void RigidBody::move(float time)
{
	center += time*velocity;
	velocity += time*acceleration;
	acceleration = Vec2f();
}

void RigidBody::translate(const Vec2f & change)
{
	center += change;
}
