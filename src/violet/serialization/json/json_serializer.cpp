// ============================================================================

/*
 * NOTE: this uses a modified version of the JsonCpp library and will NOT work with the original source.
 * The deserialization process is designed to be read in the order presented in the input stream. However,
 * raw json does not enforce this. You can find the modified version at https://github.com/rgriege/jsoncpp.
 *
 */

#include "violet/serialization/file/file_serializer_factory.h"
#include "violet/serialization/json/json_serializer.h"

using namespace vlt;

// ============================================================================

struct Sub_Serializer final : public Serializer
{
	Json::Value & value;

	Sub_Serializer(Json::Value & value);

	virtual unique_val<Serializer> create_segment(const char * label) override;

	virtual void write_b8(const char * label, b8 value) override;
	virtual void write_u32(const char * label, u32 value) override;
	virtual void write_s32(const char * label, int value) override;
	virtual void write_r32(const char * label, r32 value) override;
	virtual void write_r64(const char * label, r64 value) override;
	virtual void write_string(const char * label, const char * value) override;
};

// ============================================================================

void Json_Serializer::install()
{
	File_Serializer_Factory::instance().assign<Json_Serializer>("json");
}

// ============================================================================

Json_Serializer::Json_Serializer(std::ostream & _stream) :
	stream(_stream),
	value()
{
}

// ----------------------------------------------------------------------------

Json_Serializer::Json_Serializer(Json_Serializer && other) :
	stream(other.stream),
	value(std::move(other.value))
{
}

// ----------------------------------------------------------------------------

Json_Serializer::~Json_Serializer()
{
	Json::StyledStreamWriter writer;
	writer.write(stream, value);
}

// ----------------------------------------------------------------------------

unique_val<Serializer> Json_Serializer::create_segment(const char * label)
{
	return make_unique_val<Sub_Serializer>(value.append(label));
}

// ----------------------------------------------------------------------------

void Json_Serializer::write_b8(const char * label, const b8 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Json_Serializer::write_u32(const char * label, const u32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Json_Serializer::write_s32(const char * label, const int v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Json_Serializer::write_r32(const char * label, const r32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Json_Serializer::write_r64(const char * label, const r64 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Json_Serializer::write_string(const char * label, const char * v)
{
	value.append(label, v);
}

// ============================================================================

Sub_Serializer::Sub_Serializer(Json::Value & _value) :
	value(_value)
{
}

// ----------------------------------------------------------------------------

unique_val<Serializer> Sub_Serializer::create_segment(const char * label)
{
	return make_unique_val<Sub_Serializer>(value.append(label));
}

// ----------------------------------------------------------------------------

void Sub_Serializer::write_b8(const char * label, const b8 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Sub_Serializer::write_u32(const char * label, const u32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Sub_Serializer::write_s32(const char * label, const int v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Sub_Serializer::write_r32(const char * label, const r32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Sub_Serializer::write_r64(const char * label, const r64 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void Sub_Serializer::write_string(const char * label, const char * const v)
{
	value.append(label, v);
}

// ============================================================================
