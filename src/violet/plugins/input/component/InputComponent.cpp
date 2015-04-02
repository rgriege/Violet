#include "violet/plugins/input/component/InputComponent.h"

using namespace Violet;

const char * InputComponent::getLabel()
{
	return "inpt";
}

InputComponent::InputComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_mesh(deserializer)
{
}

InputComponent::InputComponent(InputComponent && other) :
	Component(std::move(other)),
	m_mesh(std::move(other.m_mesh))
{
}

InputComponent & InputComponent::operator=(InputComponent && other)
{
	m_mesh = std::move(other.m_mesh);
	return *this;
}