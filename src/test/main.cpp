#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"
#include "test/game/PathfindingTest.h"
#include "test/math/MathTest.h"
#include "test/physics/PhysicsTest.h"
#include "test/serialization/JsonSerializationTest.h"
#include "test/template/TemplateTest.h"

#include <iostream>

using namespace Violet;

int main(int argc, char** argv)
{
	{
		TestEvaluator evaluator(std::cout);

		MathTests::run(evaluator);
		PhysicsTest::run(evaluator);
		JsonSerializationTests::run(evaluator);
		TemplateTests::run(evaluator);
		PathfindingTest::run(evaluator);
	}

	std::cout << "Done." << std::endl;
	char c;
	std::cin >> c;
}
