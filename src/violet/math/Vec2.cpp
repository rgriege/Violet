// ============================================================================

#include "violet/math/Vec2.h"
#include "violet/serialization/Serializer.h"

// ============================================================================

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<int> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeInt("x", vec.x);
	segment->writeInt("y", vec.y);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<uint32> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeUint("x", vec.x);
	segment->writeUint("y", vec.y);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<float> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeFloat("x", vec.x);
	segment->writeFloat("y", vec.y);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<double> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeDouble("x", vec.x);
	segment->writeDouble("y", vec.y);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec2<int> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getInt("x");
	vec.y = segment->getInt("y");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec2<uint32> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getUint("x");
	vec.y = segment->getUint("y");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec2<float> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getFloat("x");
	vec.y = segment->getFloat("y");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec2<double> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getDouble("x");
	vec.y = segment->getDouble("y");
	return deserializer;
}

// ============================================================================
