// ============================================================================

#include "violet/ui/UiStateComponent.h"

#include "violet/entity/Entity.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/serialization/SerializationUtilities.h"
#include "violet/serialization/file/FileDeserializerFactory.h"

using namespace Violet;

// ============================================================================

Tag UiStateComponent::getStaticTag()
{
	return Tag('u', 'i', 's', 'c');
}

// ============================================================================

UiStateComponent::UiStateComponent(Entity & owner, Deserializer & deserializer) :
	ComponentBase<UiStateComponent>(owner),
	m_currentIndex(deserializer.getUint("index")),
	m_states(SerializationUtilities::deserializeElements<std::string>(deserializer))
{
	const uint32 index = m_currentIndex;
	m_currentIndex = std::numeric_limits<uint32>::max();
	setCurrentIndex(index);
}

// ----------------------------------------------------------------------------

uint32 UiStateComponent::getCurrentIndex() const
{
	return m_currentIndex;
}

// ----------------------------------------------------------------------------

void UiStateComponent::setCurrentIndex(const uint32 index)
{
	if (m_currentIndex != index)
	{
		const auto & stateElementFileName = m_states[index];
		m_owner.getChildren().clear();
		auto const deserializer = FileDeserializerFactory::getInstance().create(stateElementFileName.c_str());
		if (deserializer != nullptr)
			m_owner.addChild(*deserializer);

		m_currentIndex = index;
	}
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const UiStateComponent & component)
{
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer & Violet::operator>>(Deserializer & deserializer, UiStateComponent & component)
{
	component.m_currentIndex = deserializer.getUint("index");
	component.m_states = SerializationUtilities::deserializeElements<std::string>(deserializer);
	return deserializer;
}

// ============================================================================
