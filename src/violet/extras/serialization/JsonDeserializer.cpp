/*
 * NOTE: this uses a modified version of the JsonCpp library and will NOT work with the original source.
 * The deserialization process is designed to be read in the order presented in the input stream. However,
 * raw json does not enforce this. You can find the modified version at https://github.com/rgriege/jsoncpp.
 *
 */

#include "violet/extras/serialization/JsonDeserializer.h"

#include "violet/core/serialization/FileDeserializerFactory.h"

#include <memory>

using namespace Violet;

namespace JsonDeserializerNamespace
{
	class SubDeserializer : public Deserializer
	{
	public:

		SubDeserializer(const Json::Value & value);

		virtual operator bool() const override;

		virtual std::unique_ptr<Deserializer> enterSegment(const char * label) override;
		virtual const char * nextLabel() const override;

		virtual bool getBoolean(const char * label) override;
		virtual uint32 getUint(const char * label) override;
		virtual int32 getInt(const char * label) override;
		virtual float getFloat(const char * label) override;
		virtual double getDouble(const char * label) override;
		virtual const char * getString(const char * label) override;

	private:

		const Json::Value & m_value;
		Json::ValueConstIterator m_position;
	};

	std::unique_ptr<JsonDeserializer> create(std::istream & stream);
}

using namespace JsonDeserializerNamespace;

void JsonDeserializer::install()
{
	FileDeserializerFactory::getInstance().assign("json", &create);
}

JsonDeserializer::JsonDeserializer(std::istream & stream) :
	m_root(),
	m_position()
{
	Json::Reader reader;
	m_position = reader.parse(stream, m_root, false) ? m_root.begin() : m_root.end();
}

JsonDeserializer::JsonDeserializer(JsonDeserializer && other) :
	m_root(),
	m_position()
{
	const uint32 end = other.m_position - other.m_root.begin();

	m_root = std::move(other.m_root);
	m_position = m_root.begin();
	for (uint32 i = 0; i < end; ++i)
		++m_position;
}

JsonDeserializer::operator bool() const
{
	return m_position != m_root.end();
}

std::unique_ptr<Deserializer> JsonDeserializer::enterSegment(const char * label)
{
	return std::unique_ptr<Deserializer>(new SubDeserializer(*m_position++));
}

const char * JsonDeserializer::nextLabel() const
{
	return m_position.memberName();
}

bool JsonDeserializer::getBoolean(const char * label)
{
	return (*m_position++).asBool();
}

uint32 JsonDeserializer::getUint(const char * label)
{
	return (*m_position++).asUInt();
}

int JsonDeserializer::getInt(const char * label)
{
	return (*m_position++).asInt();
}

float JsonDeserializer::getFloat(const char * label)
{
	return static_cast<float>(getDouble(label));
}

double JsonDeserializer::getDouble(const char * label)
{
	return (*m_position++).asDouble();
}

const char * JsonDeserializer::getString(const char * label)
{
	return (*m_position++).asCString();
}

JsonDeserializerNamespace::SubDeserializer::SubDeserializer(const Json::Value & value) :
	m_value(value),
	m_position(m_value.begin())
{
}

JsonDeserializerNamespace::SubDeserializer::operator bool() const
{
	return m_position != m_value.end();
}

std::unique_ptr<Deserializer> JsonDeserializerNamespace::SubDeserializer::enterSegment(const char * label)
{
	return std::unique_ptr<Deserializer>(new SubDeserializer(*m_position++));
}

const char * JsonDeserializerNamespace::SubDeserializer::nextLabel() const
{
	return m_position.memberName();
}

bool JsonDeserializerNamespace::SubDeserializer::getBoolean(const char * label)
{
	return (*m_position++).asBool();
}

uint32 JsonDeserializerNamespace::SubDeserializer::getUint(const char * label)
{
	return (*m_position++).asUInt();
}

int JsonDeserializerNamespace::SubDeserializer::getInt(const char * label)
{
	return (*m_position++).asInt();
}

float JsonDeserializerNamespace::SubDeserializer::getFloat(const char * label)
{
	return static_cast<float>(getDouble(label));
}

double JsonDeserializerNamespace::SubDeserializer::getDouble(const char * label)
{
	return (*m_position++).asDouble();
}

const char * JsonDeserializerNamespace::SubDeserializer::getString(const char * label)
{
	return (*m_position++).asCString();
}

std::unique_ptr<JsonDeserializer> JsonDeserializerNamespace::create(std::istream & stream)
{
	return std::make_unique<JsonDeserializer>(stream);
}
