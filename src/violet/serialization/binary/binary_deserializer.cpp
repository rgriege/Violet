// ============================================================================

#include "violet/serialization/binary/binary_deserializer.h"

#include "violet/serialization/file/file_deserializer_factory.h"

using namespace vlt;

// ============================================================================

template <typename T>
T get_value(std::istream & stream);

// ============================================================================

void binary_deserializer::install()
{
	 file_deserializer_factory::instance().assign<binary_deserializer>("vba");
}

// ============================================================================

binary_deserializer::binary_deserializer(std::istream & stream) :
	stream(stream),
	string_buffer()
{
}

// ----------------------------------------------------------------------------

bool binary_deserializer::is_valid() const
{
	// error with implicitly/explicitly calling operator bool()
	return static_cast<bool>(stream);
}

// ----------------------------------------------------------------------------

std::unique_ptr<deserializer> binary_deserializer::enter_segment(const char * /*label*/)
{
	return std::unique_ptr<deserializer>(new binary_deserializer(stream));
}

// ----------------------------------------------------------------------------

const char * binary_deserializer::next_label() const
{
	return "";
}

// ----------------------------------------------------------------------------

bool binary_deserializer::get_b8(const char * /*label*/)
{
	return get_value<bool>(stream);
}

// ----------------------------------------------------------------------------

u32 binary_deserializer::get_u32(const char * /*label*/)
{
	return get_value<u32>(stream);
}

// ----------------------------------------------------------------------------

s32 binary_deserializer::get_s32(const char * /*label*/)
{
	return get_value<int>(stream);
}

// ----------------------------------------------------------------------------

r32 binary_deserializer::get_r32(const char * /*label*/)
{
	return get_value<r32>(stream);
}

// ----------------------------------------------------------------------------

r64 binary_deserializer::get_r64(const char * /*r64*/)
{
	return get_value<r64>(stream);
}

// ----------------------------------------------------------------------------

const char * binary_deserializer::get_string(const char * /*label*/)
{
	string_buffer.clear();
	char c = '\0';
	do
	{
		stream >> c;
		string_buffer += c;
	}
	while (c != '\0');
	return string_buffer.c_str();
}

// ============================================================================

template <typename T>
T get_value(std::istream & stream)
{
	T value = T();
	stream >> value;
	return value;
}

// ============================================================================
