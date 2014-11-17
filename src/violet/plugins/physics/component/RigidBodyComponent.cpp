#include "violet/plugins/physics/component/RigidBodyComponent.h"

#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

RigidBodyComponent::RigidBodyComponent(Deserializer & deserializer) :
	m_shape(std::move(Shape::create(deserializer)))
{
}

RigidBodyComponent::RigidBodyComponent(RigidBodyComponent && other) :
	m_shape(std::move(other.m_shape))
{

}