#include "test/core/testevaluator.h"
#include "test/core/testfactory.h"
#include "test/game/pathfindingtest.h"
#include "test/lua/luatest.h"
#include "test/math/mathtest.h"
#include "test/physics/physicstest.h"
#include "test/serialization/jsonserializationtest.h"
#include "test/task/tasktest.h"
#include "test/template/templatetest.h"

#include <iostream>

using namespace vlt;

int main(int argc, char** argv)
{
	{
		TestEvaluator evaluator(std::cout);

		LuaTest::run(evaluator);
		MathTests::run(evaluator);
		PhysicsTest::run(evaluator);
		JsonSerializationTests::run(evaluator);
		TemplateTests::run(evaluator);
		PathfindingTest::run(evaluator);
		TaskTest::run(evaluator);
	}

	std::cout << "Done." << std::endl;
	char c;
	std::cin >> c;
}
