#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include "violet/core/serialization/Deserializer.h"
#include "violet/plugins/physics/shape/Shape.h"

namespace Violet
{
	class RigidBodyComponent
	{
	public:

		RigidBodyComponent(Deserializer & deserializer);
		RigidBodyComponent(RigidBodyComponent && other);
		RigidBodyComponent(const RigidBodyComponent &) = delete;

	public:

		std::unique_ptr<Shape> m_shape;
	};
}

#endif