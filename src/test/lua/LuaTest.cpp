// ============================================================================

#include "test/lua/luatest.h"

#include "violet/script/lua/lualibrary.h"
#include "violet/script/lua/lua_script.h"
#include "test/core/testevaluator.h"
#include "test/core/testfactory.h"

#include <tuple>

using namespace vlt;

namespace LuaTestNamespace
{
	// ----------------------------------------------------------------------------

	Shared_Val<Text_Resource> m_validScriptSource = make_shared_val<string_resource>("valid.inl",
		"function init()\n"
		"	sayHiInC()\n"
		"end\n"
		"function get42()\n"
		"	return 42\n"
		"end\n"
		"function get42FromC()\n"
		"   return get42InC()\n"
		"end\n"
		"function add5(x)\n"
		"	return x + 5\n"
		"end\n"
		"function add5FromC(x)\n"
		"	return add5InC(x)\n"
		"end\n"
		"function sum(x, y)\n"
		"	return x + y\n"
		"end\n"
		"function getSumFromC(x, y)\n"
		"   return addInC(x, y)\n"
		"end\n"
		"function clean() end\n");

	Shared_Val<Text_Resource> m_invalidScriptSource = make_shared_val<string_resource>("invalid.inl",
		"function sayHi(\n"
		"	sayHiInC()\n"
		"end\n");

	Shared_Val<Text_Resource> m_noInitScriptSource = make_shared_val<string_resource>("noInit.inl",
		"function sayHi()\n"
		"	sayHiInC()\n"
		"end\n");

	bool saidHi = false;

	// ----------------------------------------------------------------------------

	void sayHiFromLua();
	int get42();
	int add5(int x);
	int add(int x, int y);

	// ----------------------------------------------------------------------------
}

using namespace LuaTestNamespace;

// ============================================================================

void LuaTest::run(TestEvaluator & evaluator)
{
	vlt::LuaLibrary::addMethod(vlt::make_unique_val<vlt::LuaMethod<void(void)>>("sayHiInC", sayHiFromLua));
	vlt::LuaLibrary::addMethod(vlt::make_unique_val<vlt::LuaMethod<int(void)>>("get42InC", get42));
	vlt::LuaLibrary::addMethod(vlt::make_unique_val<vlt::LuaMethod<int(int)>>("add5InC", add5));
	vlt::LuaLibrary::addMethod(vlt::make_unique_val<vlt::LuaMethod<int(int, int)>>("addInC", add));

	TestFactory::makeStatelessSuite("lua tests", std::forward_as_tuple(
		TestFactory::makeStatelessSuite("load tests", std::forward_as_tuple(
			TestFactory::makeStateless("valid", true, []() { return Lua_Script(m_validScriptSource).is_valid(); }),
			TestFactory::makeStateless("invalid", false, []() { return Lua_Script(m_invalidScriptSource).is_valid(); }),
			TestFactory::makeStateless("no init", false, []() { return Lua_Script(m_noInitScriptSource).is_valid(); })
		)),
		TestFactory::makeStatelessSuite("lua function calls", std::forward_as_tuple(
			TestFactory::makeStateless("int(void)", 42, []() { return Lua_Script(m_validScriptSource).run<int>("get42"); }),
			TestFactory::makeStateless("int(int)", 42, []() { return Lua_Script(m_validScriptSource).run<int>("add5", 37); }),
			TestFactory::makeStateless("int(int, int)", 42, []() { return Lua_Script(m_validScriptSource).run<int>("sum", 14, 28); })
		)),
		TestFactory::makeStatelessSuite("c function calls", std::forward_as_tuple(
			TestFactory::makeStateless("void(void)", true, []() { return saidHi; }),
			TestFactory::makeStateless("int(void)", 42, []() { return Lua_Script(m_validScriptSource).run<int>("get42FromC"); }),
			TestFactory::makeStateless("int(int)", 42, []() { return Lua_Script(m_validScriptSource).run<int>("add5FromC", 37); }),
			TestFactory::makeStateless("int(int, int)", 42, []() { return Lua_Script(m_validScriptSource).run<int>("getSumFromC", 14, 28); })
		))
	)).evaluate(evaluator);
}

// ============================================================================

void LuaTestNamespace::sayHiFromLua()
{
	saidHi = true;
}

// ----------------------------------------------------------------------------

int LuaTestNamespace::get42()
{
	return 42;
}

// ----------------------------------------------------------------------------

int LuaTestNamespace::add5(const int x)
{
	return x + 5;
}

// ----------------------------------------------------------------------------

int LuaTestNamespace::add(const int x, const int y)
{
	return x + y;
}

// ============================================================================
