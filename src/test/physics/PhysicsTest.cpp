#include "test/physics/physicstest.h"

#include "violet/math/poly.h"
#include "violet/physics/collision/intersection.h"
#include "test/core/testevaluator.h"
#include "test/core/testfactory.h"

#include <tuple>

using namespace vlt;

void PhysicsTest::run(TestEvaluator & evaluator)
{
	Rigid_Body r1(v2(3, 3), Poly(Vector<v2>({ v2(-2, -2), v2(2, -2), v2(2, 2), v2(-2, 2) })), 1.f);
	Rigid_Body r2(v2(8, 3), Poly(Vector<v2>({ v2(-2, -2), v2(2, -2), v2(2, 2), v2(-2, 2) })), 1.f);
	r2.applyImpulse(v2(-2, 0));
	Intersection firstIntersection(std::move(r1), std::move(r2), 1.f);

	TestFactory::makeStatefulSuite("physics tests", std::move(firstIntersection), std::forward_as_tuple(
		TestFactory::makeStateful<Intersection>("rec-rec Intersection exists", true, [](intersection & i) { return i.exists(); }),
		TestFactory::makeStateful<Intersection>("rec-rec Intersection axis", v2::X_Axis, [](intersection & i)->v2 { return i.getIntersectionAxis(); }),
		TestFactory::makeStateful<Intersection>("rec-rec Intersection location", v2(5, 3), [](intersection & i)->v2 { return i.getImpactLocation(); }),
		TestFactory::makeStateful<Intersection>("rec-rec Intersection Timestamp", 0.5f, [](intersection & i) { return i.getTimeOfImpact(); })
	)).evaluate(evaluator);
}
