#include "violet/test/core/TestEvaluator.h"
#include "violet/test/core/TestFactory.h"
#include "violet/test/math/MathTest.h"
#include "violet/test/physics/PhysicsTest.h"
#include "violet/test/serialization/JsonSerializationTest.h"
#include "violet/test/template/TemplateTest.h"

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
	}

	std::cout << "Done." << std::endl;
	char c;
	std::cin >> c;
}
