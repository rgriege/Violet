#include "violet/core/math/Polygon.h"

#include "violet/core/math/Interval.h"
#include "violet/core/math/Matrix2.h"
#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

namespace PolygonNamespace
{
	const char * const ms_segmentLabel("poly");
	const char * const ms_sizeLabel("n");
	const char * const ms_verticesLabel("vertices");

	std::vector<Vec2f> deserializeVertices(Violet::Deserializer & deserializer);
}

using namespace PolygonNamespace;

Polygon::Polygon(std::vector<Vec2f> && vertices) :
	m_vertices(std::move(vertices))
{
}

Polygon::Polygon(Deserializer & deserializer) :
	m_vertices(std::move(deserializeVertices(deserializer)))
{
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

std::vector<Vec2f> PolygonNamespace::deserializeVertices(Deserializer & deserializer)
{
	auto polygonSegment = deserializer.enterSegment(ms_segmentLabel);
	uint32 n = polygonSegment->getUint(ms_sizeLabel);
	std::vector<Vec2f> m_vertices;
	m_vertices.reserve(n);
	auto verticesSegment = polygonSegment->enterSegment(ms_verticesLabel);
	for (uint32 i = 0; i < n; ++i)
	{
		auto vertexSegment = verticesSegment->enterSegment(nullptr);
		m_vertices.emplace_back(*vertexSegment);
	}
	return m_vertices;
}