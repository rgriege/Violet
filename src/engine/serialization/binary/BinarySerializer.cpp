// ============================================================================

#include "engine/serialization/binary/BinarySerializer.h"

#include "engine/serialization/file/FileSerializerFactory.h"

using namespace Violet;

// ============================================================================

namespace BinarySerializerNamespace
{
	template <typename T>
	void writeValue(std::ostream & stream, T value);
}

using namespace BinarySerializerNamespace;

// ============================================================================

void BinarySerializer::install()
{
	FileSerializerFactory::getInstance().assign<BinarySerializer>("vba");
}

// ============================================================================

BinarySerializer::BinarySerializer(std::ostream & stream) :
	m_stream(stream)
{
}

// ----------------------------------------------------------------------------

unique_val<Serializer> BinarySerializer::createSegment(const char * /*label*/)
{
	return unique_val<Serializer>::create<BinarySerializer>(m_stream);
}

// ----------------------------------------------------------------------------

void BinarySerializer::writeBoolean(const char * /*label*/, bool const value)
{
	writeValue(m_stream, value);
}

// ----------------------------------------------------------------------------

void BinarySerializer::writeUint(const char * /*label*/, uint32 const value)
{
	writeValue(m_stream, value);
}

// ----------------------------------------------------------------------------

void BinarySerializer::writeInt(const char * /*label*/, int const value)
{
	writeValue(m_stream, value);
}

// ----------------------------------------------------------------------------

void BinarySerializer::writeFloat(const char * /*label*/, float const value)
{
	writeValue(m_stream, value);
}

// ----------------------------------------------------------------------------

void BinarySerializer::writeDouble(const char * /*double*/, double const value)
{
	writeValue(m_stream, value);
}

// ----------------------------------------------------------------------------

void BinarySerializer::writeString(const char * /*label*/, const char * value)
{
	m_stream << value << '\0';
}

// ============================================================================

template <typename T>
void BinarySerializerNamespace::writeValue(std::ostream & stream, T const value)
{
	stream << value << ' ';
}

// ============================================================================
