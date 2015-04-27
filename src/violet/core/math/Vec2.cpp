#include "violet/core/math/Vec2.h"
#include "violet/core/serialization/Serializer.h"

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<int> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeInt("x", vec.x);
	segment->writeInt("y", vec.y);
	return serializer;
}

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<uint32> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeUint("x", vec.x);
	segment->writeUint("y", vec.y);
	return serializer;
}

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<float> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeFloat("x", vec.x);
	segment->writeFloat("y", vec.y);
	return serializer;
}

template<>
Violet::Serializer & Violet::operator<<(Serializer & serializer, const Vec2<double> & vec)
{
	auto segment = serializer.createSegment("vec");
	segment->writeDouble("x", vec.x);
	segment->writeDouble("y", vec.y);
	return serializer;
}
