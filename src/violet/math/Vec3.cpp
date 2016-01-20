// ============================================================================

#include "violet/math/Vec3.h"
#include "violet/serialization/Serializer.h"

// ============================================================================

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec3<int> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeInt("x", vec.x);
	segment->writeInt("y", vec.y);
	segment->writeInt("z", vec.z);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec3<uint32> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeUint("x", vec.x);
	segment->writeUint("y", vec.y);
	segment->writeUint("z", vec.z);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec3<float> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeFloat("x", vec.x);
	segment->writeFloat("y", vec.y);
	segment->writeFloat("z", vec.z);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec3<double> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeDouble("x", vec.x);
	segment->writeDouble("y", vec.y);
	segment->writeDouble("z", vec.z);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec3<int> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getInt("x");
	vec.y = segment->getInt("y");
	vec.z = segment->getInt("z");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec3<uint32> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getUint("x");
	vec.y = segment->getUint("y");
	vec.z = segment->getUint("z");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec3<float> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getFloat("x");
	vec.y = segment->getFloat("y");
	vec.z = segment->getFloat("z");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Violet::Deserializer & Violet::operator>>(Deserializer & deserializer, Vec3<double> & vec)
{
	auto segment = deserializer.enterSegment("vec");
	vec.x = segment->getDouble("x");
	vec.y = segment->getDouble("y");
	vec.z = segment->getDouble("z");
	return deserializer;
}

// ============================================================================
