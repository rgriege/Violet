// ============================================================================

#include "violet/graphics/color.h"

#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"

using namespace vlt;

// ============================================================================

namespace ColorNamespace
{
	const char * const ms_segmentLabel = "color";

	r32 convert(u8 value);
	u8 convert(r32 value);

	void deserializeFromHexString(color & color, const char * hexString);
}

using namespace ColorNamespace;

// ============================================================================

const color color::Black(0, 0, 0);
const color color::Red(255, 0, 0);
const color color::Orange(255, 127, 0);
const color color::Yellow(255, 255, 0);
const color color::Green(0, 255, 0);
const color color::Indigo(0, 255, 255);
const color color::Blue(0, 0, 255);
const color color::Purple(140, 0, 255);
const color color::White(255, 255, 255);
const color color::Tan(240, 230, 170);
const color color::Brown(107, 66, 38);
const color color::Rainbow[] = { Red, Orange, Yellow, Green, Indigo, Blue, Purple };

// ============================================================================

color::component::component() :
	value()
{
}

// ----------------------------------------------------------------------------

color::component::component(const u8 value) :
	value(value)
{
}

// ----------------------------------------------------------------------------

color::component::component(const r32 value) :
	value(convert(value))
{
}

// ----------------------------------------------------------------------------

color::component & color::component::operator=(const u8 _value)
{
	value = _value;
	return *this;
}

// ----------------------------------------------------------------------------

color::component & color::component::operator=(const r32 _value)
{
	value = convert(_value);
	return *this;
}

// ----------------------------------------------------------------------------

bool color::component::operator!=(const u8 _value) const
{
	return value != _value;
}

// ----------------------------------------------------------------------------

color::component & color::component::operator+=(const u8 _value)
{
	value += _value;
	return *this;
}

// ----------------------------------------------------------------------------

color::component & color::component::operator-=(const u8 _value)
{
	value -= _value;
	return *this;
}

// ----------------------------------------------------------------------------

color::component::operator u8() const
{
	return value;
}

// ----------------------------------------------------------------------------

color::component::operator r32() const
{
	return convert(value);
}

// ----------------------------------------------------------------------------

u8 color::component::asUint() const
{
	return value;
}

// ----------------------------------------------------------------------------

r32 color::component::asFloat() const
{
	return convert(value);
}

// ============================================================================

color::color() :
	r(static_cast<u8>(0)),
	g(static_cast<u8>(0)),
	b(static_cast<u8>(0)),
	a(static_cast<u8>(255))
{
}

// ----------------------------------------------------------------------------

color::color(const u8 _r, const u8 _g, const u8 _b, const u8 _a) :
	r(_r),
	g(_g),
	b(_b),
	a(_a)
{
}

// ----------------------------------------------------------------------------

color::color(deserializer & deserializer) :
	color()
{
	deserializer >> *this;
}

// ----------------------------------------------------------------------------

color::color(const char * const hexString) :
	color()
{
	deserializeFromHexString(*this, hexString);
}

// ----------------------------------------------------------------------------

bool color::operator!=(const color & rhs) const
{
	return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
}

// ----------------------------------------------------------------------------

std::array<r32, 4> color::as4fv() const
{
	return { r, g, b, a };
}

// ============================================================================

deserializer & vlt::operator>>(deserializer & deserializer, color & color)
{
	const char * hexString = deserializer.get_string(ms_segmentLabel);
	deserializeFromHexString(color, hexString);
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const color & color)
{
	char hexString[11];
	sprintf(hexString, "0x%.2x%.2x%.2x%.2x", color.r.asUint(), color.g.asUint(), color.b.asUint(), color.a.asUint());
	hexString[10] = 0;
	serializer.write_string(ms_segmentLabel, hexString);
	return serializer;
}

// ============================================================================

r32 ColorNamespace::convert(const u8 value)
{
	return static_cast<r32>(value) / 255.f;
}

// ----------------------------------------------------------------------------

u8 ColorNamespace::convert(const r32 value)
{
	return static_cast<u8>(value * 255);
}

// ----------------------------------------------------------------------------

void ColorNamespace::deserializeFromHexString(color & color, const char * const hexString)
{
	u32 rgba = strtoul(hexString, nullptr, 16);
	color.r = static_cast<u8>((rgba >> 24) & 0xff);
	color.g = static_cast<u8>((rgba >> 16) & 0xff);
	color.b = static_cast<u8>((rgba >> 8) & 0xff);
	color.a = static_cast<u8>(rgba & 0xff);
}

// ============================================================================
