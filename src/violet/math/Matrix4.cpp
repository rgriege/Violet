// ============================================================================

#include "violet/math/Matrix4.h"

#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"

using namespace Violet;

// ============================================================================

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix4<int> & mat)
{
	auto segment = serializer.createSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			segment->writeInt(label.c_str(), mat[i][j]);
			++label[0];
		}
	}
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix4<uint32> & mat)
{
	auto segment = serializer.createSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			segment->writeUint(label.c_str(), mat[i][j]);
			++label[0];
		}
	}
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix4<float> & mat)
{
	auto segment = serializer.createSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			segment->writeFloat(label.c_str(), mat[i][j]);
			++label[0];
		}
	}
	return serializer;
}

// ----------------------------------------------------------------------------

template<>
Serializer & Violet::operator<<(Serializer & serializer, const Matrix4<double> & mat)
{
	auto segment = serializer.createSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			segment->writeDouble(label.c_str(), mat[i][j]);
			++label[0];
		}
	}
	return serializer;
}

// ----------------------------------------------------------------------------


template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix4<int> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i][j] = segment->getInt(label.c_str());
			++label[0];
		}
	}
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix4<uint32> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i][j] = segment->getUint(label.c_str());
			++label[0];
		}
	}
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix4<float> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i][j] = segment->getFloat(label.c_str());
			++label[0];
		}
	}
	return deserializer;
}

// ----------------------------------------------------------------------------

template<>
Deserializer & Violet::operator>>(Deserializer & deserializer, Matrix4<double> & mat)
{
	auto segment = deserializer.enterSegment("mat");
	std::string label = "a";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i][j] = segment->getDouble(label.c_str());
			++label[0];
		}
	}
	return deserializer;
}

// ============================================================================
