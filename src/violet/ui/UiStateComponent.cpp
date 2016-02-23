// ============================================================================

#include "violet/ui/uistatecomponent.h"

#include "violet/entity/entity.h"
#include "violet/handle/handlecomponent.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/serialization/serializationutilities.h"
#include "violet/serialization/file/filedeserializerfactory.h"

using namespace Violet;

// ============================================================================

Tag UiStateComponent::getStaticTag()
{
	return Tag('u', 'i', 's', 'c');
}

// ============================================================================

UiStateComponent::UiStateComponent(const Handle entityId, Deserializer & Deserializer) :
	ComponentBase<UiStateComponent>(entityId),
	m_currentIndex(Deserializer.getUint("index")),
	m_currentHandle(),
	m_states()
{
	auto const & stateFilenames = SerializationUtilities::deserializeElements<std::string>(Deserializer);
	uint32 index = 0;
	for (auto const & subElementFilename : stateFilenames)
	{
		auto const Deserializer = FileDeserializerFactory::getInstance().create(subElementFilename.c_str());
		if (Deserializer != nullptr)
		{
			if (index == m_currentIndex)
			{
				m_states.emplace_back(make_unique_val<Entity>(entityId.getScene()));
				auto & child = m_entityId.addChild(make_unique_val<Entity>(entityId.getScene(), *Deserializer));
				if (!child.hasComponent<HandleComponent>())
					child.addComponent<HandleComponent>();
				m_currentHandle = child.getComponent<HandleComponent>()->getHandle();
			}
			else
			{
				m_states.emplace_back(make_unique_val<Entity>(entityId.getScene(), *Deserializer));
				auto & child = *m_states.back();
				if (!child.hasComponent<HandleComponent>())
					child.addComponent<HandleComponent>();
			}
		}
		else
		{
			m_states.emplace_back(make_unique_val<Entity>(entityId.getScene()));
			auto & child = *m_states.back();
			child.addComponent<HandleComponent>();
		}

		++index;
	}

	if (!m_currentHandle.isValid())
	{
		m_currentHandle = m_entityId.addChild(std::move(m_states[0])).getComponent<HandleComponent>()->getHandle();
		m_states[0] = make_unique_val<Entity>(entityId.getScene());
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
		auto & children = m_entityId.getChildren();
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
		/*if (m_entityId.stealChild(m_currentHandle, std::move(m_states[m_currentIndex])))
			m_currentHandle = m_entityId.addChild(std::move(m_states[index])).getComponent<HandleComponent>()->getHandle();*/
	}
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & Serializer, const UiStateComponent & component)
{
	return Serializer;
}

// ----------------------------------------------------------------------------

Deserializer & Violet::operator>>(Deserializer & Deserializer, UiStateComponent & component)
{
	return Deserializer;
}

// ============================================================================
