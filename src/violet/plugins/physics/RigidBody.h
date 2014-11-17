#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <string>
#include <memory>

#include "violet/core/math/Vec2.h"
#include "violet/core/math/Interval.h"
#include "violet/plugins/physics/shape/Shape.h"

namespace Violet
{
	class RigidBody
	{
	public:

		RigidBody();

		void apply_impulse(const Vec2f & impulse);
		void apply_force(const Vec2f & force);
		virtual void move(float time);
		void translate(const Vec2f & change);
		virtual void rotate(float radians) = 0;

	public:

		const float mass;
		Vec2f center, velocity, acceleration;
		bool fixed;
		std::unique_ptr<Shape const> const shape;
	};
}

#endif
