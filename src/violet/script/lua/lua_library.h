#ifndef VIOLET_LUA_LIBRARY_H
#define VIOLET_LUA_LIBRARY_H

#include <algorithm>
#include <lua.hpp>
#include <map>
#include <vector>

#include "violet/core/engine.h"
#include "violet/utility/unique_val.h"

namespace vlt
{
	template <typename T>
	void luaV_pushudata(lua_State * lua, const T & udata);

	template <typename T>
	void luaV_push(lua_State * lua, const T & value);

	template <>
	void VIOLET_API luaV_push(lua_State * lua, const bool & value);

	template <>
	void VIOLET_API luaV_push(lua_State * lua, const int & value);

	template <>
	void VIOLET_API luaV_push(lua_State * lua, const r64 & value);

	template <>
	void VIOLET_API luaV_push(lua_State * lua, const std::string & value);


	template <typename T>
	T luaV_pop(lua_State * lua);

	template <>
	bool VIOLET_API luaV_pop(lua_State * lua);

	template <>
	int VIOLET_API luaV_pop(lua_State * lua);

	template <>
	r64 VIOLET_API luaV_pop(lua_State * lua);

	template <>
	std::string VIOLET_API luaV_pop(lua_State * lua);

	struct LuaMethodBase
	{
	public:

		virtual void install(lua_State * lua) = 0;
		virtual int eval(lua_State * lua) = 0;
	};

	template <typename Signature>
	struct LuaMethod;

	template <typename ResultType, typename ... Args>
	struct LuaMethod<ResultType(Args ...)> : public LuaMethodBase
	{
	public:

		LuaMethod(const char * name, std::function<ResultType(Args...)> func);

		virtual void install(lua_State * lua) override;
		virtual int eval(lua_State * lua) override;

	private:

		const char * m_name;
		std::function<ResultType(Args...)> m_func;
	};

	template <typename ... Args>
	struct LuaMethod<void(Args ...)> : public LuaMethodBase
	{
	public:

		LuaMethod(const char * name, std::function<void(Args...)> func);

		virtual void install(lua_State * lua) override;
		virtual int eval(lua_State * lua) override;

	private:

		const char * m_name;
		std::function<void(Args...)> m_func;
	};

	struct VIOLET_API LuaLibrary
	{
	public:

		static void addMethod(unique_val<LuaMethodBase> && method);
		static void open(lua_State * lua);
		static int callFromLua(lua_State * lua);

	private:

		static std::vector<unique_val<LuaMethodBase>> ms_methods;
	};
}

#include "violet/script/lua/lua_library.inl"

#endif
