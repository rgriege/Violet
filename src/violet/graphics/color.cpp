// ============================================================================

#include "violet/graphics/color.h"

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

namespace Color_Namespace
{
	r32 convert(u8 value);
	u8 convert(r32 value);

	void deserialize_from_hex_string(Color & Color, const char * hex_string);
}

using namespace Color_Namespace;

// ============================================================================

const Color Color::Black(0, 0, 0);
const Color Color::Red(255, 0, 0);
const Color Color::Orange(255, 127, 0);
const Color Color::Yellow(255, 255, 0);
const Color Color::Green(0, 255, 0);
const Color Color::Indigo(0, 255, 255);
const Color Color::Blue(0, 0, 255);
const Color Color::Purple(140, 0, 255);
const Color Color::White(255, 255, 255);
const Color Color::Tan(240, 230, 170);
const Color Color::Brown(107, 66, 38);
const Color Color::Rainbow[] = { Red, Orange, Yellow, Green, Indigo, Blue, Purple };

// ============================================================================

Color::component::component() :
	value()
{
}

// ----------------------------------------------------------------------------

Color::component::component(const u8 value) :
	value(value)
{
}

// ----------------------------------------------------------------------------

Color::component::component(const r32 value) :
	value(convert(value))
{
}

// ----------------------------------------------------------------------------

Color::component & Color::component::operator=(const u8 _value)
{
	value = _value;
	return *this;
}

// ----------------------------------------------------------------------------

Color::component & Color::component::operator=(const r32 _value)
{
	value = convert(_value);
	return *this;
}

// ----------------------------------------------------------------------------

bool Color::component::operator!=(const u8 _value) const
{
	return value != _value;
}

// ----------------------------------------------------------------------------

Color::component & Color::component::operator+=(const u8 _value)
{
	value += _value;
	return *this;
}

// ----------------------------------------------------------------------------

Color::component & Color::component::operator-=(const u8 _value)
{
	value -= _value;
	return *this;
}

// ----------------------------------------------------------------------------

Color::component::operator u8() const
{
	return value;
}

// ----------------------------------------------------------------------------

Color::component::operator r32() const
{
	return convert(value);
}

// ----------------------------------------------------------------------------

u8 Color::component::as_uint() const
{
	return value;
}

// ----------------------------------------------------------------------------

r32 Color::component::as_float() const
{
	return convert(value);
}

// ============================================================================

Color::Color() :
	r(static_cast<u8>(0)),
	g(static_cast<u8>(0)),
	b(static_cast<u8>(0)),
	a(static_cast<u8>(255))
{
}

// ----------------------------------------------------------------------------

Color::Color(const u8 _r, const u8 _g, const u8 _b, const u8 _a) :
	r(_r),
	g(_g),
	b(_b),
	a(_a)
{
}

// ----------------------------------------------------------------------------

Color::Color(Deserializer & deserializer) :
	Color()
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

Color::Color(const char * const hex_string) :
	Color()
{
	deserialize_from_hex_string(*this, hex_string);
}

// ----------------------------------------------------------------------------

bool Color::operator!=(const Color & rhs) const
{
	return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
}

// ----------------------------------------------------------------------------

std::array<r32, 4> Color::as4fv() const
{
	return { r, g, b, a };
}

// ============================================================================

Deserializer & vlt::operator>>(Deserializer & deserializer, Color & color)
{
	const char * hex_string = deserializer.get_string("color");
	deserialize_from_hex_string(color, hex_string);
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Color & color)
{
	char hex_string[11];
	sprintf(hex_string, "0x%.2x%.2x%.2x%.2x", color.r.as_uint(), color.g.as_uint(), color.b.as_uint(), color.a.as_uint());
	hex_string[10] = 0;
	serializer.write_string("color", hex_string);
	return serializer;
}

// ============================================================================

r32 Color_Namespace::convert(const u8 value)
{
	return static_cast<r32>(value) / 255.f;
}

// ----------------------------------------------------------------------------

u8 Color_Namespace::convert(const r32 value)
{
	return static_cast<u8>(value * 255);
}

// ----------------------------------------------------------------------------

void Color_Namespace::deserialize_from_hex_string(Color & Color, const char * const hex_string)
{
	u32 rgba = strtoul(hex_string, nullptr, 16);
	Color.r = static_cast<u8>((rgba >> 24) & 0xff);
	Color.g = static_cast<u8>((rgba >> 16) & 0xff);
	Color.b = static_cast<u8>((rgba >> 8) & 0xff);
	Color.a = static_cast<u8>(rgba & 0xff);
}

// ============================================================================
