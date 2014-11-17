#include "violet/plugins/physics/collision/CollisionResolver.h"
#include "violet/plugins/physics/collision/Intersection.h"

using namespace Violet;

namespace CollisionResolverNamespace
{
	void resolveShape(Shape & shape, Intersection & intersection);
}

using namespace CollisionResolverNamespace;

void CollisionResolver::resolve(Intersection & intersection)
{
	if (!intersection.exists())
		return;

	const Vec2f impulse = intersection.getIntersectionAxis() * intersection.getImpulseScalar();

	resolveShape(intersection.getShape1(), intersection);
	resolveShape(intersection.getShape2(), intersection); 
}

void CollisionResolverNamespace::resolveShape(Shape & shape, Intersection & intersection)
{
	shape.update(intersection.getTimeOfImpact());
	Vec2f const impulse = intersection.getIntersectionAxis() * intersection.getImpulseScalar();
	Vec2f const location = intersection.getImpactLocation() - shape.getCenter();
	shape.applyImpulse(impulse.dot(location) > 0 ? impulse : impulse.inverse(), location);
}