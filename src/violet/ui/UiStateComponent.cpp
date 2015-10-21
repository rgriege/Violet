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
	m_currentHandle(),
	m_states()
{
	auto const & stateFilenames = SerializationUtilities::deserializeElements<std::string>(deserializer);
	uint32 index = 0;
	for (auto const & subElementFilename : stateFilenames)
	{
		auto const deserializer = FileDeserializerFactory::getInstance().create(subElementFilename.c_str());
		if (deserializer != nullptr)
		{
			if (index == m_currentIndex)
			{
				m_states.emplace_back(make_unique_val<Entity>(owner.getScene()));
				m_currentHandle = m_owner.addChild(make_unique_val<Entity>(owner.getScene(), *deserializer)).getHandle();
			}
			else
				m_states.emplace_back(make_unique_val<Entity>(owner.getScene(), *deserializer));
		}
		else
			m_states.emplace_back(make_unique_val<Entity>(owner.getScene()));

		++index;
	}

	if (!m_currentHandle.isValid())
	{
		m_currentHandle = m_owner.addChild(std::move(m_states[0])).getHandle();
		m_states[0] = make_unique_val<Entity>(owner.getScene());
	}
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
		if (m_owner.stealChild(m_currentHandle, std::move(m_states[m_currentIndex])))
			m_currentHandle = m_owner.addChild(std::move(m_states[index])).getHandle();

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
	auto const & stateFilenames = SerializationUtilities::deserializeElements<std::string>(deserializer);
	for (auto const & subElementFilename : stateFilenames)
	{
		auto const deserializer = FileDeserializerFactory::getInstance().create(subElementFilename.c_str());
		if (deserializer != nullptr)
			component.m_states.emplace_back(make_unique_val<Entity>(component.m_owner.getScene(), *deserializer));
	}

	component.m_currentHandle = component.m_owner.addChild(std::move(component.m_states[component.m_currentIndex])).getHandle();
	return deserializer;
}

// ============================================================================
