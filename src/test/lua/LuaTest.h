#ifndef VIOLET_LuaTest_H
#define VIOLET_LuaTest_H

namespace Violet
{
	class TestEvaluator;

	class LuaTest
	{
	public:

		static void run(TestEvaluator & evaluator);
	};
}

#endif