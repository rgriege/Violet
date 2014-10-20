#include "violet/plugins/graphics/component/RenderComponent.h"

#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

RenderComponent::RenderComponent(Deserializer & deserializer) :
	m_mesh(deserializer),
	m_color(deserializer)
{
}
