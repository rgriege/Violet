#include "violet/core/transform/TransformComponent.h"

#include "violet/core/serialization/Deserializer.h"

TransformComponent::TransformComponent(const Entity & entity) :
	Component(entity),
	m_position(),
	m_rotation()
{
}

TransformComponent::TransformComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_position(deserializer),
	m_rotation(0)
{
}

TransformComponent::TransformComponent(TransformComponent && other) :
	Component(std::move(other)),
	m_position(std::move(other.m_position)),
	m_rotation(other.m_rotation)
{
}