#include "violet/extras/serialization/JsonDeserializer.h"

#include <fstream>
#include <memory>
#include <algorithm>

using namespace Violet;

JsonDeserializer::Frame::Frame(const Json::Value * value) :
	m_value(value),
	m_accessCount()
{
}

Json::Value const & JsonDeserializer::Frame::get(const char * label)
{
	++m_accessCount;
	return m_value->operator[](label);
}

Json::Value const & JsonDeserializer::Frame::getArray()
{
	return m_value->operator[](m_accessCount++);
}

bool JsonDeserializer::Frame::finished() const
{
	return m_accessCount >= m_value->size();
}

JsonDeserializer::JsonDeserializer(std::istream & stream) :
	m_root(),
	m_valid(),
	m_stack()
{
	Json::Reader reader;
	if (reader.parse(stream, m_root, false))
	{
		m_stack.push_front(&m_root);
		m_valid = !m_stack.front().finished();
	}
}

JsonDeserializer::JsonDeserializer(JsonDeserializer && other) :
	m_root(std::move(other.m_root)),
	m_valid(other.m_valid),
	m_stack(std::move(other.m_stack))
{
}

JsonDeserializer::operator bool() const
{
	return m_valid && !m_stack.empty() && !(m_stack.size() == 1 && m_stack.front().finished());
}

void JsonDeserializer::enterSegment(const char * label)
{
	m_stack.emplace_front(label != nullptr ? &m_stack.front().get(label) : &m_stack.front().getArray());
}

void JsonDeserializer::leaveSegment()
{
	m_stack.pop_front();
}

bool JsonDeserializer::getBoolean(const char * label)
{
	return m_stack.front().get(label).asBool();
}

uint32 JsonDeserializer::getUint(const char * label)
{
	return m_stack.front().get(label).asUInt();
}

int JsonDeserializer::getInt(const char * label)
{
	return m_stack.front().get(label).asInt();
}

float JsonDeserializer::getFloat(const char * label)
{
	return static_cast<float>(getDouble(label));
}

double JsonDeserializer::getDouble(const char * label)
{
	return m_stack.front().get(label).asDouble();
}

const char * JsonDeserializer::getString(const char * label)
{
	return m_stack.front().get(label).asCString();
}