// ============================================================================

/*
 * NOTE: this uses a modified version of the JsonCpp library and will NOT work with the original source.
 * The deserialization process is designed to be read in the order presented in the input stream. However,
 * raw json does not enforce this. You can find the modified version at https://github.com/rgriege/jsoncpp.
 *
 */

#include "engine/serialization/json/JsonSerializer.h"

#include "engine/serialization/file/FileSerializerFactory.h"

using namespace Violet;

// ============================================================================

namespace JsonSerializerNamespace
{
	class SubSerializer : public Serializer
	{
	public:

		SubSerializer(Json::Value & value);

		virtual unique_val<Serializer> createSegment(const char * label) override;

		virtual void writeBoolean(const char * label, bool value) override;
		virtual void writeUint(const char * label, uint32 value) override;
		virtual void writeInt(const char * label, int value) override;
		virtual void writeFloat(const char * label, float value) override;
		virtual void writeDouble(const char * label, double value) override;
		virtual void writeString(const char * label, const char * value) override;

	private:

		Json::Value & m_value;
	};
}

using namespace JsonSerializerNamespace;

// ============================================================================

void JsonSerializer::install()
{
	FileSerializerFactory::getInstance().assign<JsonSerializer>("json");
}

// ============================================================================

JsonSerializer::JsonSerializer(std::ostream & stream) :
	m_stream(stream),
	m_value()
{
}

// ----------------------------------------------------------------------------

JsonSerializer::JsonSerializer(JsonSerializer && other) :
	m_stream(other.m_stream),
	m_value(other.m_value)
{
}

// ----------------------------------------------------------------------------

JsonSerializer::~JsonSerializer()
{
	Json::StyledStreamWriter writer;
	writer.write(m_stream, m_value);
}

// ----------------------------------------------------------------------------

unique_val<Serializer> JsonSerializer::createSegment(const char * label)
{
	return make_unique_val<SubSerializer>(m_value.append(label));
}

// ----------------------------------------------------------------------------

void JsonSerializer::writeBoolean(const char * label, const bool value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void JsonSerializer::writeUint(const char * label, const uint32 value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void JsonSerializer::writeInt(const char * label, const int value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void JsonSerializer::writeFloat(const char * label, const float value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void JsonSerializer::writeDouble(const char * label, const double value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void JsonSerializer::writeString(const char * label, const char * value)
{
	m_value.append(label, value);
}

// ============================================================================

SubSerializer::SubSerializer(Json::Value & value) :
	m_value(value)
{
}

// ----------------------------------------------------------------------------

unique_val<Serializer> SubSerializer::createSegment(const char * label)
{
	return make_unique_val<SubSerializer>(m_value.append(label));
}

// ----------------------------------------------------------------------------

void SubSerializer::writeBoolean(const char * label, bool value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void SubSerializer::writeUint(const char * label, uint32 value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void SubSerializer::writeInt(const char * label, int value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void SubSerializer::writeFloat(const char * label, float value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void SubSerializer::writeDouble(const char * label, double value)
{
	m_value.append(label, value);
}

// ----------------------------------------------------------------------------

void SubSerializer::writeString(const char * label, const char * value)
{
	m_value.append(label, value);
}

// ============================================================================
