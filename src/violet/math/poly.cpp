// ============================================================================

#include "violet/math/interval.h"
#include "violet/math/m2.h"
#include "violet/math/poly.h"
#include "violet/serialization/serialization_utilities.h"

using namespace vlt;

// ============================================================================

const char * const ms_segmentLabel("poly");

// ============================================================================

poly::poly(vector<v2> && vertices) :
	vertices(std::move(vertices))
{
}

// ----------------------------------------------------------------------------

poly::poly(deserializer & deserializer) :
	vertices()
{
	auto polygonSegment = deserializer.enter_segment(ms_segmentLabel);
	vertices = deserialize_vector<v2>(*polygonSegment);
}

// ----------------------------------------------------------------------------

poly::poly(const aabb & box) :
	vertices()
{
	vertices.emplace_back(box.bottom_right);
	vertices.emplace_back(box.bottom_right.x, box.top_left.y);
	vertices.emplace_back(box.top_left);
	vertices.emplace_back(box.top_left.x, box.bottom_right.y);
}

// ----------------------------------------------------------------------------

bool poly::contains(const v2 & point) const
{
	static const interval interval(0, 1);
	bool inside = false;
	u32 const n = vertices.size();

	{
		const v2 & p1 = vertices[n - 1];
		const v2 & p2 = vertices[0];
		const v2 barycentricCoords = m2(p1.x, p2.x, p1.y, p2.y).inverse() * point;
		inside = interval.contains(barycentricCoords.x) && interval.contains(barycentricCoords.y) && barycentricCoords.x + barycentricCoords.y <= 1;
	}

	u32 i = 1;
	while (!inside && i < n)
	{
		const v2 & p1 = vertices[i];
		const v2 & p2 = vertices[i - 1];
		const v2 barycentricCoords = m2(p1.x, p2.x, p1.y, p2.y).inverse() * point;
		inside = interval.contains(barycentricCoords.x) && interval.contains(barycentricCoords.y) && barycentricCoords.x + barycentricCoords.y <= 1;
		++i;
	}

	return inside;
}

// ----------------------------------------------------------------------------

aabb poly::get_bounding_box() const
{
	aabb result{ get_center(), v2::Zero };
	for (auto const & vertex : vertices)
		result.extend(vertex);
	return result;
}

// ----------------------------------------------------------------------------

void poly::translate(const v2 delta)
{
	for (auto & vertex : vertices)
		vertex += delta;
}

// ----------------------------------------------------------------------------

interval poly::project(const v2 & axis) const
{
	const v2 & unitAxis = axis.is_unit() ? axis : axis.get_unit();
	interval projection;
	for (const auto & vertex : vertices)
	{
		const r32 dp = vertex.dot(unitAxis);
		if (dp < projection.left)
			projection.left = dp;
		else if (dp > projection.right)
			projection.right = dp;
	}
	return projection;
}

// ----------------------------------------------------------------------------

v2 poly::get_center() const
{
	v2 center;
	for (auto const & vertex : vertices)
		center += vertex;
	return center / static_cast<r32>(vertices.size());
}

// ----------------------------------------------------------------------------

v2 & poly::operator[](u32 const index)
{
	return vertices[index];
}

// ----------------------------------------------------------------------------

const v2 & poly::operator[](u32 const index) const
{
	return vertices[index];
}

// ----------------------------------------------------------------------------

vector<v2>::iterator poly::begin()
{
	return vertices.begin();
}

// ----------------------------------------------------------------------------

vector<v2>::iterator poly::end()
{
	return vertices.end();
}

// ============================================================================

serializer & vlt::operator<<(serializer & serializer, const poly & poly)
{
	auto segment = serializer.create_segment(ms_segmentLabel);
	serialize_vector(*segment, poly.vertices);
	return serializer;
}

// ----------------------------------------------------------------------------

deserializer & vlt::operator>>(deserializer & deserializer, poly & poly)
{
	auto segment = deserializer.enter_segment(ms_segmentLabel);
	poly.vertices = deserialize_vector<v2>(*segment);
	return deserializer;
}

// ============================================================================
