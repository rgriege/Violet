// ============================================================================

#include "violet/math/Polygon.h"

#include "violet/math/Interval.h"
#include "violet/math/Matrix2.h"
#include "violet/serialization/SerializationUtilities.h"

using namespace Violet;

// ============================================================================

namespace PolygonNamespace
{
	const char * const ms_segmentLabel("poly");
}

using namespace PolygonNamespace;

// ============================================================================

Polygon::Polygon(std::vector<Vec2f> && vertices) :
	m_vertices(std::move(vertices))
{
}

// ----------------------------------------------------------------------------

Polygon::Polygon(Deserializer & deserializer) :
	m_vertices()
{
	auto polygonSegment = deserializer.enterSegment(ms_segmentLabel);
	m_vertices = SerializationUtilities::deserializeElements<Vec2f>(*polygonSegment);
}

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

AABB Polygon::getBoundingBox() const
{
	AABB result;
	for (auto const & vertex : m_vertices)
		result.extend(vertex);
	return result;
}

// ----------------------------------------------------------------------------

FloatInterval Polygon::project(const Vec2f & axis) const
{
	const Vec2f & unitAxis = axis.isUnit() ? axis : axis.getUnit();
	FloatInterval projection;
	for (const auto & vertex : m_vertices)
	{
		const float dp = vertex.dot(unitAxis);
		if (dp < projection.m_left)
			projection.m_left = dp;
		else if (dp > projection.m_right)
			projection.m_right = dp;
	}
	return projection;
}

// ----------------------------------------------------------------------------

Vec2f Polygon::getCenter() const
{
	Vec2f center;
	for (auto const & vertex : m_vertices)
		center += vertex;
	return center / static_cast<float>(m_vertices.size());
}

// ----------------------------------------------------------------------------

Vec2f & Polygon::operator[](uint32 const index)
{
	return m_vertices[index];
}

// ----------------------------------------------------------------------------

const Vec2f & Polygon::operator[](uint32 const index) const
{
	return m_vertices[index];
}

// ----------------------------------------------------------------------------

Polygon::Vertices::iterator Polygon::begin()
{
	return m_vertices.begin();
}

// ----------------------------------------------------------------------------

Polygon::Vertices::iterator Polygon::end()
{
	return m_vertices.end();
}

// ============================================================================

Serializer & Violet::operator<<(Serializer & serializer, const Polygon & poly)
{
	auto segment = serializer.createSegment(ms_segmentLabel);
	SerializationUtilities::serializeElements(*segment, poly.m_vertices);
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer & Violet::operator>>(Deserializer & deserializer, Polygon & poly)
{
	auto segment = deserializer.enterSegment(ms_segmentLabel);
	poly.m_vertices = SerializationUtilities::deserializeElements<Vec2f>(*segment);
	return deserializer;
}

// ============================================================================
