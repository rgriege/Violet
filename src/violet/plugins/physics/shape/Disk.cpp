#include "violet/plugins/physics/shape/Disk.h"

#include "violet/core/serialization/Deserializer.h"

using namespace Violet;

namespace DiskNamespace
{
	const char * const ms_segmentLabel = "disk";
	const char * const ms_radiusLabel = "radius";
}

using namespace DiskNamespace;

Disk::Disk(const Vec2f center, const float mass, const float radius) :
	Shape(center, mass, Vec2f::ZERO),
	m_radius(radius)
{
}

Disk::Disk(Deserializer & deserializer) :
	Shape(deserializer),
	m_radius()
{
	deserializer.enterSegment(DiskNamespace::ms_segmentLabel);
	m_radius = deserializer.getFloat(ms_radiusLabel);
	deserializer.leaveSegment();
}

void Disk::rotate(const float radians)
{
}

float Disk::calculateMomentOfInertia() const
{
	return 0.5f * m_mass * pow(m_radius, 2);
}

FloatInterval Disk::project(const Vec2f & axis) const
{
	const Vec2f & unitAxis = axis.isUnit() ? axis : axis.getUnit();
	const float dp = m_center.dot(unitAxis);
	return FloatInterval(dp - m_radius, dp + m_radius);
}

float Disk::maxRadius() const
{
	return m_radius;
}

void Disk::findIntersectionAxes(const Shape & other, std::vector<Vec2f> & axes) const
{
	axes.push_back(other.getCenter() - m_center);
}