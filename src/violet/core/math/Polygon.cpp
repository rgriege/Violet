#include "violet/core/math/Polygon.h"

#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

namespace PolygonNamespace
{
	const char * const ms_segmentLabel("poly");
	const char * const ms_sizeLabel("n");
	const char * const ms_verticesLabel("vertices");

	std::vector<Vec2f> deserializeVertices(Violet::Deserializer & deserializer);
	Vec2f findCenter(const std::vector<Vec2f> & vertices);
}

using namespace PolygonNamespace;

Polygon::Polygon(std::vector<Vec2f> _vertices) :
	vertices(_vertices),
	center(findCenter(vertices))
{
}

Polygon::Polygon(Deserializer & deserializer) :
	vertices(deserializeVertices(deserializer)),
	center(findCenter(vertices))
{
}

FloatInterval Polygon::project(const Vec2f & vec) const
{
	const Vec2f & axis = vec.isUnit() ? vec : vec.getUnit();

	float vertex_projection = vertices[0].dot(axis);
	FloatInterval result(vertex_projection, vertex_projection);
	size_t len = vertices.size();
	for (size_t i = 1; i < len; i++) {
		vertex_projection = vertices[i].dot(axis);
		if (vertex_projection < result.left)
			result.left = vertex_projection;
		else if (vertex_projection > result.right)
			result.right = vertex_projection;
	}
	result.left = result.left + center.dot(axis);
	result.right = result.right + center.dot(axis);
	return result;
}

std::vector<Vec2f> Polygon::getEdgePerps() const
{
	std::vector<Vec2f> result;
	size_t len = vertices.size();
	result.push_back((vertices[0] - vertices[len-1]).perpendicular(true));
	for (size_t i = 1; i < len; i++) {
		result.push_back((vertices[i] - vertices[i-1]).perpendicular(true));
	}
	return result;
}

const Vec2f & Polygon::getClosestVertex(const Vec2f & target) const {
	float min_dist = (vertices[0]+center-target).magSquared();
	Vec2f result = vertices[0];
	size_t len = vertices.size();
	for (size_t i = 1; i < len; i++) {
		float const dist = (vertices[i]+center-target).magSquared();
		if(dist < min_dist) {
			min_dist = dist;
			result = vertices[i];
		}
	}
	return result+center;
}

std::vector<Vec2f> PolygonNamespace::deserializeVertices(Deserializer & deserializer)
{
	deserializer.enterSegment(ms_segmentLabel);
	uint32 n = deserializer.getUint(ms_sizeLabel);
	std::vector<Vec2f> vertices;
	vertices.reserve(n);
	deserializer.enterSegment(ms_verticesLabel);
	for (uint32 i = 0; i < n; ++i)
	{
		deserializer.enterSegment(nullptr);
		vertices.emplace_back(deserializer);
		deserializer.leaveSegment();
	}
	deserializer.leaveSegment();
	deserializer.leaveSegment();
	return vertices;
}

Vec2f PolygonNamespace::findCenter(const std::vector<Vec2f> & vertices)
{
	Vec2f center;
	for (auto const & vertex : vertices)
		center += vertex;
	return center /= static_cast<float>(vertices.size());
}