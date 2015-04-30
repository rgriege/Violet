#include "engine/math/Polygon.h"

#include "engine/math/Interval.h"
#include "engine/math/Matrix2.h"
#include "engine/serialization/SerializationUtilities.h"

using namespace Violet;

namespace PolygonNamespace
{
	const char * const ms_segmentLabel("poly");
}

using namespace PolygonNamespace;

Polygon::Polygon(std::vector<Vec2f> && vertices) :
	m_vertices(std::move(vertices))
{
}

Polygon::Polygon(Deserializer & deserializer) :
	m_vertices()
{
	auto polygonSegment = deserializer.enterSegment(ms_segmentLabel);
	m_vertices.swap(SerializationUtilities::deserializeElements<Vec2f>(*polygonSegment));
}

bool Polygon::contains(const Vec2f & point) const
{
	static const FloatInterval interval(0, 1);
	bool inside = false;
	uint32 const n = m_vertices.size();

	{
		const Vec2f & p1 = m_vertices[n - 1];
		const Vec2f & p2 = m_vertices[0];
		const Vec2f barycentricCoords = Matrix2f(p1.x, p2.x, p1.y, p2.y).inverse() * point;
		inside = interval.contains(barycentricCoords.x) && interval.contains(barycentricCoords.y) && barycentricCoords.x + barycentricCoords.y <= 1;
	}

	uint32 i = 1;
	while (!inside && i < n)
	{
		const Vec2f & p1 = m_vertices[i];
		const Vec2f & p2 = m_vertices[i - 1];
		const Vec2f barycentricCoords = Matrix2f(p1.x, p2.x, p1.y, p2.y).inverse() * point;
		inside = interval.contains(barycentricCoords.x) && interval.contains(barycentricCoords.y) && barycentricCoords.x + barycentricCoords.y <= 1;
		++i;
	}

	return inside;
}

Serializer & Violet::operator<<(Serializer & serializer, const Polygon & poly)
{
	auto segment = serializer.createSegment(ms_segmentLabel);
	SerializationUtilities::serializeElements(*segment, poly.m_vertices);
	return serializer;
}