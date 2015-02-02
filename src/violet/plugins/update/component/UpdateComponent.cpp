#include "violet/plugins/update/component/UpdateComponent.h"

#include <vector>

using namespace Violet;

const char * UpdateComponent::getLabel()
{
	return "updt";
}

UpdateComponent::UpdateComponent(const Entity & entity, Deserializer & /*deserializer*/) :
	Component(entity)
{
}

UpdateComponent::UpdateComponent(UpdateComponent && other) :
	Component(std::move(other))
{
}