// ============================================================================

#include "engine/script/lua/LuaLibrary.h"

// ============================================================================

namespace Violet
{
	namespace detail
	{
		template <typename ResultType, typename ... Args>
		struct Runner;

		template <>
		struct Runner<void>
		{
			static void run(lua_State * lua, const char * method, const int argc)
			{
				lua_pcall(lua, argc, 0, 0);
			}
		};

		template <typename ResultType>
		struct Runner<ResultType>
		{
			static ResultType run(lua_State * lua, const char * method, const int argc)
			{
				lua_pcall(lua, argc, 1, 0);
				return luaV_pop<ResultType>(lua);
			}
		};

		template <typename ResultType, typename Arg, typename ... Args>
		struct Runner<ResultType, Arg, Args...>
		{
			static ResultType run(lua_State * lua, const char * method, const int argc, Arg && arg, Args && ... args)
			{
				luaV_push(lua, arg);
				return Runner<ResultType, Args...>::run(lua, method, argc, std::forward<Args>(args)...);
			}
		};
	}
}

// ============================================================================

template <typename ResultType, typename... Args>
ResultType Violet::LuaScript::run(const char * method, Args && ... args) const
{
	if (isValid())
	{
		lua_getglobal(m_lua, method);
		return detail::Runner<ResultType, Args...>::run(m_lua, method, sizeof...(args), std::forward<Args>(args)...);
	}

	return ResultType{};
}

// ============================================================================
