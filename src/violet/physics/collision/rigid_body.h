#ifndef VIOLET_RIGID_BODY_H
#define VIOLET_RIGID_BODY_H

#include "violet/math/interval.h"
#include "violet/math/poly.h"
#include "violet/math/v2.h"

#include <vector>

namespace vlt
{
	struct world_transform_component;
	struct physics_component;

	struct VIOLET_API rigid_body final
	{
		rigid_body(const world_transform_component & transform, const physics_component & physics);
		rigid_body(v2 && center, poly && poly, r32 mass);

		const v2 & get_center();
		const v2 & getVelocity();

		void translate(const v2 & translation);
		void rotate(r32 radians);
		void applyImpulse(v2 impulse, const v2 & location = v2::Zero);

		interval project(const v2 & axis) const;
		r32 maxRadius() const;
		void findIntersectionAxes(const rigid_body & other, std::vector<v2> & axes) const;
		void findClosestVertices(const v2 & axis, const v2 & goal, std::vector<v2> & vertices) const;

	private:

		v2 m_center;
		r32 m_rotation;
		poly m_polygon;
		r32 m_mass;
		v2 m_velocity;
		v2 m_force;
		r32 m_momentOfInertia;
		r32 m_angularVelocity;
		r32 m_torque;
	};
}

#endif