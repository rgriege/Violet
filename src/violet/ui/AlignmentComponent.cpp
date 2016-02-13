// ============================================================================

#include "violet/ui/AlignmentComponent.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag AlignmentComponent::getStaticTag()
{
	return Tag('u', 'a', 'l', 'n');
}

// ----------------------------------------------------------------------------

Thread AlignmentComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

AlignmentComponent::AlignmentComponent(const Handle entityId) :
	ComponentBase<AlignmentComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

AlignmentComponent::AlignmentComponent(const Handle entityId, Deserializer & /*deserializer*/) :
	ComponentBase<AlignmentComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

AlignmentComponent::AlignmentComponent(AlignmentComponent && other) :
	ComponentBase<AlignmentComponent>(std::move(other))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, AlignmentComponent & component)
{
	component.m_parent = Handle(deserializer.getUint("parent"), 0);
	component.m_x = deserializer.getFloat("x");
	component.m_y = deserializer.getFloat("y");
	component.m_width = deserializer.getFloat("width");
	component.m_height = deserializer.getFloat("height");
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const AlignmentComponent & /*component*/)
{
	return serializer;
}

// ============================================================================
