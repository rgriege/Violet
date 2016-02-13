// ============================================================================

#include "violet/serialization/binary/binary_serializer.h"

#include "violet/serialization/file/file_serializer_factory.h"

using namespace vlt;

// ============================================================================

template <typename T>
void write_value(std::ostream & stream, T value);

// ============================================================================

void binary_serializer::install()
{
	file_serializer_factory::instance().assign<binary_serializer>("vba");
}

// ============================================================================

binary_serializer::binary_serializer(std::ostream & stream) :
	stream(stream)
{
}

// ----------------------------------------------------------------------------

unique_val<serializer> binary_serializer::create_segment(const char * /*label*/)
{
	return make_unique_val<binary_serializer>(stream);
}

// ----------------------------------------------------------------------------

void binary_serializer::write_b8(const char * /*label*/, bool const value)
{
	write_value(stream, value);
}

// ----------------------------------------------------------------------------

void binary_serializer::write_u32(const char * /*label*/, u32 const value)
{
	write_value(stream, value);
}

// ----------------------------------------------------------------------------

void binary_serializer::write_s32(const char * /*label*/, int const value)
{
	write_value(stream, value);
}

// ----------------------------------------------------------------------------

void binary_serializer::write_r32(const char * /*label*/, r32 const value)
{
	write_value(stream, value);
}

// ----------------------------------------------------------------------------

void binary_serializer::write_r64(const char * /*r64*/, r64 const value)
{
	write_value(stream, value);
}

// ----------------------------------------------------------------------------

void binary_serializer::write_string(const char * /*label*/, const char * value)
{
	stream << value << '\0';
}

// ============================================================================

template <typename T>
void write_value(std::ostream & stream, T const value)
{
	stream << value << ' ';
}

// ============================================================================
