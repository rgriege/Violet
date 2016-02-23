// ============================================================================

#include "violet/math/interval.h"
#include "violet/math/m2.h"
#include "violet/math/poly.h"
#include "violet/serialization/serialization_utilities.h"

using namespace vlt;

// ============================================================================

const char * const ms_segmentLabel("Poly");

// ============================================================================

Poly::Poly(Vector<v2> && vertices) :
	vertices(std::move(vertices))
{
}

// ----------------------------------------------------------------------------

Poly::Poly(Deserializer & deserializer) :
	vertices()
{
	auto polygonSegment = deserializer.enter_segment(ms_segmentLabel);
	vertices = deserialize_vector<v2>(*polygonSegment);
}

// ----------------------------------------------------------------------------

Poly::Poly(const aabb & box) :
	vertices()
{
	vertices.emplace_back(box.bottom_right);
	vertices.emplace_back(box.bottom_right.x, box.top_left.y);
	vertices.emplace_back(box.top_left);
	vertices.emplace_back(box.top_left.x, box.bottom_right.y);
}

// ----------------------------------------------------------------------------

bool Poly::contains(const v2 & point) const
{
	static const Interval interval(0, 1);
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

aabb Poly::get_bounding_box() const
{
	aabb result{ get_center(), v2::Zero };
	for (auto const & vertex : vertices)
		result.extend(vertex);
	return result;
}

// ----------------------------------------------------------------------------

void Poly::translate(const v2 delta)
{
	for (auto & vertex : vertices)
		vertex += delta;
}

// ----------------------------------------------------------------------------

Interval Poly::project(const v2 & axis) const
{
	const v2 & unitAxis = axis.is_unit() ? axis : axis.get_unit();
	Interval projection;
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

v2 Poly::get_center() const
{
	v2 center;
	for (auto const & vertex : vertices)
		center += vertex;
	return center / static_cast<r32>(vertices.size());
}

// ----------------------------------------------------------------------------

v2 & Poly::operator[](u32 const index)
{
	return vertices[index];
}

// ----------------------------------------------------------------------------

const v2 & Poly::operator[](u32 const index) const
{
	return vertices[index];
}

// ----------------------------------------------------------------------------

Vector<v2>::iterator Poly::begin()
{
	return vertices.begin();
}

// ----------------------------------------------------------------------------

Vector<v2>::iterator Poly::end()
{
	return vertices.end();
}

// ============================================================================

Serializer & vlt::operator<<(Serializer & serializer, const Poly & poly)
{
	auto segment = serializer.create_segment(ms_segmentLabel);
	serialize_vector(*segment, poly.vertices);
	return serializer;
}

// ----------------------------------------------------------------------------

Deserializer & vlt::operator>>(Deserializer & deserializer, Poly & poly)
{
	auto segment = deserializer.enter_segment(ms_segmentLabel);
	poly.vertices = deserialize_vector<v2>(*segment);
	return deserializer;
}

// ============================================================================
