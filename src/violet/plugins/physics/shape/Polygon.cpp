#include "violet/plugins/physics/shape/Polygon.h"

using namespace Violet;

namespace PolygonNamespace
{
	const char * const ms_segmentLabel("poly");
	const char * const ms_sizeLabel("n");
	const char * const ms_verticesLabel("vertices");

	Vec2f centralize(std::vector<Vec2f> & vertices);
}

using namespace PolygonNamespace;

Polygon::Polygon(const float mass, std::vector<Vec2f> vertices) :
	Shape(std::move(centralize(vertices)), mass, Vec2f::ZERO),
	m_vertices(std::move(vertices))
{
}

Polygon::Polygon(Deserializer & deserializer) :
	Shape(deserializer),
	m_vertices()
{
	deserializer.enterSegment(PolygonNamespace::ms_segmentLabel);
	uint32 const size = deserializer.getUint(ms_sizeLabel);
	m_vertices.reserve(size);
	deserializer.enterSegment(ms_verticesLabel);
	for (uint32 i = 0; i < size; ++i)
	{
		deserializer.enterSegment(nullptr);
		m_vertices.emplace_back(deserializer);
		deserializer.leaveSegment();
	}
	deserializer.leaveSegment();
	deserializer.leaveSegment();
}

void Polygon::rotate(const float radians)
{
	for(auto & vertex : m_vertices)
		vertex.rotate(radians);
}

float Polygon::calculateMomentOfInertia() const
{
	float area = 0;
	float numerator = 0;
	float denominator = 0;
	const uint32 size = m_vertices.size();
	for (uint32 i = 0; i < size; ++i)
	{
		const Vec2f & vertex = m_vertices[i];
		const Vec2f & nextVertex = m_vertices[i < size - 1 ? i + 1 : 0];
		Vec2f perp = vertex.perpendicular();
		const float height = abs(nextVertex.dot(perp));
		const float squareArea = height * vertex.magnitude();
		area += 0.5f * squareArea;
		numerator += squareArea*(nextVertex.magSquared() + nextVertex.dot(vertex) + vertex.magSquared());
		denominator += squareArea;
	}
	return (m_mass / 6) * (numerator / denominator);
}

FloatInterval Polygon::project(const Vec2f & axis) const
{
	const Vec2f & unitAxis = axis.isUnit() ? axis : axis.getUnit();
	FloatInterval projection;
	for (const auto & vertex : m_vertices)
	{
		const float dp = vertex.dot(unitAxis);
		if (dp < projection.left)
			projection.left = dp;
		else if (dp > projection.right)
			projection.right = dp;
	}
	const float dp = m_center.dot(unitAxis);
	projection.left += dp;
	projection.right += dp;
	return projection;
}

float Polygon::maxRadius() const
{
	float result = 0;
	for (const auto & vertex : m_vertices)
	{
		const float dist = (vertex + m_center).magSquared();
		if (dist > result)
			result = dist;
	}
	return sqrt(result);
}

void Polygon::findIntersectionAxes(const Shape & /*other*/, std::vector<Vec2f> & axes) const
{
	axes.emplace_back((m_vertices.front() - m_vertices.back()).perpendicular());
	for (uint32 i = 1, end = m_vertices.size(); i < end; ++i)
	{
		const Vec2f side = m_vertices[i] - m_vertices[i - 1];
		axes.push_back(side.perpendicular());
	}
}

std::unique_ptr<Shape> Polygon::copy() const
{
	return std::unique_ptr<Shape>(new Polygon(*this));
}

// TODO: fails if any vertices are past the goal
void Polygon::findClosestVertices(const Vec2f & axis, const Vec2f & goal, std::vector<Vec2f> & vertices) const
{
	const Vec2f localGoal = goal - m_center;
	float closestDist = abs(localGoal.dot(axis));
	uint32 closestIdx = 0;
	uint32 edgeIdx = 0;
	for (uint32 i = 0, len = m_vertices.size(); i < len; ++i)
	{
		const Vec2f & vertex = m_vertices[i];
		float const dist = abs((localGoal - vertex).dot(axis));
		if (dist < closestDist)
		{
			closestDist = dist;
			closestIdx = i;
			edgeIdx = 0;
		}
		else if (dist == closestDist && (i - closestIdx == 1 || i - closestIdx == m_vertices.size() - 1))
		{
			edgeIdx = i;
		}
	}

	vertices.emplace_back(m_center + m_vertices[closestIdx]);
	if (edgeIdx != 0)
		vertices.emplace_back(m_center + m_vertices[edgeIdx]);
}

Vec2f PolygonNamespace::centralize(std::vector<Vec2f> & vertices)
{
	Vec2f center;
	for (const auto & vertex : vertices)
		center += vertex;
	center /= static_cast<float>(vertices.size());

	for (auto & vertex : vertices)
		vertex -= center;

	return center;
}