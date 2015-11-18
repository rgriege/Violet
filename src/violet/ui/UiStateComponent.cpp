// ============================================================================

#include "violet/ui/UiStateComponent.h"

#include "violet/entity/Entity.h"
#include "violet/handle/HandleComponent.h"
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
				auto & child = m_owner.addChild(make_unique_val<Entity>(owner.getScene(), *deserializer));
				if (!child.hasComponent<HandleComponent>())
					child.addComponent<HandleComponent>();
				m_currentHandle = child.getComponent<HandleComponent>()->getHandle();
			}
			else
			{
				m_states.emplace_back(make_unique_val<Entity>(owner.getScene(), *deserializer));
				auto & child = *m_states.back();
				if (!child.hasComponent<HandleComponent>())
					child.addComponent<HandleComponent>();
			}
		}
		else
		{
			m_states.emplace_back(make_unique_val<Entity>(owner.getScene()));
			auto & child = *m_states.back();
			child.addComponent<HandleComponent>();
		}

		++index;
	}

	if (!m_currentHandle.isValid())
	{
		m_currentHandle = m_owner.addChild(std::move(m_states[0])).getComponent<HandleComponent>()->getHandle();
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
		auto & children = m_owner.getChildren();
		const auto it = std::find_if(children.begin(), children.end(), [=](const unique_val<Entity> & child)
			{
				return child->hasComponent<HandleComponent>() && child->getComponent<HandleComponent>()->getHandle() == m_currentHandle;
			});
		if (it != children.end())
		{
			auto & child = *it;
			std::swap(child, m_states[m_currentIndex]);
			std::swap(m_states[index], child);
			m_currentHandle = child->getComponent<HandleComponent>()->getHandle();
			m_currentIndex = index;
		}
		/*if (m_owner.stealChild(m_currentHandle, std::move(m_states[m_currentIndex])))
			m_currentHandle = m_owner.addChild(std::move(m_states[index])).getComponent<HandleComponent>()->getHandle();*/
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
	return deserializer;
}

// ============================================================================
