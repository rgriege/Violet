#include "violet/plugins/input/component/InputComponent.h"

#include "violet/core/script/ScriptFactory.h"

const char * InputComponent::getLabel()
{
	return "inpt";
}

InputComponent::InputComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_mesh(deserializer),
	m_script(ScriptFactory::getInstance().create(deserializer.getString("script")))
{
}

InputComponent::InputComponent(InputComponent && other) :
	Component(std::move(other)),
	m_mesh(std::move(other.m_mesh)),
	m_script(std::move(other.m_script))
{
}