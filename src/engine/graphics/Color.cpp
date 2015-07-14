// ============================================================================

#include "engine/graphics/Color.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

namespace ColorNamespace
{
	const char * const ms_segmentLabel = "color";

	float convert(uint8 value);
	uint8 convert(float value);
}

using namespace ColorNamespace;

// ============================================================================

const Color Color::kBlack(0, 0, 0);
const Color Color::kRed(255, 0, 0);
const Color Color::kOrange(255, 127, 0);
const Color Color::kYellow(255, 255, 0);
const Color Color::kGreen(0, 255, 0);
const Color Color::kIndigo(0, 255, 255);
const Color Color::kBlue(0, 0, 255);
const Color Color::kPurple(140, 0, 255);
const Color Color::kWhite(255, 255, 255);
const Color Color::kTan(240, 230, 170);
const Color Color::kBrown(107, 66, 38);
const Color Color::kRainbow[] = { kRed, kOrange, kYellow, kGreen, kIndigo, kBlue, kPurple };

// ============================================================================

Color::Component::Component() :
	m_value()
{
}

// ----------------------------------------------------------------------------

Color::Component::Component(const uint8 value) :
	m_value(value)
{
}

// ----------------------------------------------------------------------------

Color::Component::Component(const float value) :
	m_value(convert(value))
{
}

// ----------------------------------------------------------------------------

Color::Component & Color::Component::operator=(const uint8 value)
{
	m_value = value;
	return *this;
}

// ----------------------------------------------------------------------------

Color::Component & Color::Component::operator=(const float value)
{
	m_value = convert(value);
	return *this;
}

// ----------------------------------------------------------------------------

bool Color::Component::operator!=(const uint8 value) const
{
	return m_value != value;
}

// ----------------------------------------------------------------------------

Color::Component & Color::Component::operator+=(const uint8 value)
{
	m_value += value;
	return *this;
}

// ----------------------------------------------------------------------------

Color::Component & Color::Component::operator-=(const uint8 value)
{
	m_value -= value;
	return *this;
}

// ----------------------------------------------------------------------------

Color::Component::operator uint8() const
{
	return m_value;
}

// ----------------------------------------------------------------------------

Color::Component::operator float() const
{
	return convert(m_value);
}

// ----------------------------------------------------------------------------

uint8 Color::Component::asUint() const
{
	return m_value;
}

// ----------------------------------------------------------------------------

float Color::Component::asFloat() const
{
	return convert(m_value);
}

// ============================================================================

Color::Color() :
	r(static_cast<uint8>(0)),
	g(static_cast<uint8>(0)),
	b(static_cast<uint8>(0)),
	a(static_cast<uint8>(255))
{
}

// ----------------------------------------------------------------------------

Color::Color(const uint8 _r, const uint8 _g, const uint8 _b, const uint8 _a) :
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

std::array<float, 4> Color::as4fv() const
{
	return { r, g, b, a };
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, Color & color)
{
	const char * hexString = deserializer.getString(ms_segmentLabel);
	uint32 rgba = strtoul(hexString, nullptr, 16);
	color.r = static_cast<uint8>((rgba >> 24) & 0xff);
	color.g = static_cast<uint8>((rgba >> 16) & 0xff);
	color.b = static_cast<uint8>((rgba >> 8) & 0xff);
	color.a = static_cast<uint8>(rgba & 0xff);
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const Color & color)
{
	char hexString[11];
	sprintf(hexString, "0x%.2x%.2x%.2x%.2x", color.r, color.g, color.b, color.a);
	hexString[10] = 0;
	serializer.writeString(ms_segmentLabel, hexString);
	return serializer;
}

// ============================================================================

float ColorNamespace::convert(const uint8 value)
{
	return static_cast<float>(value) / 255.f;
}

// ----------------------------------------------------------------------------

uint8 ColorNamespace::convert(const float value)
{
	return static_cast<uint8>(value * 255);
}

// ============================================================================
