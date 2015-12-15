// ============================================================================

#include "violet/component/ComponentDeserializer.h"

using namespace Violet;

// ============================================================================

ComponentDeserializer::ComponentDeserializer(std::unique_ptr<Deserializer> && deserializer, std::shared_ptr<const std::unordered_map<uint32, EntityId>> handleIdMap) :
	m_deserializer(std::move(deserializer)),
	m_handleIdMap(std::move(handleIdMap))
{
}

// ----------------------------------------------------------------------------

ComponentDeserializer::operator bool() const
{
	return *m_deserializer;
}

// ----------------------------------------------------------------------------


std::unique_ptr<Deserializer> ComponentDeserializer::enterSegment(const char * const label)
{
	return m_deserializer->enterSegment(label);
}

// ----------------------------------------------------------------------------


const char * ComponentDeserializer::nextLabel() const
{
	return m_deserializer->nextLabel();
}

// ----------------------------------------------------------------------------


bool ComponentDeserializer::getBoolean(const char * const label)
{
	return m_deserializer->getBoolean(label);
}

// ----------------------------------------------------------------------------


uint32 ComponentDeserializer::getUint(const char * const label)
{
	return m_deserializer->getUint(label);
}

// ----------------------------------------------------------------------------


int32 ComponentDeserializer::getInt(const char * const label)
{
	return m_deserializer->getInt(label);
}

// ----------------------------------------------------------------------------


float ComponentDeserializer::getFloat(const char * const label)
{
	return m_deserializer->getFloat(label);
}

// ----------------------------------------------------------------------------


double ComponentDeserializer::getDouble(const char * const label)
{
	return m_deserializer->getDouble(label);
}

// ----------------------------------------------------------------------------


const char * ComponentDeserializer::getString(const char * const label)
{
	return m_deserializer->getString(label);
}

// ----------------------------------------------------------------------------

EntityId ComponentDeserializer::getEntityId(const char * const label)
{
	const uint32 desiredId = m_deserializer->getUint(label);
	const auto it = m_handleIdMap->find(desiredId);
	return it == m_handleIdMap->end() ? EntityId(desiredId, 0) : it->second;
}

// ============================================================================
