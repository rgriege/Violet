// ============================================================================

#include "violet/serialization/binary/binary_deserializer.h"

#include "violet/serialization/file/file_deserializer_factory.h"

using namespace vlt;

// ============================================================================

template <typename T>
T get_value(std::istream & stream);

// ============================================================================

void Binary_Deserializer::install()
{
	 File_Deserializer_Factory::instance().assign<Binary_Deserializer>("vba");
}

// ============================================================================

Binary_Deserializer::Binary_Deserializer(std::istream & stream) :
	stream(stream),
	string_buffer()
{
}

// ----------------------------------------------------------------------------

bool Binary_Deserializer::is_valid() const
{
	// error with implicitly/explicitly calling operator bool()
	return static_cast<bool>(stream);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Deserializer> Binary_Deserializer::enter_segment(const char * /*label*/)
{
	return std::unique_ptr<Deserializer>(new Binary_Deserializer(stream));
}

// ----------------------------------------------------------------------------

const char * Binary_Deserializer::next_label() const
{
	return "";
}

// ----------------------------------------------------------------------------

bool Binary_Deserializer::get_b8(const char * /*label*/)
{
	return get_value<bool>(stream);
}

// ----------------------------------------------------------------------------

u32 Binary_Deserializer::get_u32(const char * /*label*/)
{
	return get_value<u32>(stream);
}

// ----------------------------------------------------------------------------

s32 Binary_Deserializer::get_s32(const char * /*label*/)
{
	return get_value<int>(stream);
}

// ----------------------------------------------------------------------------

r32 Binary_Deserializer::get_r32(const char * /*label*/)
{
	return get_value<r32>(stream);
}

// ----------------------------------------------------------------------------

r64 Binary_Deserializer::get_r64(const char * /*r64*/)
{
	return get_value<r64>(stream);
}

// ----------------------------------------------------------------------------

const char * Binary_Deserializer::get_string(const char * /*label*/)
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
