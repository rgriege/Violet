#ifndef VIOLET_RigidBody_H
#define VIOLET_RigidBody_H

#include "violet/math/Interval.h"
#include "violet/math/Polygon.h"
#include "violet/math/Vec2.h"

namespace Violet
{
	class WorldTransformComponent;
	class PhysicsComponent;

	class VIOLET_API RigidBody
	{
	public:

		RigidBody(const WorldTransformComponent & transform, const PhysicsComponent & physics);
		RigidBody(Vec2f && center, Polygon && polygon, float mass);

		const Vec2f & getCenter();
		const Vec2f & getVelocity();

		void translate(const Vec2f & translation);
		void rotate(float radians);
		void applyImpulse(Vec2f impulse, const Vec2f & location = Vec2f::ZERO);

		FloatInterval project(const Vec2f & axis) const;
		float maxRadius() const;
		void findIntersectionAxes(const RigidBody & other, std::vector<Vec2f> & axes) const;
		void findClosestVertices(const Vec2f & axis, const Vec2f & goal, std::vector<Vec2f> & vertices) const;

	private:

		Vec2f m_center;
		float m_rotation;
		Polygon m_polygon;
		float m_mass;
		Vec2f m_velocity;
		Vec2f m_force;
		float m_momentOfInertia;
		float m_angularVelocity;
		float m_torque;
	};
}

#endif