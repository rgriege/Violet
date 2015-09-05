// ============================================================================

#include "violet/math/Matrix3.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix3<int> & mat)
{
	auto segment = serializer.createSegment("mat");
	segment->writeInt("a", mat[0][0]);
	segment->writeInt("b", mat[0][1]);
	segment->writeInt("c", mat[0][2]);
	segment->writeInt("d", mat[1][0]);
	segment->writeInt("e", mat[1][1]);
	segment->writeInt("f", mat[1][2]);
	segment->writeInt("g", mat[2][0]);
	segment->writeInt("h", mat[2][1]);
	segment->writeInt("i", mat[2][2]);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix3<uint32> & mat)
{
	auto segment = serializer.createSegment("mat");
	segment->writeUint("a", mat[0][0]);
	segment->writeUint("b", mat[0][1]);
	segment->writeUint("c", mat[0][2]);
	segment->writeUint("d", mat[1][0]);
	segment->writeUint("e", mat[1][1]);
	segment->writeUint("f", mat[1][2]);
	segment->writeUint("g", mat[2][0]);
	segment->writeUint("h", mat[2][1]);
	segment->writeUint("i", mat[2][2]);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix3<float> & mat)
{
	auto segment = serializer.createSegment("mat");
	segment->writeFloat("a", mat[0][0]);
	segment->writeFloat("b", mat[0][1]);
	segment->writeFloat("c", mat[0][2]);
	segment->writeFloat("d", mat[1][0]);
	segment->writeFloat("e", mat[1][1]);
	segment->writeFloat("f", mat[1][2]);
	segment->writeFloat("g", mat[2][0]);
	segment->writeFloat("h", mat[2][1]);
	segment->writeFloat("i", mat[2][2]);
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix3<double> & mat)
{
	auto segment = serializer.createSegment("mat");
	segment->writeDouble("a", mat[0][0]);
	segment->writeDouble("b", mat[0][1]);
	segment->writeDouble("c", mat[0][2]);
	segment->writeDouble("d", mat[1][0]);
	segment->writeDouble("e", mat[1][1]);
	segment->writeDouble("f", mat[1][2]);
	segment->writeDouble("g", mat[2][0]);
	segment->writeDouble("h", mat[2][1]);
	segment->writeDouble("i", mat[2][2]);
	return serializer;
}

// ----------------------------------------------------------------------------


template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix3<int> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	mat[0][0] = segment->getInt("a");
	mat[0][1] = segment->getInt("b");
	mat[0][2] = segment->getInt("c");
	mat[1][0] = segment->getInt("d");
	mat[1][1] = segment->getInt("e");
	mat[1][2] = segment->getInt("f");
	mat[2][0] = segment->getInt("g");
	mat[2][1] = segment->getInt("h");
	mat[2][2] = segment->getInt("i");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix3<uint32> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	mat[0][0] = segment->getUint("a");
	mat[0][1] = segment->getUint("b");
	mat[0][2] = segment->getUint("c");
	mat[1][0] = segment->getUint("d");
	mat[1][1] = segment->getUint("e");
	mat[1][2] = segment->getUint("f");
	mat[2][0] = segment->getUint("g");
	mat[2][1] = segment->getUint("h");
	mat[2][2] = segment->getUint("i");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix3<float> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	mat[0][0] = segment->getFloat("a");
	mat[0][1] = segment->getFloat("b");
	mat[0][2] = segment->getFloat("c");
	mat[1][0] = segment->getFloat("d");
	mat[1][1] = segment->getFloat("e");
	mat[1][2] = segment->getFloat("f");
	mat[2][0] = segment->getFloat("g");
	mat[2][1] = segment->getFloat("h");
	mat[2][2] = segment->getFloat("i");
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix3<double> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	mat[0][0] = segment->getDouble("a");
	mat[0][1] = segment->getDouble("b");
	mat[0][2] = segment->getDouble("c");
	mat[1][0] = segment->getDouble("d");
	mat[1][1] = segment->getDouble("e");
	mat[1][2] = segment->getDouble("f");
	mat[2][0] = segment->getDouble("g");
	mat[2][1] = segment->getDouble("h");
	mat[2][2] = segment->getDouble("i");
	return deserializer;
}

// ============================================================================
