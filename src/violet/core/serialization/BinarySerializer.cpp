#include "violet/core/serialization/BinaryDeserializer.h"

using namespace Violet;

BinaryDeserializer::BinaryDeserializer(std::istream & stream) :
	m_stream(stream)
{
}

BinaryDeserializer::operator bool() const
{
	// error with implicitly/explicitly calling operator bool()
	return m_stream ? true : false;
}

BinaryDeserializer & BinaryDeserializer::operator>>(bool & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(uint8 & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(int8 & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(uint16 & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(int16 & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(uint32 & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(int32 & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(float & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(double & value)
{
	m_stream >> value;
	return *this;
}

BinaryDeserializer & BinaryDeserializer::operator>>(std::string & value)
{
	m_stream >> value;
	return *this;
}