// ============================================================================

#include "violet/ui/alignmentcomponent.h"

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

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

AlignmentComponent::AlignmentComponent(const Handle entityId, Deserializer & /*Deserializer*/) :
	ComponentBase<AlignmentComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

AlignmentComponent::AlignmentComponent(AlignmentComponent && other) :
	ComponentBase<AlignmentComponent>(std::move(other))
{
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & Deserializer, AlignmentComponent & component)
{
	component.m_parent = Handle(Deserializer.getUint("parent"), 0);
	component.m_x = Deserializer.getFloat("x");
	component.m_y = Deserializer.getFloat("y");
	component.m_width = Deserializer.getFloat("width");
	component.m_height = Deserializer.getFloat("height");
	return Deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & Serializer, const AlignmentComponent & /*component*/)
{
	return Serializer;
}

// ============================================================================
