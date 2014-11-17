#include "violet/plugins/physics/shape/Shape.h"

#include "violet/core/serialization/Deserializer.h"
#include "violet/plugins/physics/shape/Polygon.h"

using namespace Violet;

namespace ShapeNamespace
{
	const char * const ms_segmentLabel = "shape";
	const char * const ms_massLabel = "mass";
}

using namespace ShapeNamespace;

std::unique_ptr<Shape> Shape::create(Deserializer & deserializer)
{
	return std::make_unique<Polygon>(deserializer);
}

Shape::Shape(const Vec2f center, const float mass, const Vec2f velocity) :
	m_center(std::move(center)),
	m_mass(mass),
	m_velocity(std::move(velocity))
{
}

Shape::Shape(Deserializer & deserializer)
{
	deserializer.enterSegment(ShapeNamespace::ms_segmentLabel);
	m_center = Vec2f(deserializer);
	m_mass = deserializer.getFloat(ms_massLabel);
	deserializer.leaveSegment();
}

Shape::~Shape()
{
}

const Vec2f & Shape::getCenter() const
{
	return m_center;
}

float Shape::getMass() const
{
	return m_mass;
}

const Vec2f & Shape::getVelocity() const
{
	return m_velocity;
}

void Shape::translate(const Vec2f & translation)
{
	m_center += translation;
}

void Shape::applyImpulse(const Vec2f & impulse, const Vec2f & location)
{
	m_velocity += impulse / m_mass;
	m_angularVelocity += impulse.cross(location) / calculateMomentOfInertia();
}

void Shape::applyForce(const Vec2f & force, const Vec2f & location)
{
	m_force += force;
	m_torque += force.cross(location);
}

void Shape::update(const float dt)
{
	Vec2f const acceleration = m_force / m_mass;
	translate(acceleration * dt * dt / 2.f + m_velocity * dt);
	m_velocity += acceleration * dt;
	m_force.zero();

	float const angularAcceleration = m_torque / calculateMomentOfInertia();
	rotate(angularAcceleration * dt * dt + m_angularVelocity * dt);
	m_angularVelocity += angularAcceleration * dt;
	m_torque = 0;
}