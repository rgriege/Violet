// ============================================================================

#include "engine/serialization/binary/BinaryDeserializer.h"

#include "engine/serialization/file/FileDeserializerFactory.h"

using namespace Violet;

// ============================================================================

namespace BinaryDeserializerNamespace
{
	template <typename T>
	T getValue(std::istream & stream);
}

using namespace BinaryDeserializerNamespace;

// ============================================================================

void BinaryDeserializer::install()
{
	 FileDeserializerFactory::getInstance().assign<BinaryDeserializer>("vba");
}

// ============================================================================

BinaryDeserializer::BinaryDeserializer(std::istream & stream) :
	m_stream(stream),
	m_stringBuffer()
{
}

// ----------------------------------------------------------------------------

BinaryDeserializer::operator bool() const
{
	// error with implicitly/explicitly calling operator bool()
	return static_cast<bool>(m_stream);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Deserializer> BinaryDeserializer::enterSegment(const char * /*label*/)
{
	return std::unique_ptr<Deserializer>(new BinaryDeserializer(m_stream));
}

// ----------------------------------------------------------------------------

const char * BinaryDeserializer::nextLabel() const
{
	return "";
}

// ----------------------------------------------------------------------------

bool BinaryDeserializer::getBoolean(const char * /*label*/)
{
	return getValue<bool>(m_stream);
}

// ----------------------------------------------------------------------------

uint32 BinaryDeserializer::getUint(const char * /*label*/)
{
	return getValue<uint32>(m_stream);
}

// ----------------------------------------------------------------------------

int32 BinaryDeserializer::getInt(const char * /*label*/)
{
	return getValue<int>(m_stream);
}

// ----------------------------------------------------------------------------

float BinaryDeserializer::getFloat(const char * /*label*/)
{
	return getValue<float>(m_stream);
}

// ----------------------------------------------------------------------------

double BinaryDeserializer::getDouble(const char * /*double*/)
{
	return getValue<double>(m_stream);
}

// ----------------------------------------------------------------------------

const char * BinaryDeserializer::getString(const char * /*label*/)
{
	m_stringBuffer.clear();
	char c = '\0';
	do
	{
		m_stream >> c;
		m_stringBuffer += c;
	}
	while (c != '\0');
	return m_stringBuffer.c_str();
}

// ============================================================================

template <typename T>
T BinaryDeserializerNamespace::getValue(std::istream & stream)
{
	T value = T();
	stream >> value;
	return value;
}

// ============================================================================
