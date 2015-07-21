#ifndef VIOLET_LuaLibrary_H
#define VIOLET_LuaLibrary_H

#include <lua.hpp>
#include <vector>
#include <algorithm>
#include <map>

#include "engine/Engine.h"
#include "engine/utility/unique_val.h"

namespace Violet
{
	template <typename T>
	void luaV_pushudata(lua_State * lua, const T & udata);

	template <typename T>
	void luaV_push(lua_State * lua, const T & value);

	template <>
	void VIOLET_API luaV_push(lua_State * lua, const int & value);

	class LuaMethodBase
	{
	public:

		virtual void install(lua_State * lua) = 0;
		virtual int eval(lua_State * lua) = 0;
	};

	template <typename Signature>
	class LuaMethod;

	template <typename ResultType, typename ... Args>
	class LuaMethod<ResultType(Args ...)> : public LuaMethodBase
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
	class LuaMethod<void(Args ...)> : public LuaMethodBase
	{
	public:

		LuaMethod(const char * name, std::function<void(Args...)> func);

		virtual void install(lua_State * lua) override;
		virtual int eval(lua_State * lua) override;

	private:

		const char * m_name;
		std::function<void(Args...)> m_func;
	};

	class VIOLET_API LuaLibrary
	{
	public:

		static void addMethod(unique_val<LuaMethodBase> && method);
		static void open(lua_State * lua);
		static int callFromLua(lua_State * lua);

	private:

		static std::vector<unique_val<LuaMethodBase>> ms_methods;
	};
}

#include "engine/script/lua/LuaLibrary.inl"

#endif