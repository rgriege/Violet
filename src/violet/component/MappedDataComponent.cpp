// ============================================================================

#include "violet/component/MappedDataComponent.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

Tag MappedDataComponent::getStaticTag()
{
	return Tag('m', 'a', 'p', 'd');
}

// ----------------------------------------------------------------------------

Thread MappedDataComponent::getStaticThread()
{
	return Thread::Any;
}

// ============================================================================

MappedDataComponent::MappedDataComponent(const Handle entityId) :
	ComponentBase<MappedDataComponent>(entityId),
	m_data()
{
}

// ----------------------------------------------------------------------------

MappedDataComponent::MappedDataComponent(const Handle entityId, Deserializer & deserializer) :
	ComponentBase<MappedDataComponent>(entityId),
	m_data()
{
	const uint32 n = deserializer.getUint("n");
	for (uint32 i = 0; i < n; ++i)
	{
		const char * const key = deserializer.getString("key");
		m_data[key] = deserializer.getString("value");
	}
}

// ----------------------------------------------------------------------------

MappedDataComponent::MappedDataComponent(MappedDataComponent && other) :
	ComponentBase<MappedDataComponent>(std::move(other)),
	m_data(std::move(other.m_data))
{
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const MappedDataComponent & component)
{
	serializer.writeUint("n", component.m_data.size());
	for (auto const & kvp : component.m_data)
	{
		serializer.writeString("key", kvp.first.c_str());
		serializer.writeString("value", kvp.second.c_str());
	}
	return serializer;
}

// ============================================================================
