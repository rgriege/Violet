#include "test/math/MathTest.h"

#include "violet/math/interval.h"
#include "violet/math/m2.h"
#include "violet/math/poly.h"
#include "violet/math/v2.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <tuple>

using namespace vlt;

void MathTests::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("math tests", std::forward_as_tuple(
		TestFactory::makeStateless("projection formula", v2(4, 5).get_unit() * v2(7, 2).dot(v2(4, 5)), []() { return v2(7, 2).project(v2(4, 5)); }),
		TestFactory::makeStateless("x-axis projection", v2(13, 0), []() { return v2(13, 4).project(v2::X_Axis); }),
		TestFactory::makeStateless("interval overlapping", 3.f, []() { return interval(0, 5).overlap(interval(2, 8)); }),
		TestFactory::makeStateless("interval enclosed", 3.f, []() { return interval(0, 5).overlap(interval(1, 4)); }),
		TestFactory::makeStateless("interval not overlapping", 0.f, []() { return interval(0, 5).overlap(interval(5, 8)); }),
		TestFactory::makeStateless("matrix determinant", -2, []() { return Matrix2i(2, -5, -2, 4).determinant(); }),
		TestFactory::makeStateless("matrix inverse", m2(-2, -2.5, -1, -1), []() { return m2(2, -5, -2, 4).inverse(); }),
		TestFactory::makeStateless("matrix vector multiplication", v2(1, -1), []() { return m2(-2, -2.5, -1, -1) * v2(7, -6); }),
		TestFactory::makeStateless("Matrix Ax=b", v2(1, -1), []() { return m2(2, -5, -2, 4).inverse() * v2(7, -6); }),
		TestFactory::makeStateless("poly contains", true, []() { return poly({ { -1, -1 }, { 1, -1 }, { 0, 2 } }).contains(v2(0, 1)); })
	)).evaluate(evaluator);
}