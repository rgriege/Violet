// ============================================================================

#include "editor/component/EditorComponent.h"

#include "violet/serialization/Serializer.h"

#include <assert.h>

using namespace edt;
using namespace Violet;

// ============================================================================

Tag EditorComponent::getStaticTag()
{
	return Tag('e', 'd', 't', 'r');
}

// ----------------------------------------------------------------------------

Thread EditorComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

EditorComponent::EditorComponent(const EntityId entityId) :
	ComponentBase<EditorComponent>(entityId)
{
}

// ----------------------------------------------------------------------------

EditorComponent::EditorComponent(const EntityId entityId, const Violet::ComponentDeserializer & /*deserializer*/) :
	ComponentBase<EditorComponent>(entityId)
{
	assert(false);
}

// ----------------------------------------------------------------------------

EditorComponent::EditorComponent(EditorComponent && other) :
	ComponentBase<EditorComponent>(std::move(other))
{
}

// ============================================================================

Violet::Serializer & edt::operator<<(Violet::Serializer & serializer, const EditorComponent & /*component*/)
{
	assert(false);
	return serializer;
}

// ============================================================================
