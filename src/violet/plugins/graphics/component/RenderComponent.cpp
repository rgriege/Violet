#include "violet/plugins/graphics/component/RenderComponent.h"

#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

RenderComponent::RenderComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_mesh(deserializer),
	m_color(deserializer)
{
}

RenderComponent::RenderComponent(RenderComponent && other) :
	Component(std::move(other)),
	m_mesh(std::move(other.m_mesh)),
	m_color(std::move(other.m_color))
{
}