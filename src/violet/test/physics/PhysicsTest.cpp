#include "violet/test/physics/PhysicsTest.h"

#include "violet/core/utility/FormattedString.h"
#include "violet/plugins/core/math/Polygon.h"
#include "violet/plugins/physics/collision/Intersection.h"
#include "violet/test/core/TestEvaluator.h"
#include "violet/test/core/TestFactory.h"

#include <tuple>

using namespace Violet;

void PhysicsTest::run(TestEvaluator & evaluator)
{
	Polygon r1(1.f, std::vector<Vec2f>({ Vec2f(1, 1), Vec2f(5, 1), Vec2f(5, 5), Vec2f(1, 5) }));
	Polygon r2(1.f, std::vector<Vec2f>({ Vec2f(6, 1), Vec2f(10, 1), Vec2f(10, 5), Vec2f(6, 5) }));
	r2.applyImpulse(Vec2f(-2, 0));
	Intersection firstIntersection(r1, r2, 1.f);

	TestFactory::makeStatefulSuite("physics tests", std::move(firstIntersection), std::forward_as_tuple(
		TestFactory::makeStateful<Intersection>("rec-rec intersection exists", true, [](Intersection & i) { return i.exists(); }),
		TestFactory::makeStateful<Intersection>("rec-rec intersection axis", Vec2f::X_AXIS, [](Intersection & i) { return i.getIntersectionAxis(); }),
		TestFactory::makeStateful<Intersection>("rec-rec intersection location", Vec2f(5, 3), [](Intersection & i) { return i.getImpactLocation(); }),
		TestFactory::makeStateful<Intersection>("rec-rec intersection time", 0.5f, [](Intersection & i) { return i.getTimeOfImpact(); })
	)).evaluate(evaluator);
}
