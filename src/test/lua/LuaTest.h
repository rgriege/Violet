#ifndef VIOLET_LuaTest_H
#define VIOLET_LuaTest_H

namespace vlt
{
	struct TestEvaluator;

	struct LuaTest
	{
	public:

		static void run(TestEvaluator & evaluator);
	};
}

#endif