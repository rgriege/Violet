#ifndef SHAPE_H
#define SHAPE_H

#include "violet/core/math/Interval.h"
#include "violet/core/math/Vec2.h"

#include <vector>
#include <memory>

namespace Violet
{
	class Deserializer;

	class Shape
	{
	public:

		static std::unique_ptr<Shape> create(Deserializer & deserializer);

	public:

		Shape(Vec2f center, float mass, Vec2f velocity);
		virtual ~Shape();

	public:

		const Vec2f & getCenter() const;
		float getMass() const;
		const Vec2f & getVelocity() const;

		void translate(const Vec2f & translation);
		virtual void rotate(float radians) = 0;
		void applyImpulse(const Vec2f & impulse, const Vec2f & location = Vec2f());
		void applyForce(const Vec2f & force, const Vec2f & location = Vec2f());
		void update(float dt);

		virtual float calculateMomentOfInertia() const = 0;
		virtual FloatInterval project(const Vec2f & axis) const = 0;
		virtual float maxRadius() const = 0;
		virtual void findIntersectionAxes(const Shape & other, std::vector<Vec2f> & axes) const = 0;
		virtual std::unique_ptr<Shape> copy() const = 0;
		virtual void findClosestVertices(const Vec2f & axis, const Vec2f & goal, std::vector<Vec2f> & vertices) const = 0;

	protected:

		Shape(Deserializer & deserializer);

	protected:

		Vec2f m_center;
		float m_mass;
		Vec2f m_velocity;
		float m_angularVelocity;
		Vec2f m_force;
		float m_torque;
	};
}

#endif