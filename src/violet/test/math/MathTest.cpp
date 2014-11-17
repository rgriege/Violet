#include "violet/test/math/MathTest.h"

#include "violet/core/math/Interval.h"
#include "violet/core/math/Vec2.h"
#include "violet/test/core/TestEvaluator.h"
#include "violet/test/core/TestFactory.h"

#include <tuple>

using namespace Violet;

void MathTests::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("math tests", std::forward_as_tuple(
		TestFactory::makeStateless("projection formula", Vec2f(4, 5).getUnit() * Vec2f(7, 2).dot(Vec2f(4, 5)), []() { return Vec2f(7, 2).project(Vec2f(4, 5)); }),
		TestFactory::makeStateless("x-axis projection", Vec2f(13, 0), []() { return Vec2f(13, 4).project(Vec2f::X_AXIS); }),
		TestFactory::makeStateless("interval overlapping", 3.f, []() { return FloatInterval(0, 5).overlap(FloatInterval(2, 8)); }),
		TestFactory::makeStateless("interval enclosed", 3.f, []() { return FloatInterval(0, 5).overlap(FloatInterval(1, 4)); }),
		TestFactory::makeStateless("interval not overlapping", 0.f, []() { return FloatInterval(0, 5).overlap(FloatInterval(5, 8)); })
	)).evaluate(evaluator);
}