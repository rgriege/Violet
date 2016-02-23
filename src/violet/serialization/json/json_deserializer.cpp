// ============================================================================

/*
 * NOTE: this uses a modified version of the JsonCpp library and will NOT work with the original source.
 * The deserialization process is designed to be read in the order presented in the input stream. However,
 * raw json does not enforce this. You can find the modified version at https://github.com/rgriege/jsoncpp.
 *
 */

#include <memory>

#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/json/json_deserializer.h"

using namespace vlt;

// ============================================================================

struct Sub_Deserializer : public Deserializer
{
public:

	Sub_Deserializer(const Json::Value & value);

	virtual bool is_valid() const override;

	virtual std::unique_ptr<Deserializer> enter_segment(const char * label) override;
	virtual const char * next_label() const override;

	virtual bool get_b8(const char * label) override;
	virtual u32 get_u32(const char * label) override;
	virtual s32 get_s32(const char * label) override;
	virtual r32 get_r32(const char * label) override;
	virtual r64 get_r64(const char * label) override;
	virtual const char * get_string(const char * label) override;

private:

	const Json::Value & value;
	Json::ValueConstIterator position;
};

// ============================================================================

void Json_Deserializer::install()
{
	File_Deserializer_Factory::instance().assign<Json_Deserializer>("json");
}

// ============================================================================

Json_Deserializer::Json_Deserializer(std::istream & stream) :
	root(),
	position()
{
	Json::Reader reader;
	position = reader.parse(stream, root, false) ? root.begin() : root.end();
}

// ----------------------------------------------------------------------------

Json_Deserializer::Json_Deserializer(Json_Deserializer && other) :
	root(),
	position()
{
	const u32 end = other.position - other.root.begin();

	root = std::move(other.root);
	position = root.begin();
	for (u32 i = 0; i < end; ++i)
		++position;
}

// ----------------------------------------------------------------------------

bool Json_Deserializer::is_valid() const
{
	return position != root.end();
}

// ----------------------------------------------------------------------------

std::unique_ptr<Deserializer> Json_Deserializer::enter_segment(const char * label)
{
	return std::unique_ptr<Deserializer>(new Sub_Deserializer(*position++));
}

// ----------------------------------------------------------------------------

const char * Json_Deserializer::next_label() const
{
	return position.memberName();
}

// ----------------------------------------------------------------------------

bool Json_Deserializer::get_b8(const char * label)
{
	return (*position++).asBool();
}

// ----------------------------------------------------------------------------

u32 Json_Deserializer::get_u32(const char * label)
{
	return (*position++).asUInt();
}

// ----------------------------------------------------------------------------

int Json_Deserializer::get_s32(const char * label)
{
	return (*position++).asInt();
}

// ----------------------------------------------------------------------------

r32 Json_Deserializer::get_r32(const char * label)
{
	return static_cast<r32>(get_r64(label));
}

// ----------------------------------------------------------------------------

r64 Json_Deserializer::get_r64(const char * label)
{
	return (*position++).asDouble();
}

// ----------------------------------------------------------------------------

const char * Json_Deserializer::get_string(const char * label)
{
	return (*position++).asCString();
}

// ============================================================================

Sub_Deserializer::Sub_Deserializer(const Json::Value & _value) :
	value(_value),
	position(value.begin())
{
}

// ----------------------------------------------------------------------------

bool Sub_Deserializer::is_valid() const
{
	return position != value.end();
}

// ----------------------------------------------------------------------------

std::unique_ptr<Deserializer> Sub_Deserializer::enter_segment(const char * label)
{
	return std::unique_ptr<Deserializer>(new Sub_Deserializer(*position++));
}

// ----------------------------------------------------------------------------

const char * Sub_Deserializer::next_label() const
{
	return position.memberName();
}

// ----------------------------------------------------------------------------

bool Sub_Deserializer::get_b8(const char * label)
{
	return (*position++).asBool();
}

// ----------------------------------------------------------------------------

u32 Sub_Deserializer::get_u32(const char * label)
{
	return (*position++).asUInt();
}

// ----------------------------------------------------------------------------

int Sub_Deserializer::get_s32(const char * label)
{
	return (*position++).asInt();
}

// ----------------------------------------------------------------------------

r32 Sub_Deserializer::get_r32(const char * label)
{
	return static_cast<r32>(get_r64(label));
}

// ----------------------------------------------------------------------------

r64 Sub_Deserializer::get_r64(const char * label)
{
	return (*position++).asDouble();
}

// ----------------------------------------------------------------------------

const char * Sub_Deserializer::get_string(const char * label)
{
	return (*position++).asCString();
}

// ============================================================================
