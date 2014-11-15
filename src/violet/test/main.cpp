#include <iostream>
#include <tuple>

#include "violet/test/math/MathTest.h"
#include "violet/test/serialization/JsonSerializationTest.h"

using namespace Violet;

#include "violet/test/core/TestEvaluator.h"
#include "violet/test/core/TestFactory.h"

using namespace Violet;

int main(int argc, char** argv) {

	{
		TestEvaluator evaluator(std::cout);

		MathTests::run(evaluator);
		JsonSerializationTests::run(evaluator);
	}

	std::cout << "Done." << std::endl;
	char c;
	std::cin >> c;
}
