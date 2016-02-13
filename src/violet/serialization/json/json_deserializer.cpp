// ============================================================================

/*
 * NOTE: this uses a modified version of the JsonCpp library and will NOT work with the original source.
 * The deserialization process is designed to be read in the order presented in the input stream. However,
 * raw json does not enforce this. You can find the modified version at https://github.com/rgriege/jsoncpp.
 *
 */

#include <memory>

#include "violet/serialization/json/json_deserializer.h"
#include "violet/serialization/file/file_deserializer_factory.h"

using namespace vlt;

// ============================================================================

struct sub_deserializer : public deserializer
{
public:

	sub_deserializer(const Json::Value & value);

	virtual bool is_valid() const override;

	virtual std::unique_ptr<deserializer> enter_segment(const char * label) override;
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

void json_deserializer::install()
{
	file_deserializer_factory::instance().assign<json_deserializer>("json");
}

// ============================================================================

json_deserializer::json_deserializer(std::istream & stream) :
	root(),
	position()
{
	Json::Reader reader;
	position = reader.parse(stream, root, false) ? root.begin() : root.end();
}

// ----------------------------------------------------------------------------

json_deserializer::json_deserializer(json_deserializer && other) :
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

bool json_deserializer::is_valid() const
{
	return position != root.end();
}

// ----------------------------------------------------------------------------

std::unique_ptr<deserializer> json_deserializer::enter_segment(const char * label)
{
	return std::unique_ptr<deserializer>(new sub_deserializer(*position++));
}

// ----------------------------------------------------------------------------

const char * json_deserializer::next_label() const
{
	return position.memberName();
}

// ----------------------------------------------------------------------------

bool json_deserializer::get_b8(const char * label)
{
	return (*position++).asBool();
}

// ----------------------------------------------------------------------------

u32 json_deserializer::get_u32(const char * label)
{
	return (*position++).asUInt();
}

// ----------------------------------------------------------------------------

int json_deserializer::get_s32(const char * label)
{
	return (*position++).asInt();
}

// ----------------------------------------------------------------------------

r32 json_deserializer::get_r32(const char * label)
{
	return static_cast<r32>(get_r64(label));
}

// ----------------------------------------------------------------------------

r64 json_deserializer::get_r64(const char * label)
{
	return (*position++).asDouble();
}

// ----------------------------------------------------------------------------

const char * json_deserializer::get_string(const char * label)
{
	return (*position++).asCString();
}

// ============================================================================

sub_deserializer::sub_deserializer(const Json::Value & value) :
	value(value),
	position(value.begin())
{
}

// ----------------------------------------------------------------------------

bool sub_deserializer::is_valid() const
{
	return position != value.end();
}

// ----------------------------------------------------------------------------

std::unique_ptr<deserializer> sub_deserializer::enter_segment(const char * label)
{
	return std::unique_ptr<deserializer>(new sub_deserializer(*position++));
}

// ----------------------------------------------------------------------------

const char * sub_deserializer::next_label() const
{
	return position.memberName();
}

// ----------------------------------------------------------------------------

bool sub_deserializer::get_b8(const char * label)
{
	return (*position++).asBool();
}

// ----------------------------------------------------------------------------

u32 sub_deserializer::get_u32(const char * label)
{
	return (*position++).asUInt();
}

// ----------------------------------------------------------------------------

int sub_deserializer::get_s32(const char * label)
{
	return (*position++).asInt();
}

// ----------------------------------------------------------------------------

r32 sub_deserializer::get_r32(const char * label)
{
	return static_cast<r32>(get_r64(label));
}

// ----------------------------------------------------------------------------

r64 sub_deserializer::get_r64(const char * label)
{
	return (*position++).asDouble();
}

// ----------------------------------------------------------------------------

const char * sub_deserializer::get_string(const char * label)
{
	return (*position++).asCString();
}

// ============================================================================
