// ============================================================================

/*
 * NOTE: this uses a modified version of the JsonCpp library and will NOT work with the original source.
 * The deserialization process is designed to be read in the order presented in the input stream. However,
 * raw json does not enforce this. You can find the modified version at https://github.com/rgriege/jsoncpp.
 *
 */

#include "violet/serialization/json/json_serializer.h"

#include "violet/serialization/file/file_serializer_factory.h"

using namespace vlt;

// ============================================================================

struct sub_serializer final : public serializer
{
	Json::Value & value;

	sub_serializer(Json::Value & value);

	virtual unique_val<serializer> create_segment(const char * label) override;

	virtual void write_b8(const char * label, b8 value) override;
	virtual void write_u32(const char * label, u32 value) override;
	virtual void write_s32(const char * label, int value) override;
	virtual void write_r32(const char * label, r32 value) override;
	virtual void write_r64(const char * label, r64 value) override;
	virtual void write_string(const char * label, const char * value) override;
};

// ============================================================================

void json_serializer::install()
{
	file_serializer_factory::instance().assign<json_serializer>("json");
}

// ============================================================================

json_serializer::json_serializer(std::ostream & stream) :
	stream(stream),
	value()
{
}

// ----------------------------------------------------------------------------

json_serializer::json_serializer(json_serializer && other) :
	stream(std::move(other.stream)),
	value(std::move(other.value))
{
}

// ----------------------------------------------------------------------------

json_serializer::~json_serializer()
{
	Json::StyledStreamWriter writer;
	writer.write(stream, value);
}

// ----------------------------------------------------------------------------

unique_val<serializer> json_serializer::create_segment(const char * label)
{
	return make_unique_val<sub_serializer>(value.append(label));
}

// ----------------------------------------------------------------------------

void json_serializer::write_b8(const char * label, const b8 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void json_serializer::write_u32(const char * label, const u32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void json_serializer::write_s32(const char * label, const int v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void json_serializer::write_r32(const char * label, const r32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void json_serializer::write_r64(const char * label, const r64 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void json_serializer::write_string(const char * label, const char * v)
{
	value.append(label, v);
}

// ============================================================================

sub_serializer::sub_serializer(Json::Value & value) :
	value(value)
{
}

// ----------------------------------------------------------------------------

unique_val<serializer> sub_serializer::create_segment(const char * label)
{
	return make_unique_val<sub_serializer>(value.append(label));
}

// ----------------------------------------------------------------------------

void sub_serializer::write_b8(const char * label, const b8 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void sub_serializer::write_u32(const char * label, const u32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void sub_serializer::write_s32(const char * label, const int v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void sub_serializer::write_r32(const char * label, const r32 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void sub_serializer::write_r64(const char * label, const r64 v)
{
	value.append(label, v);
}

// ----------------------------------------------------------------------------

void sub_serializer::write_string(const char * label, const char * const v)
{
	value.append(label, v);
}

// ============================================================================
