#include "violet/test/math/MathTest.h"

#include "violet/core/math/Interval.h"
#include "violet/core/math/Matrix2.h"
#include "violet/core/math/Polygon.h"
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
		TestFactory::makeStateless("interval not overlapping", 0.f, []() { return FloatInterval(0, 5).overlap(FloatInterval(5, 8)); }),
		TestFactory::makeStateless("matrix determinant", -2, []() { return Matrix2i(2, -5, -2, 4).determinant(); }),
		TestFactory::makeStateless("matrix inverse", Matrix2f(-2, -2.5, -1, -1), []() { return Matrix2f(2, -5, -2, 4).inverse(); }),
		TestFactory::makeStateless("matrix vector multiplication", Vec2f(1, -1), []() { return Matrix2f(-2, -2.5, -1, -1) * Vec2f(7, -6); }),
		TestFactory::makeStateless("Matrix Ax=b", Vec2f(1, -1), []() { return Matrix2f(2, -5, -2, 4).inverse() * Vec2f(7, -6); }),
		TestFactory::makeStateless("Polygon contains", true, []() { return Polygon({ { -1, -1 }, { 1, -1 }, { 0, 2 } }).contains(Vec2f(0, 1)); })
	)).evaluate(evaluator);
}