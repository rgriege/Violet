#include "test/physics/PhysicsTest.h"

#include "violet/math/poly.h"
#include "violet/physics/collision/intersection.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <tuple>

using namespace vlt;

void PhysicsTest::run(TestEvaluator & evaluator)
{
	rigid_body r1(v2(3, 3), poly(vector<v2>({ v2(-2, -2), v2(2, -2), v2(2, 2), v2(-2, 2) })), 1.f);
	rigid_body r2(v2(8, 3), poly(vector<v2>({ v2(-2, -2), v2(2, -2), v2(2, 2), v2(-2, 2) })), 1.f);
	r2.applyImpulse(v2(-2, 0));
	intersection firstIntersection(std::move(r1), std::move(r2), 1.f);

	TestFactory::makeStatefulSuite("physics tests", std::move(firstIntersection), std::forward_as_tuple(
		TestFactory::makeStateful<intersection>("rec-rec intersection exists", true, [](intersection & i) { return i.exists(); }),
		TestFactory::makeStateful<intersection>("rec-rec intersection axis", v2::X_Axis, [](intersection & i)->v2 { return i.getIntersectionAxis(); }),
		TestFactory::makeStateful<intersection>("rec-rec intersection location", v2(5, 3), [](intersection & i)->v2 { return i.getImpactLocation(); }),
		TestFactory::makeStateful<intersection>("rec-rec intersection timestamp", 0.5f, [](intersection & i) { return i.getTimeOfImpact(); })
	)).evaluate(evaluator);
}
