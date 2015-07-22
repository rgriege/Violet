// ============================================================================

#include "test/lua/LuaTest.h"

#include "engine/script/lua/LuaLibrary.h"
#include "engine/script/lua/LuaScript.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <tuple>

using namespace Violet;

namespace LuaTestNamespace
{
	// ----------------------------------------------------------------------------

	shared_val<TextResource> m_validScriptSource = make_shared_val<StringResource>("valid.inl",
		"function init()\n"
		"	sayHiInC()\n"
		"end\n"
		"function clean() end\n");

	shared_val<TextResource> m_invalidScriptSource = make_shared_val<StringResource>("invalid.inl",
		"function sayHi(\n"
		"	sayHiInC()\n"
		"end\n");

	shared_val<TextResource> m_noInitScriptSource = make_shared_val<StringResource>("noInit.inl",
		"function sayHi()\n"
		"	sayHiInC()\n"
		"end\n");

	bool saidHi = false;

	// ----------------------------------------------------------------------------

	void sayHiFromLua();
	int divideFromLua(int x, int y);

	// ----------------------------------------------------------------------------
}

using namespace LuaTestNamespace;

// ============================================================================

void LuaTest::run(TestEvaluator & evaluator)
{
	Violet::LuaLibrary::addMethod(Violet::make_unique_val<Violet::LuaMethod<void(void)>>("sayHiInC", sayHiFromLua));

	TestFactory::makeStatelessSuite("lua tests", std::forward_as_tuple(
		TestFactory::makeStatelessSuite("load tests", std::forward_as_tuple(
			TestFactory::makeStateless("valid", true, []() { return LuaScript(m_validScriptSource).isValid(); }),
			TestFactory::makeStateless("invalid", false, []() { return LuaScript(m_invalidScriptSource).isValid(); }),
			TestFactory::makeStateless("no init", false, []() { return LuaScript(m_noInitScriptSource).isValid(); })
		)),
		TestFactory::makeStatelessSuite("c function calls", std::forward_as_tuple(
			TestFactory::makeStateless("void(void)", true, []() { return saidHi; })
		))
	)).evaluate(evaluator);
}

// ============================================================================

void LuaTestNamespace::sayHiFromLua()
{
	saidHi = true;
}

// ----------------------------------------------------------------------------

int LuaTestNamespace::divideFromLua(const int x, const int y)
{
	return x / y;
}

// ============================================================================
