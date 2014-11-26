#include "violet/test/physics/PhysicsTest.h"

#include "violet/core/utility/FormattedString.h"
#include "violet/core/math/Polygon.h"
#include "violet/plugins/physics/collision/Intersection.h"
#include "violet/test/core/TestEvaluator.h"
#include "violet/test/core/TestFactory.h"

#include <tuple>

using namespace Violet;

void PhysicsTest::run(TestEvaluator & evaluator)
{
	RigidBody r1(Vec2f(3, 3), Polygon(std::vector<Vec2f>({ Vec2f(-2, -2), Vec2f(2, -2), Vec2f(2, 2), Vec2f(-2, 2) })), 1.f);
	RigidBody r2(Vec2f(8, 3), Polygon(std::vector<Vec2f>({ Vec2f(-2, -2), Vec2f(2, -2), Vec2f(2, 2), Vec2f(-2, 2) })), 1.f);
	r2.applyImpulse(Vec2f(-2, 0));
	Intersection firstIntersection(std::move(r1), std::move(r2), 1.f);

	TestFactory::makeStatefulSuite("physics tests", std::move(firstIntersection), std::forward_as_tuple(
		TestFactory::makeStateful<Intersection>("rec-rec intersection exists", true, [](Intersection & i) { return i.exists(); }),
		TestFactory::makeStateful<Intersection>("rec-rec intersection axis", Vec2f::X_AXIS, [](Intersection & i) { return i.getIntersectionAxis(); }),
		TestFactory::makeStateful<Intersection>("rec-rec intersection location", Vec2f(5, 3), [](Intersection & i) { return i.getImpactLocation(); }),
		TestFactory::makeStateful<Intersection>("rec-rec intersection time", 0.5f, [](Intersection & i) { return i.getTimeOfImpact(); })
	)).evaluate(evaluator);
}
